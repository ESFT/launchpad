#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <math.h>

#include "inc/hw_i2c.h"
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"

#include "driverlib/adc.h"
#include "driverlib/debug.h"
#include "driverlib/fpu.h"
#include "driverlib/gpio.h"
#include "driverlib/i2c.h"
#include "driverlib/interrupt.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"

#include "altimeter.h"
#include "flashstore.h"
#include "uartstdio.h"

//*****************************************************************************
//
// Defines
//
//*****************************************************************************

// Altimeter
#define ALT_ADDRESS ALT_ADDRESS_CSB_LO

// I2C
#define I2C_MODE_WRITE false
#define I2C_MODE_READ true
#define I2C_SPEED_100 false
#define I2C_SPEED_400 true

// LED Colors
#define RED_LED GPIO_PIN_1
#define BLUE_LED GPIO_PIN_2
#define GREEN_LED GPIO_PIN_3
#define YELLOW_LED RED_LED | GREEN_LED
#define MAGENTA_LED RED_LED | BLUE_LED
#define CYAN_LED GREEN_LED | BLUE_LED
#define WHITE_LED RED_LED | GREEN_LED | BLUE_LED

//*****************************************************************************
//
// "Beep" Codes (Uses LED)
// See ESFT Error Codes Spreadsheet for more information
//
//*****************************************************************************
typedef enum BEEPCODE {
  GOOD, DRL_ERR, ALT_CRC_ERR, OUT_OF_FLASH,
  ALT_RESET_ERR, ALT_PROM_R_WRITE_ERR, ALT_PROM_R_READ_ERR,
  ALT_ADC_CONV_ERR, ALT_ADC_R_WRITE_ERR, ALT_ADC_R_READ_ERR
  } BeepCode;

//*****************************************************************************
//
// Prototypes
//
//*****************************************************************************
void delay(uint32_t ms);
void beep(BeepCode code);
void LEDOn(uint8_t color);
void LEDOff(uint8_t color);
void LEDBlink(uint8_t color, uint32_t time);
void LEDInit(void);
void consoleInit(void);
void gpsInit(void);
void accelInit(void);
void altInit(void);
void gpsReceive(uint8_t* buffer, uint32_t* bIndex);
bool altADCConversion(uint8_t alt_cmd, uint32_t* adc_conv);
uint8_t altCRC4(uint16_t n_prom[]);
bool altProm(uint16_t C[8]);
bool altReset(void);
uint32_t I2CRead();
uint32_t I2CWrite(uint8_t ucValue);
uint32_t I2CBurstRead(uint32_t* cReadData, uint32_t uiSize);
uint32_t I2CBurstWrite(uint8_t* cSendData, uint32_t uiSize);

//*****************************************************************************
//
// The error routine that is called if the driver library encounters an error.
//
//*****************************************************************************
#ifdef DEBUG
void
__error__(char *pcFilename, uint32_t ui32Line) {

  while(true) {
    UARTprintf("Error at line %d of %s\n", ui32Line, pcFilename);
    beep(DRL_ERR);
  };
}
#endif

//*****************************************************************************
//
// Main function
//
//*****************************************************************************
int32_t
main(void) {
  //
  // Enable the floating-point unit
  //
  MAP_FPUEnable();

  //
  // Configure the floating-point unit to perform lazy stacking of the floating-point state.
  //
  MAP_FPULazyStackingEnable();

  //
  // Set the clocking to run directly from the crystal.
  //
  MAP_SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN |
                     SYSCTL_XTAL_16MHZ);

  ///////////////////Initialize//////////////////////////////

  //
  // Enable the LED
  //
  LEDInit();

  //
  // Enable the Console IO
  //
  consoleInit();

  //
  // Enable the GPS
  //
  gpsInit();

  //
  // Enable the accelerometer
  //
  accelInit();

  //
  // Enable the altimeter
  // TODO: fix code to read from altimeter
  //
  altInit();

  //
  // Enable flash storage
  //
  uint32_t FreeSpaceAvailable = flashstoreInit();

  //
  // Enable processor interrupts.
  //
  MAP_IntMasterEnable();

  //
  // Flash storage variables
  //
  uint32_t flashStartAddr = FLASH_STORE_START_ADDR; // Starting address of flash storage
  uint32_t flashEndAddr = FLASH_STORE_END_ADDR; // Ending address of flash storage
  uint32_t flashCurrAddr = flashStartAddr; // Current address in flash storage for read. Initialize to starting address
  uint32_t flashHeader = FLASH_STORE_RECORD_HEADER; // Magic Header Byte to find record beginning
  uint32_t flashPackedChar; // 4 Byte return from storage. Theoretically holds 4 packed chars
  uint32_t flashRecordSize; // Size of the current flash record

  //
  // GPS Variables
  //
  uint32_t gpsBufferIndex;
  uint8_t gpsBuffer[128]; /*
  * GPS throws max of 87 chars (TextCRLF) ( 87 chars = 87 bytes )
  * $GPGGA,HHMMSS.SS,DDMM.MMMMM,K,DDDMM.MMMMM,L,N,QQ,PP.P,AAAA.AA,M,+XX.XX,M,SSS,RRRR*CC<CR><LF>
  * Strip *CC<CR><LF>            ( 87 - 5 = 82 chars = 82 bytes )
  * $GPGGA,HHMMSS.SS,DDMM.MMMMM,K,DDDMM.MMMMM,L,N,QQ,PP.P,AAAA.AA,M,+XX.XX,M,SSS,RRRR
  * Add 6 chars for HAM callsign ( 82 + 6 = 88 chars = 88 bytes)
  * $GPGGA,HHMMSS.SS,DDMM.MMMMM,K,DDDMM.MMMMM,L,N,QQ,PP.P,AAAA.AA,M,+XX.XX,M,SSS,RRRRKR0KCT
  * Round to near 4 bytes for flash (88 Bytes = 22 packed uint32_t)
  * Add some padding just in case (128 bytes)
  */

  //
  // Accelerometer variables
  //
  uint32_t accelData;

  //
  // Altimeter variables
  //
  // Calibration data
  uint16_t altCalibration[8]; // calibration coefficients
  uint8_t  altCRC; // calculated CRC
  // Digital pressure and temp
  uint32_t altD1;   // ADC value of the pressure conversion
  uint32_t altD2;   // ADC value of the temperature conversion
  // Temp difference, offset, and sensitivity
  int32_t  altdT;   // difference between actual and measured temperature
  int64_t  altOFF;  // offset at actual temperature
  int64_t  altSENS; // sensitivity at actual temperature
  // Temperature
  // int32_t  altT;    // compensated temperature value
  // float altTFloat;  // Decimal form of temperature value (altT/100)
  // Pressure
  int32_t  altP;    // compensated pressure value
  float altPFloat;  // Decimal form of pressure value (altP/100)

  float altAltitude; // Calculated altitude

  while (!altProm(altCalibration)) {}; // read coefficients
  altCRC = altCRC4(altCalibration); // calculate the CRC.

  if (altCRC == (altCalibration[7] & 0xFF00)) { // If prom CRC (Last byte of coefficient 7) and calculated CRC do not match, something went wrong!
    while (true) {
      beep(ALT_CRC_ERR);
      delay(250);
    }
  }

  //////////////////////////////////////////////////////////
  while(FreeSpaceAvailable) {
    gpsBufferIndex = 0; //start gpsBuffer[] at zero

    //
    // Get data from GPS
    //
    if (MAP_UARTCharsAvail(UART4_BASE)) { // Find out if GPS has data available
      gpsReceive(&gpsBuffer[0], &gpsBufferIndex);
    }

    //
    // Get data from accelerometer
    //
    MAP_ADCProcessorTrigger(ADC0_BASE, 3);
    while(!MAP_ADCIntStatus(ADC0_BASE, 3, false)) {} // wait for a2d conversion
        // if stuck in while() unplug lauchpad for 15s to reset
    MAP_ADCIntClear(ADC0_BASE, 3);
    MAP_ADCSequenceDataGet(ADC0_BASE, 3, &accelData);

    //
    // Get data from altimeter
    //

    // D1 and D2 Conversion
    if (altADCConversion(ALT_ADC_D1+ALT_ADC_256, &altD1) && altADCConversion(ALT_ADC_D2+ALT_ADC_256, &altD2)) {
      // Calculate 1st order pressure and temperature (MS5607 1st order algorithm)

      // Calculate temp difference, offset, and sensitivity
      altdT=altD2-altCalibration[5]*pow(2,8);
      altOFF=altCalibration[2]*pow(2,17)+altdT*altCalibration[4]/pow(2,6);
      altSENS=altCalibration[1]*pow(2,16)+altdT*altCalibration[3]/pow(2,7);

      // Calculate temperature
      // altT=2000+(altdT*altCalibration[6])/pow(2,23);
      // altTFloat = altT/100;

      // Calculate pressure
      altP=((altD1*altSENS)/pow(2,21)-altOFF)/pow(2,15);
      altPFloat = altP/100;

      altAltitude = (1-pow((altPFloat/1013.25),.190284))*145366.45;
    }

    //
    // Send data over UART for debugging
    //
    UARTprintf("accel: %d\n\rpressure: %d\n\ralt: %d\n\r\n\r", accelData, altP, (uint64_t) altAltitude);

    /*
    //
    // Write GPS Buffer to Flash
    // TODO: save alt data to flash
    // TODO: save accelData data to flash
    //
    if (gpsBufferIndex > 0) { // We have data to write
      FreeSpaceAvailable = flashstoreWriteRecord(&gpsBuffer[0], gpsBufferIndex);
    }
    */

    beep(GOOD); // Green LED to verify code is running

  } // main while end

  beep(OUT_OF_FLASH);

  // Out of flash memory. Output flash memory to console
  while (true)
  {
    flashPackedChar = flashstoreGetData(flashCurrAddr);
    if((flashPackedChar & 0xFFFFFF00) == flashHeader) {
      flashRecordSize = flashPackedChar & 0xFF;
      for (; flashCurrAddr < (flashCurrAddr + flashRecordSize); flashCurrAddr += 0x04) {
        flashPackedChar = flashstoreGetData(flashCurrAddr);
        UARTprintf("%c%c%c%c", unpack_c0(flashPackedChar), unpack_c1(flashPackedChar), unpack_c2(flashPackedChar), unpack_c3(flashPackedChar));
      }
      UARTprintf("\n\r");
    } else {
      flashCurrAddr += 0x04;
    }
    if (flashCurrAddr >= flashEndAddr) {
      flashCurrAddr = flashStartAddr;
      UARTprintf("*\n\r");
      delay(5000);
    }
  }
}

//*****************************************************************************
//
// Misc Functions
//
//*****************************************************************************

void
delay(uint32_t ms) {
  MAP_SysCtlDelay((MAP_SysCtlClockGet()/(3*1000))*ms);
}

void
beep(BeepCode code) {
  LEDOff(RED_LED | GREEN_LED | BLUE_LED);
  switch (code) {
    case GOOD: { // code is running
      LEDOn(GREEN_LED);
      break;
    }
    case DRL_ERR: { // driver library encountered an error
      LEDBlink(RED_LED, 250);
      delay(250);
      LEDBlink(BLUE_LED, 250);
      delay(250);
      LEDBlink(WHITE_LED, 250);
      delay(250);
      break;
    }
    case ALT_CRC_ERR: { // altimeter calibration error
      LEDBlink(RED_LED, 250);
      delay(250);
      LEDBlink(RED_LED, 250);
      delay(250);
      LEDBlink(RED_LED, 250);
      delay(250);
      break;
    }
    case OUT_OF_FLASH: { // out of flash memory
      LEDOn(BLUE_LED);
      break;
    }
    case ALT_ADC_CONV_ERR: { // ALT_ADC_CONV error
      LEDBlink(YELLOW_LED, 250);
      delay(250);
      LEDBlink(YELLOW_LED, 250);
      delay(250);
      LEDBlink(RED_LED, 250);
      delay(250);
      break;
    }
    case ALT_ADC_R_WRITE_ERR: { // ALT_ADC_READ write error
      LEDBlink(YELLOW_LED, 250);
      delay(250);
      LEDBlink(YELLOW_LED, 250);
      delay(250);
      LEDBlink(RED_LED, 750);
      delay(250);
      break;
    }
    case ALT_ADC_R_READ_ERR: { // ALT_ADC_READ read error
      LEDBlink(YELLOW_LED, 250);
      delay(250);
      LEDBlink(YELLOW_LED, 750);
      delay(250);
      LEDBlink(RED_LED, 250);
      delay(250);
      break;
    }
    case ALT_PROM_R_WRITE_ERR: { // ALT_PROM_READ write error
      LEDBlink(YELLOW_LED, 250);
      delay(250);
      LEDBlink(YELLOW_LED, 750);
      delay(250);
      LEDBlink(RED_LED, 750);
      delay(250);
      break;
    }
    case ALT_PROM_R_READ_ERR: { // ALT_PROM_READ read error
      LEDBlink(YELLOW_LED, 750);
      delay(250);
      LEDBlink(YELLOW_LED, 250);
      delay(250);
      LEDBlink(RED_LED, 250);
      delay(250);
      break;
    }
    case ALT_RESET_ERR: { // ALT_RESET error
      LEDBlink(YELLOW_LED, 750);
      delay(250);
      LEDBlink(YELLOW_LED, 250);
      delay(250);
      LEDBlink(RED_LED, 750);
      delay(250);
      break;
    }
  }
}

//*****************************************************************************
//
// LED functions
//
//*****************************************************************************
void
LEDOn(uint8_t color) {
  MAP_GPIOPinWrite(GPIO_PORTF_BASE, color, color);
}

void
LEDOff(uint8_t color) {
  MAP_GPIOPinWrite(GPIO_PORTF_BASE, color, 0);
}

void
LEDBlink(uint8_t color, uint32_t time) {
    LEDOn(color);
    delay(time);
    LEDOff(color);
}

//*****************************************************************************
//
// Initialize Peripherals
//
//*****************************************************************************
void
LEDInit(void) {
  //
  // Enable the peripherals used by the on-board LED.
  //
  MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);

  //
  // Enable the GPIO pins for the RGB LED.
  //
  MAP_GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, RED_LED); // Red
  MAP_GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GREEN_LED); // Blue
  MAP_GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, BLUE_LED); // Green
}

void
consoleInit(void) {
  //
  // Enable the peripherals used by the console.
  //
  MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
  MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);

  //
  // Set GPIO A1 as UART pin for console output
  //
  MAP_GPIOPinConfigure(GPIO_PA0_U0RX);
  MAP_GPIOPinConfigure(GPIO_PA1_U0TX);
  MAP_GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

  //
  // Configure UARTstdio Library
  //
  UARTStdioConfig(0, 115200, MAP_SysCtlClockGet());
}

void
gpsInit(void) {
  //
  // Enable the peripherals used by the GPS.
  //
  MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_UART4);
  MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);

  //
  // Set GPIO PC4 as U4RX for GPS int32_terface
  //
  MAP_GPIOPinConfigure(GPIO_PC4_U4RX);
  MAP_GPIOPinTypeUART(GPIO_PORTC_BASE, GPIO_PIN_4);

  //
  // Configure the UART for 9600, 8-N-1 operation.
  //
  MAP_UARTConfigSetExpClk(UART4_BASE, MAP_SysCtlClockGet(), 9600,
                          (UART_CONFIG_WLEN_8 | UART_CONFIG_PAR_NONE |
                          UART_CONFIG_STOP_ONE));
}

void
accelInit(void) {
  //
  // Enable the peripherals used by the accelerometer
  //
  MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);
  MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);

  //
  // Configure GPIO PE3 as ADC0 for accelerometer
  //
  MAP_GPIOPinTypeADC(GPIO_PORTE_BASE, GPIO_PIN_3);

  //
  // Configure ADC0 on sequence 3
  //
  MAP_ADCSequenceDisable(ADC0_BASE, 3);
  MAP_ADCSequenceConfigure(ADC0_BASE, 3, ADC_TRIGGER_PROCESSOR, 0);
  MAP_ADCSequenceStepConfigure(ADC0_BASE, 3, 0, ADC_CTL_CH0 | ADC_CTL_IE | ADC_CTL_END);
  MAP_ADCSequenceEnable(ADC0_BASE, 3);

  // Enable ADC interupts
  MAP_ADCIntEnable(ADC0_BASE, 3);
  MAP_ADCIntClear(ADC0_BASE, 3);
}

void
altInit(void) {
  //
  // Enable the peripherals used by the accelerometer
  //
  MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_I2C0);
  MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);

  //
  // Configure GPIO PB2 as I2C0 SCL
  //
  MAP_GPIOPinConfigure(GPIO_PB2_I2C0SCL);
  MAP_GPIOPinTypeI2CSCL(GPIO_PORTB_BASE, GPIO_PIN_2);

  //
  // Configure GPIO PB3 as I2C0 SDA
  //
  MAP_GPIOPinConfigure(GPIO_PB3_I2C0SDA);
  MAP_GPIOPinTypeI2C(GPIO_PORTB_BASE, GPIO_PIN_3);

  //
  // Configure I2C0.
  //
  MAP_I2CMasterInitExpClk(I2C0_BASE, MAP_SysCtlClockGet(), I2C_SPEED_400);

  //
  // Enable I2C0 Master Block
  //
  MAP_I2CMasterEnable(I2C0_BASE);

  // Reset altimeter
  while (!altReset()) {};
}

//*****************************************************************************
//
// Device Functions
//
//*****************************************************************************

void
gpsReceive(uint8_t* buffer, uint32_t* bIndex) {
  uint8_t command[5] = {'G','P','G','G','A'}, newChar, i;
  bool match, noStar; // booleans

  newChar = MAP_UARTCharGet(UART4_BASE);
  if ( newChar == '$') { // find start of a string of info

    buffer[*bIndex] = newChar; *bIndex++; // Add $ as delimiter

    match = true; // Assume match is true. Check in next loop.

    for (i = 0; i < 5; i++) {
      newChar = MAP_UARTCharGet(UART4_BASE); // collect the next five characters

      if (newChar == command[i]) { // validate init match assumption
        buffer[*bIndex] = newChar; *bIndex++;
      }
      else {
        match = false; // Assumption was wrong. Break and retry.
        break;
      }
    }

    //if the opening string matched "GPGGA", start processing the data feed
    if (match) {

      noStar = true; //while asterisk is not found

      while (noStar) {
      newChar = MAP_UARTCharGet(UART4_BASE); // collect the next five characters
        if (newChar == '*') {
          noStar = false; //if asterisk is found
        } else {
          buffer[*bIndex] = newChar; *bIndex++;
        }
      }

      // Add Callsign at end of GPS data log
      buffer[*bIndex] = 'K'; *bIndex++;
      buffer[*bIndex] = 'R'; *bIndex++;
      buffer[*bIndex] = '0'; *bIndex++;
      buffer[*bIndex] = 'C'; *bIndex++;
      buffer[*bIndex] = 'K'; *bIndex++;
      buffer[*bIndex] = 'T'; *bIndex++;
    }
  } //If char == $
}

bool
altADCConversion(uint8_t alt_cmd, uint32_t* adc_conv) {
  uint32_t ret[3];

  if (!I2CWrite(ALT_ADC_CONV & alt_cmd)) {
    beep(ALT_ADC_CONV_ERR);
    UARTprintf("ALT_ADC_CONV write error on line %d in file %s\n\r", __LINE__, __FILE__);
  }
  switch(alt_cmd & 0x0F) {
    case ALT_ADC_256:  delay(ALT_256_DELAY);  break;
    case ALT_ADC_512:  delay(ALT_512_DELAY);  break;
    case ALT_ADC_1024: delay(ALT_1024_DELAY); break;
    case ALT_ADC_2048: delay(ALT_2048_DELAY); break;
    case ALT_ADC_4096: delay(ALT_4096_DELAY); break;
  }

  //
  // Tell altimeter to send data to launchpad
  //
  if (!I2CWrite(ALT_ADC_READ)) {
    beep(ALT_ADC_R_WRITE_ERR);
    UARTprintf("ALT_ADC_READ write error on line %d in file %s\n\r", __LINE__, __FILE__);
    return 0;
  }

  //
  // Start receiving data from altimeter
  //
  if (!I2CBurstRead(ret, 3)) {
    beep(ALT_ADC_R_READ_ERR);
    UARTprintf("ALT_ADC_READ read error on line %d in file %s\n\r", __LINE__, __FILE__);
    return 0;
  }

  *adc_conv = (65536*ret[0]) + (256 * ret[1]) + ret[2];
  return true;
}

uint8_t
altCRC4(uint16_t n_prom[]) {
  int32_t cnt; // simple counter
  uint16_t n_rem = 0x00; // crc reminder
  uint16_t crc_read = n_prom[7]; // original value of the crc
  uint8_t n_bit;
  n_prom[7]=(0xFF00 & (n_prom[7])); //CRC byte is replaced by 0
  for (cnt = 0; cnt < 16; cnt++) // operation is performed on bytes
  { // choose LSB or MSB
    if (cnt%2==1) {
      n_rem ^= (n_prom[cnt>>1]) & 0x00FF;
    }
    else {
      n_rem ^= n_prom[cnt>>1]>>8;
    }
    for (n_bit = 8; n_bit > 0; n_bit--)
    {
      if (n_rem & (0x8000))
      {
        n_rem = (n_rem << 1) ^ 0x3000;
      }
      else
      {
        n_rem = (n_rem << 1);
      }
    }
  }
  n_rem= (0x000F & (n_rem >> 12)); // final 4-bit reminder is CRC code
  n_prom[7]=crc_read; // restore the crc_read to its original place
  return (n_rem ^ 0x0);
}

bool
altProm(uint16_t C[8]) {
  uint32_t i;
  uint32_t ret[2];
  for (i = 0; i < 8; i++) {
    if (!I2CWrite(ALT_PROM_READ+(i*2))) {
      beep(ALT_PROM_R_WRITE_ERR);
      UARTprintf("ALT_PROM_READ write error on line %d in file %s\n\r", __LINE__, __FILE__);
      return 0;
    }
    if (!I2CBurstRead(ret, 2)) {
      beep(ALT_PROM_R_READ_ERR);
      UARTprintf("ALT_PROM_READ read error on line %d in file %s\n\r", __LINE__, __FILE__);
      return 0;
    }
    C[i] = ((256 * ret[0]) + ret[1]);
  }
  return 1;
}

bool
altReset(void) {
  if (!I2CWrite(ALT_RESET)) {
    beep(ALT_RESET_ERR);
    UARTprintf("ALT_RESET write error on line %d in file %s\n\r", __LINE__, __FILE__);
    return 0;
  }
  delay(ALT_RESET_DELAY);
  return 1;
}

uint32_t
I2CRead() {
  //
  // Tell the master module what address it will place on the bus when
  // reading from the slave.
  //
  MAP_I2CMasterSlaveAddrSet(I2C0_BASE, ALT_ADDRESS, I2C_MODE_READ);

  //
  // Tell the master to read data.
  //
  MAP_I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_SINGLE_RECEIVE);

  //
  // Wait until master module is done receiving.
  //
  while(MAP_I2CMasterBusy(I2C0_BASE)) {};

  //
  // Check for errors.
  //
  if(MAP_I2CMasterErr(I2C0_BASE) != I2C_MASTER_ERR_NONE) return 0;

  //
  // return the data from the master.
  //
  return MAP_I2CMasterDataGet(I2C0_BASE);
}

uint32_t
I2CWrite(uint8_t ucValue) {
  //
  // Tell the master module what address it will place on the bus when
  // writing to the slave.
  //
  MAP_I2CMasterSlaveAddrSet(I2C0_BASE, ALT_ADDRESS, I2C_MODE_WRITE);

  //
  // Place the command to be sent in the data register.
  //
  MAP_I2CMasterDataPut(I2C0_BASE, ucValue);

  //
  // Initiate send of data from the master.
  //
  MAP_I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_SINGLE_SEND);

  //
  // Wait until master module is done transferring.
  //
  while(MAP_I2CMasterBusy(I2C0_BASE)) {};

  //
  // Check for errors.
  //
  if(MAP_I2CMasterErr(I2C0_BASE) != I2C_MASTER_ERR_NONE) return 0;

  //
  // Return 1 if there is no error.
  //
  return 1;
}

uint32_t
I2CBurstRead(uint32_t* cReadData, uint32_t uiSize) {
  //
  // Use I2C single read if theres only 1 item to receive
  //
  if (uiSize == 1) {
    cReadData[0] = I2CRead();
    return cReadData[0];
  }

  uint32_t uiByteCount;        // local variable used for byte counting/state determination
  uint32_t MasterOptionCommand; // used to assign the control commands

  //
  // Tell the master module what address it will place on the bus when
  // reading from the slave.
  //
  MAP_I2CMasterSlaveAddrSet(I2C0_BASE, ALT_ADDRESS, I2C_MODE_READ);

  //
  // Start with BURST with more than one byte to read
  //
  MasterOptionCommand = I2C_MASTER_CMD_BURST_RECEIVE_START;

  for(uiByteCount = 0; uiByteCount < uiSize; uiByteCount++)
  {
    //
    // The second and intermittent byte has to be read with CONTINUE control word
    //
    if(uiByteCount == 1)
      MasterOptionCommand = I2C_MASTER_CMD_BURST_RECEIVE_CONT;

    //
    // The last byte has to be send with FINISH control word
    //
    if(uiByteCount == uiSize - 1)
      MasterOptionCommand = I2C_MASTER_CMD_BURST_RECEIVE_FINISH;

    //
    // Initiate read of data from the slave.
    //
    MAP_I2CMasterControl(I2C0_BASE, MasterOptionCommand);

    //
    // Wait until master module is done reading.
    //
    while(MAP_I2CMasterBusy(I2C0_BASE)) {};

    //
    // Check for errors.
    //
    if (MAP_I2CMasterErr(I2C0_BASE) != I2C_MASTER_ERR_NONE) return 0;

    //
    // Move byte from register
    //
    cReadData[uiByteCount] = MAP_I2CMasterDataGet(I2C0_BASE);
  }

  //
  // Return 1 if there is no error.
  //
  return uiByteCount;
}

uint32_t
I2CBurstWrite(uint8_t* cSendData, uint32_t uiSize) {
  //
  // Use I2C single write if theres only 1 item to send
  //
  if (uiSize == 1) {
    return I2CWrite(cSendData[0]);
  }

  uint32_t uiByteCount;         // local variable used for byte counting/state determination
  uint32_t MasterOptionCommand; // used to assign the control commands

  //
  // Tell the master module what address it will place on the bus when
  // writing to the slave.
  //
  MAP_I2CMasterSlaveAddrSet(I2C0_BASE, ALT_ADDRESS, I2C_MODE_WRITE);

  //
  // Wait until master module is done transferring.
  //
  while(MAP_I2CMasterBusy(I2C0_BASE)) {};

  //
  // The first byte has to be sent with the START control word
  //
  MasterOptionCommand = I2C_MASTER_CMD_BURST_SEND_START;

  for(uiByteCount = 0; uiByteCount < uiSize; uiByteCount++)
  {
    //
    // The second and intermittent byte has to be send with CONTINUE control word
    //
    if(uiByteCount == 1)
      MasterOptionCommand = I2C_MASTER_CMD_BURST_SEND_CONT;

    //
    // The last byte has to be send with FINISH control word
    //
    if(uiByteCount == uiSize - 1)
      MasterOptionCommand = I2C_MASTER_CMD_BURST_SEND_FINISH;

    //
    // Send data byte
    //
    MAP_I2CMasterDataPut(I2C0_BASE, cSendData[uiByteCount]);

    //
    // Initiate send of data from the master.
    //
    MAP_I2CMasterControl(I2C0_BASE, MasterOptionCommand);

    //
    // Wait until master module is done transferring.
    //
    while(MAP_I2CMasterBusy(I2C0_BASE)) {};

    //
    // Check for errors.
    //
    if(MAP_I2CMasterErr(I2C0_BASE) != I2C_MASTER_ERR_NONE) return 0;
  }

  //
  // Return 1 if there is no error.
  //
  return 1;
}
