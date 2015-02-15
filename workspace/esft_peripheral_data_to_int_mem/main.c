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

// Determines if beep codes are enabled. Comment out to disable functionality
#define BEEP_CODES_ENABLED

typedef enum BEEPCODE {
  INITIALIZING, RUNNING, DRL_ERR, ALT_CRC_ERR, OUT_OF_FLASH,
  ALT_RESET_ERR, ALT_PROM_R_WRITE_ERR, ALT_PROM_R_READ_ERR,
  ALT_ADC_CONV_ERR, ALT_ADC_R_WRITE_ERR, ALT_ADC_R_READ_ERR
  } BeepCode;

//*****************************************************************************
//
// Prototypes
//
//*****************************************************************************
void delay(uint32_t ui32ms);
void beep(BeepCode code);
void LEDOn(uint8_t ui8Color);
void LEDOff(uint8_t ui8Color);
void LEDBlink(uint8_t ui8Color, uint32_t ui32Time);
void FPUInit(void);
void LEDInit(void);
void consoleInit(void);
void gpsInit(void);
void accelInit(void);
void altInit(void);
bool gpsReceive(uint8_t* ui8Buffer);
void accelReceive(uint32_t* ui32ptrData);
bool altReceive(uint8_t ui8OSR, uint16_t ui16Calibration[8], float* fTemp, float* fPressure, float* fAltitude);
bool altADCConversion(uint8_t ui8Cmd, uint32_t* ui32ptrData);
uint8_t altCRC4(uint16_t ui16nProm[8]);
bool altProm(uint16_t ui16nProm[8]);
bool altReset(void);
bool I2CRead(uint32_t* ui32ptr32Data);
bool I2CWrite(uint8_t ui8Data);
bool I2CBurstRead(uint32_t* ui32ptrReadData, uint32_t ui32Size);
bool I2CBurstWrite(uint8_t ui8SendData[], uint32_t ui32Size);

#ifdef DEBUG
//*****************************************************************************
//
// The error routine that is called if the driver library encounters an error.
//
//*****************************************************************************
void
__error__(char *pcFilename, uint32_t ui32Line) {

  while(true) {
    UARTprintf("Error at line %d of %s\n\r", ui32Line, pcFilename);
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
  // Flash storage variables
  //
  bool     FreeSpaceAvailable;
  uint32_t index;
  uint32_t flashCurrAddr = FLASH_STORE_START_ADDR; // Current address in flash storage for read. Initialize to starting address
  uint32_t flashHeader = FLASH_STORE_RECORD_HEADER; // Magic Header Byte to find record beginning
  uint32_t flashPackedChar; // 4 Byte return from storage. Theoretically holds 4 packed chars
  uint32_t flashRecordSize; // Size of the current flash record
  uint8_t  flashWriteBuffer[512]; // Buffer of the data to write
  int32_t  flashWriteBufferSize;  // Length of the record to write

  //
  // GPS Variables
  //
  bool     gpsDataReceived;
  uint8_t  gpsSentence[128];

  //
  // Accelerometer variables
  //
  uint32_t accelData;

  //
  // Altimeter variables
  //
  bool     altDataReceived;
  uint16_t altCalibration[8]; // calibration coefficients
  uint8_t  altCRC;            // calculated CRC
  float    altTemperature;    // compensated temperature value
  float    altPressure;       // compensated pressure value
  float    altAltitude;       // Calculated altitude

  //
  ///////////////////Initialize//////////////////////////////
  //

  //
  // Set the clocking to run directly from the crystal.
  //
  MAP_SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN |
                     SYSCTL_XTAL_16MHZ);

  //
  // Enable the LED
  //
  LEDInit();

  beep(INITIALIZING);

  //
  // Enable the FPU
  //
  FPUInit();

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
  //
  altInit();

  //
  // Enable flash storage
  //
  FreeSpaceAvailable = flashstoreInit(false);

  //
  ///////////////////Gather Data/////////////////////////////
  //

  while (!altProm(altCalibration)) {}; // read coefficients
  altCRC = altCRC4(altCalibration); // calculate the CRC.

  if (altCRC == (altCalibration[7] & 0xFF00)) { // If prom CRC (Last byte of coefficient 7) and calculated CRC do not match, something went wrong!
    while (true) {
      beep(ALT_CRC_ERR);
      delay(250);
    }
  }


  uint32_t test = 1;
  while(FreeSpaceAvailable) {
    beep(RUNNING);

    //
    // Get data from accelerometer
    //
    accelReceive(&accelData);

    //
    // Get data from altimeter
    //
    altDataReceived = altReceive(ALT_ADC_4096, altCalibration, &altTemperature, &altPressure, &altAltitude);

    //
    // Get data from GPS
    //
    gpsDataReceived = gpsReceive(&gpsSentence[0]);

    flashWriteBufferSize = sprintf((char*) &flashWriteBuffer[0], "Accelerometer: %d\n\r", accelData);
    if (altDataReceived) { // altimeter data was received
      flashWriteBufferSize = sprintf((char*) &flashWriteBuffer[0], "%sTemperature: %f C\n\rPressure: %f mbar\n\rAltitude: %f ft\n\r", flashWriteBuffer, altTemperature, altPressure, altAltitude);
    }
    if (gpsDataReceived) { // gps data was received
      flashWriteBufferSize = sprintf((char*) &flashWriteBuffer[0], "%sGPS: %s\n\r", flashWriteBuffer, gpsSentence);
    }

    //
    // Send write buffer over UART for debugging
    //
    // UARTprintf("%s", flashWriteBuffer);

    //
    // Write data to flash
    //
    FreeSpaceAvailable = flashstoreWriteRecord(&flashWriteBuffer[0], flashWriteBufferSize);

    if (test > 5) break;
  } // main while end

  beep(OUT_OF_FLASH);

  // Out of flash memory. Output flash memory to console
  while (true) {
    flashPackedChar = flashstoreGetData(flashCurrAddr);
    if((flashPackedChar & 0xFFFFFF00) == flashHeader) {
      flashRecordSize = flashPackedChar & 0xFF;
      for (index = 0; index < flashRecordSize; index++) {
        flashCurrAddr += FLASH_STORE_BLOCK_WRITE_SIZE;
        flashPackedChar = flashstoreGetData(flashCurrAddr);
        unpack(flashPackedChar, &flashWriteBuffer[0]);
        UARTprintf("%s", flashWriteBuffer);
      }
    } else if (flashCurrAddr >= FLASH_STORE_END_ADDR) {
      flashCurrAddr = FLASH_STORE_START_ADDR;
      UARTprintf("*\n\r");
      delay(5000);
    } else {
      flashCurrAddr += FLASH_STORE_BLOCK_WRITE_SIZE;
    }
  }
}

//*****************************************************************************
//
// Misc Functions
//
//*****************************************************************************
void
delay(uint32_t ui32ms) {
  MAP_SysCtlDelay((MAP_SysCtlClockGet()/(3*1000))*ui32ms);
}
void
beep(BeepCode code) {
#ifdef BEEP_CODES_ENABLED
  LEDOff(RED_LED | GREEN_LED | BLUE_LED); // Reset LED to off state
  switch (code) {
    case INITIALIZING: { // device is initializing
      LEDOn(RED_LED | GREEN_LED | BLUE_LED);
      break;
    }
    case RUNNING: { // code is running
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
#endif
}

//*****************************************************************************
//
// Initialize Peripherals
//
//*****************************************************************************
void
FPUInit(void) {
  //
  // Enable the floating-point unit
  //
  MAP_FPUEnable();

  //
  // Configure the floating-point unit to perform lazy stacking of the floating-point state.
  //
  MAP_FPULazyStackingEnable();
}
void
LEDInit(void) {
  //
  // Enable the peripherals used by the on-board LED.
  //
  MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);

  //
  // Enable the GPIO pins for the RGB LED.
  //
  MAP_GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, RED_LED);
  MAP_GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GREEN_LED);
  MAP_GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, BLUE_LED);
}
void
consoleInit(void) {
  //
  // Enable the peripherals used by the console.
  //
  MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
  MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);

  //
  // Set GPIO A0 and A1 as UART pins for console output
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
  MAP_I2CMasterInitExpClk(I2C0_BASE, MAP_SysCtlClockGet(), I2C_SPEED_100);

  //
  // Enable I2C0 Master Block
  //
  MAP_I2CMasterEnable(I2C0_BASE);

  // Reset altimeter
  while (!altReset())
    beep(ALT_RESET_ERR);
}

//*****************************************************************************
//
// Device Functions
//
//*****************************************************************************
void
LEDOn(uint8_t ui8Color) {
  MAP_GPIOPinWrite(GPIO_PORTF_BASE, ui8Color, ui8Color);
}
void
LEDOff(uint8_t ui8Color) {
  MAP_GPIOPinWrite(GPIO_PORTF_BASE, ui8Color, 0);
}
void
LEDBlink(uint8_t ui8Color, uint32_t ui32Time) {
    LEDOn(ui8Color);
    delay(ui32Time);
    LEDOff(ui8Color);
}
bool
gpsReceive(uint8_t* ui8Buffer) {
  uint32_t ui32bIndex = 0;
  uint8_t command[5] = {'G','P','G','G','A'}, newChar, i;
  bool match = true; // If a match was found. Assume match is true until proven otherwise

  if (MAP_UARTCharsAvail(UART4_BASE)) { // Find out if GPS has data available

    newChar = MAP_UARTCharGet(UART4_BASE);

    if ( newChar == '$') { // find start of a string of info
      ui8Buffer[ui32bIndex] = newChar; ui32bIndex++; // Add $ as delimiter
      for (i = 0; i < 5; i++) {
        newChar = MAP_UARTCharGet(UART4_BASE); // collect the next five characters
        if (newChar == command[i]) { // validate match assumption
          ui8Buffer[ui32bIndex] = newChar; ui32bIndex++;
        }
        else {
          match = false; // Assumption was wrong. Break and retry.
          break;
        }
      }

      //if the opening string matched "GPGGA", start processing the data feed
      if (match) {
        while (true) {
          newChar = MAP_UARTCharGet(UART4_BASE); // collect the rest of the GPS log
          if (newChar == '*')  break; // If the character is a star, break the loop
          ui8Buffer[ui32bIndex] = newChar; ui32bIndex++;
        }

        // Collect the checksum data
        ui8Buffer[ui32bIndex] = MAP_UARTCharGet(UART4_BASE); ui32bIndex++;
        ui8Buffer[ui32bIndex] = MAP_UARTCharGet(UART4_BASE); ui32bIndex++;

        // Add null terminator to end of GPS data
        ui8Buffer[ui32bIndex] = '\0'; ui32bIndex++;
        return true;
      }
    } //If char == $
  }
  return false;
}
void
accelReceive(uint32_t* ui32ptrData) {
  MAP_ADCProcessorTrigger(ADC0_BASE, 3);
  while(!MAP_ADCIntStatus(ADC0_BASE, 3, false)) {} // wait for a2d conversion
      // if stuck in while() unplug lauchpad for 15s to reset
  MAP_ADCIntClear(ADC0_BASE, 3);
  MAP_ADCSequenceDataGet(ADC0_BASE, 3, ui32ptrData);
}
bool
altADCConversion(uint8_t ui8Cmd, uint32_t* ui32ptrData) {
  uint32_t ret[3];

  if (!I2CWrite(ALT_ADC_CONV+ui8Cmd)) {
    beep(ALT_ADC_CONV_ERR);
    UARTprintf("ALT_ADC_CONV write error\n\r");
    return false;
  }
  switch(ui8Cmd & 0x0F) {
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
    UARTprintf("ALT_ADC_READ write error\n\r");
    return false;
  }

  //
  // Start receiving data from altimeter
  //
  if (!I2CBurstRead(&ret[0], 3)) {
    beep(ALT_ADC_R_READ_ERR);
    UARTprintf("ALT_ADC_READ read error\n\r");
    return false;
  }

  *ui32ptrData = (65536*ret[0]) + (256 * ret[1]) + ret[2];
  return true;
}
uint8_t
altCRC4(uint16_t ui16nProm[8]) {
  int32_t cnt; // simple counter
  uint16_t n_rem = 0x00; // crc reminder
  uint16_t crc_read = ui16nProm[7]; // original value of the crc
  uint8_t n_bit;
  ui16nProm[7]=(0xFF00 & (ui16nProm[7])); //CRC byte is replaced by 0
  for (cnt = 0; cnt < 16; cnt++) // operation is performed on bytes
  { // choose LSB or MSB
    if (cnt%2==1) {
      n_rem ^= (ui16nProm[cnt>>1]) & 0x00FF;
    }
    else {
      n_rem ^= ui16nProm[cnt>>1]>>8;
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
  ui16nProm[7]=crc_read; // restore the crc_read to its original place
  return (n_rem ^ 0x0);
}
bool
altProm(uint16_t ui16nProm[8]) {
  uint32_t i;
  uint32_t ret[2];
  for (i = 0; i < 8; i++) {
    if (!I2CWrite(ALT_PROM_READ+(i*2))) {
      beep(ALT_PROM_R_WRITE_ERR);
      UARTprintf("ALT_PROM_READ write error\n\r");
      return false;
    }
    if (!I2CBurstRead(&ret[0], 2)) {
      beep(ALT_PROM_R_READ_ERR);
      UARTprintf("ALT_PROM_READ read error\n\r");
      return false;
    }
    ui16nProm[i] = ((256 * ret[0]) + ret[1]);
  }
  return true;
}
bool
altReset(void) {
  if (!I2CWrite(ALT_RESET)) {
    beep(ALT_RESET_ERR);
    UARTprintf("ALT_RESET write error\n\r");
    return false;
  }
  delay(ALT_RESET_DELAY);
  return true;
}
bool
altReceive(uint8_t ui8OSR, uint16_t ui16Calibration[8], float* fTemp, float* fPressure, float* fAltitude)
{
  // Digital pressure and temp
  uint32_t D1 = 0;   // ADC value of the pressure conversion
  uint32_t D2 = 0;   // ADC value of the temperature conversion

  // Temp difference, offsets, and sensitivities
  int32_t  dT = 0;   // difference between actual and measured temperature
  int32_t  T2 = 0;   // second order temperature offset
  int64_t  OFF = 0;  // offset at actual temperature
  int64_t  OFF2 = 0;  // second order offset at actual temperature
  int64_t  SENS = 0; // sensitivity at actual temperature
  int64_t  SENS2 = 0; // second order sensitivity at actual temperature

  if (altADCConversion(ALT_ADC_D1+ui8OSR, &D1) && altADCConversion(ALT_ADC_D2+ui8OSR, &D2)) {
    // Calculate 1st order pressure and temperature

    // Calculate 1st order temp difference, offset, and sensitivity (MS5607 1st order algorithm)
    dT=D2-ui16Calibration[5]*pow(2,8);
    OFF=ui16Calibration[2]*pow(2,17)+dT*ui16Calibration[4]/pow(2,6);
    SENS=ui16Calibration[1]*pow(2,16)+dT*ui16Calibration[3]/pow(2,7);

    // Calculate temperature
    *fTemp=2000+(dT*ui16Calibration[6])/pow(2,23)/100;

    // Calculate 2nd order temp difference, offset, and sensitivity (MS5607 2nd order algorithm)
    if (*fTemp < 20) {
      T2 = pow(dT, 2)/pow(2,31);
      OFF2 = 61*pow((*fTemp-2000),2)/pow(2,4);
      SENS2 = 2*pow((*fTemp-2000),2);
      if (*fTemp < -15)
      {
        OFF2 = OFF2+15*pow((*fTemp+1500),2);
        SENS2 = SENS2+8*pow((*fTemp+1500),2);
      }
    }
    *fTemp = *fTemp - T2;
    OFF = OFF - OFF2;
    SENS = SENS - SENS2;

    // Calculate pressure
    *fPressure=((D1*SENS)/pow(2,21)-OFF)/pow(2,15)/100;

    // Calculate altitude (in feet)
    *fAltitude = (1-pow((*fPressure/1013.25),.190284))*145366.45;

    return true;
  }
  return false;
}
bool
I2CRead(uint32_t* ui32ptrData) {
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
  if(MAP_I2CMasterErr(I2C0_BASE) != I2C_MASTER_ERR_NONE) return false;

  //
  // Get data
  //
  *ui32ptrData = MAP_I2CMasterDataGet(I2C0_BASE);

  //
  // return the data from the master.
  //
  return true;
}
bool
I2CWrite(uint8_t ui8SendData) {
  //
  // Tell the master module what address it will place on the bus when
  // writing to the slave.
  //
  MAP_I2CMasterSlaveAddrSet(I2C0_BASE, ALT_ADDRESS, I2C_MODE_WRITE);

  //
  // Place the command to be sent in the data register.
  //
  MAP_I2CMasterDataPut(I2C0_BASE, ui8SendData);

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
  if(MAP_I2CMasterErr(I2C0_BASE) != I2C_MASTER_ERR_NONE) return false;

  //
  // Return 1 if there is no error.
  //
  return true;
}
bool
I2CBurstRead(uint32_t* ui32ptrReadData, uint32_t ui32Size) {
  //
  // Use I2C single read if theres only 1 item to receive
  //
  if (ui32Size == 1)
    return I2CRead(&ui32ptrReadData[0]);

  uint32_t ui32ByteCount;        // local variable used for byte counting/state determination
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

  for(ui32ByteCount = 0; ui32ByteCount < ui32Size; ui32ByteCount++)
  {
    //
    // The second and intermittent byte has to be read with CONTINUE control word
    //
    if(ui32ByteCount == 1)
      MasterOptionCommand = I2C_MASTER_CMD_BURST_RECEIVE_CONT;

    //
    // The last byte has to be send with FINISH control word
    //
    if(ui32ByteCount == ui32Size - 1)
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
    if (MAP_I2CMasterErr(I2C0_BASE) != I2C_MASTER_ERR_NONE) return false;

    //
    // Move byte from register
    //
    ui32ptrReadData[ui32ByteCount] = MAP_I2CMasterDataGet(I2C0_BASE);
  }


  //
  // Return 1 if there is no error.
  //
  return true;
}
bool
I2CBurstWrite(uint8_t ui8SendData[], uint32_t ui32Size) {
  //
  // Use I2C single write if theres only 1 item to send
  //
  if (ui32Size == 1)
    return I2CWrite(ui8SendData[0]);

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

  for(uiByteCount = 0; uiByteCount < ui32Size; uiByteCount++)
  {
    //
    // The second and intermittent byte has to be send with CONTINUE control word
    //
    if(uiByteCount == 1)
      MasterOptionCommand = I2C_MASTER_CMD_BURST_SEND_CONT;

    //
    // The last byte has to be send with FINISH control word
    //
    if(uiByteCount == ui32Size - 1)
      MasterOptionCommand = I2C_MASTER_CMD_BURST_SEND_FINISH;

    //
    // Send data byte
    //
    MAP_I2CMasterDataPut(I2C0_BASE, ui8SendData[uiByteCount]);

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
    if(MAP_I2CMasterErr(I2C0_BASE) != I2C_MASTER_ERR_NONE) return false;
  }

  //
  // Return 1 if there is no error.
  //
  return true;
}
