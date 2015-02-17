//*****************************************************************************
//
// Includes
//
//*****************************************************************************

#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <math.h>

#include "inc/hw_gpio.h"
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
#include "driverlib/timer.h"
#include "driverlib/uart.h"

#include "altimeter.h"
#include "flashstore.h"
#include "gyro.h"
#include "uartstdio.h"

//*****************************************************************************
//
// Peripheral defines
//
//*****************************************************************************

// Accelerometer
#define ACCEL_ENABLED

// Altimeter
#define ALT_ENABLED
#define ALT_BASE    I2C0_BASE
#define ALT_ADDRESS ALT_ADDRESS_CSB_LO
#define ALT_SPEED   I2C_SPEED_400

// Flash
// Determines if flash debug output is enabled. Comment out to disable functionality
#define DEBUG_FLASH
#define DEBUG_FLASH_DELAY 500

// GPS
// #define GPS_ENABLED

// Gyro
// #define GYRO_ENABLED
#define GYRO_BASE    I2C1_BASE
#define GYRO_ADDRESS GYRO_ADDRESS_SDO_LO
#define GYRO_SPEED   I2C_SPEED_400

// I2C
#define I2C_MODE_WRITE false
#define I2C_MODE_READ  true
#define I2C_SPEED_100  false
#define I2C_SPEED_400  true

// LED Colors
#define RED_LED     GPIO_PIN_1
#define BLUE_LED    GPIO_PIN_2
#define GREEN_LED   GPIO_PIN_3
#define YELLOW_LED  RED_LED   | GREEN_LED
#define MAGENTA_LED RED_LED   | BLUE_LED
#define CYAN_LED    GREEN_LED | BLUE_LED
#define WHITE_LED   RED_LED   | GREEN_LED | BLUE_LED

//*****************************************************************************
//
// Status Codes (Uses LED)
// See ESFT Status Codes Spreadsheet for more information
//
//*****************************************************************************

// Determines if status codes are enabled. Comment out to disable functionality
#define STATUS_CODES_ENABLED

// Defines the length of time between interrupts in milliseconds
#define STATUS_CODE_LENGTH 250

// length of "dot" and "dash" blinks as well as the delimiter length in terms of interrupt count
#define STATUS_DOT  1
#define STATUS_DASH 3
#define STATUS_DELIMTER 3

// Status Codes
typedef enum STATUSCODE {
    INITIALIZING, RUNNING, DRL_ERR, ALT_CRC_ERR, OUT_OF_FLASH,
    ALT_RESET_ERR, ALT_PROM_R_WRITE_ERR, ALT_PROM_R_READ_ERR,
    ALT_ADC_CONV_ERR, ALT_ADC_R_WRITE_ERR, ALT_ADC_R_READ_ERR
  } StatusCode_t;

static StatusCode_t statusCode = INITIALIZING; // Current Status Code
static uint8_t      statusColor;               // Status Code Color
static uint32_t     statusBlinkDelay[3];       // Status Code Delays
static uint8_t      statusBeepIndex = 0;       // Index of beep LED
static uint32_t     statusDelayIndex = 0;      // Index of beep length
static bool         statusBusy = false;        // Status code system is busy
static bool         statusLEDOn = false;       // Status of the LED

//*****************************************************************************
//
// Prototypes
//
//*****************************************************************************
  // Misc
void delay(uint32_t ui32ms);
bool setStatus(StatusCode_t scStatus);
  // Initiations
void accelInit(void);
void altInit(void);
void consoleInit(void);
void FPUInit(void);
void gpsInit(void);
void gyroInit(void);
void I2CInit(uint32_t ui32Base, bool bSpeed);
void LEDInit(void);
void SSIInit(uint32_t ui32Base, uint32_t ui32Protocol, uint32_t ui32Mode, uint32_t ui32BitRate, uint32_t ui32DataWidth);
void statusCodeInterruptInit(void);
  // Peripherals
void LEDOn(uint8_t ui8Color);
void LEDOff(uint8_t ui8Color);
    // Receive Functions
void accelReceive(uint32_t* ui32ptrData);
bool altReceive(uint32_t ui32Base, uint8_t ui8AltAddr, uint8_t ui8OSR, uint16_t ui16Calibration[8], float* fTemp, float* fPressure, float* fAltitude);
bool gpsReceive(uint8_t* ui8Buffer);
bool gyroReceive(uint32_t ui32Base, uint8_t ui8GyroAddr);
    // Altimeter Functions
bool altADCConversion(uint32_t ui32Base, uint8_t ui8AltAddr, uint8_t ui8Cmd, uint32_t* ui32ptrData);
uint8_t altCRC4(uint16_t ui16nProm[8]);
bool altProm(uint32_t ui32Base, uint8_t ui8AltAddr, uint16_t ui16nProm[8]);
bool altReset(uint32_t ui32Base, uint8_t ui8AltAddr);
    // Gyro Functions
void gyroStartup(uint32_t ui32Base, uint8_t ui8GyroAddr);
    // I2C Functions
bool I2CRead(uint32_t ui32Base, uint8_t ui8SlaveAddr, uint32_t* ui32ptr32Data);
bool I2CWrite(uint32_t ui32Base, uint8_t ui8SlaveAddr, uint8_t ui8Data);
bool I2CBurstRead(uint32_t ui32Base, uint8_t ui8SlaveAddr, uint32_t* ui32ptrReadData, uint32_t ui32Size);
bool I2CBurstWrite(uint32_t ui32Base, uint8_t ui8SlaveAddr, uint8_t ui8SendData[], uint32_t ui32Size);

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
    setStatus(DRL_ERR);
    delay(1000);
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
  uint8_t  flashWriteBuffer[256]; // Buffer of the data to write
  int32_t  flashWriteBufferSize = 0;  // Length of the record to write

#ifdef GPS_ENABLED
  //
  // GPS Variables
  //
  bool     gpsDataReceived = false;  // GPS data has been received
  uint8_t  gpsSentence[128]; // GPS NMEA sentence
#endif

#ifdef ACCEL_ENABLED
  //
  // Accelerometer variables
  //
  uint32_t accelData = 0; // Accelerometer data
#endif

#ifdef ALT_ENABLED
  //
  // Altimeter variables
  //
  bool     altDataReceived = false;   // Altimeter data has been received
  uint16_t altCalibration[8]; // calibration coefficients
  uint8_t  altCRC;            // calculated CRC
  float    altTemperature;    // compensated temperature value
  float    altPressure;       // compensated pressure value
  float    altAltitude;       // Calculated altitude
#endif

  //
  ///////////////////Initialize//////////////////////////////
  //

  //
  // Set the clocking to run at 50MHz.
  //

  MAP_SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);

  //
  // Enable the LEDs
  //
  LEDInit();

#ifdef STATUS_CODES_ENABLED
  //
  // Enable timer interrupts
  //
  statusCodeInterruptInit();
#endif

  //
  // Enable the FPU
  //
  FPUInit();

  //
  // Enable the Console IO
  //
  consoleInit();

#ifdef GPS_ENABLED
  //
  // Enable the GPS
  //
  gpsInit();
#endif

#ifdef ACCEL_ENABLED
  //
  // Enable the accelerometer
  //
  accelInit();
#endif

#ifdef ALT_ENABLED
  //
  // Enable the altimeter
  //
  altInit();
#endif

#ifdef GYRO_ENABLED
  //
  // Enable the gyro
  //
  gyroInit();
#endif

  //
  // Enable flash storage
  //
  FreeSpaceAvailable = flashstoreInit(false);

  //
  ///////////////////Gather Data/////////////////////////////
  //

#ifdef ALT_ENABLED
  while (!altProm(ALT_BASE, ALT_ADDRESS, altCalibration)) {} // read coefficients
  altCRC = altCRC4(altCalibration); // calculate the CRC.
  while (altCRC != (altCalibration[7] & 0x000F)) { // If prom CRC (Last byte of coefficient 7) and calculated CRC do not match, something went wrong!
    setStatus(ALT_CRC_ERR);
  }
#endif

  while(FreeSpaceAvailable) {
#ifdef ACCEL_ENABLED
    //
    // Get data from accelerometer
    //
    accelReceive(&accelData);
    flashWriteBufferSize = sprintf((char*) &flashWriteBuffer[0], "%d,", accelData);
#endif

#ifdef ALT_ENABLED
    //
    // Get data from altimeter
    //
    altDataReceived = altReceive(ALT_BASE, ALT_ADDRESS, ALT_ADC_4096, altCalibration, &altTemperature, &altPressure, &altAltitude);
    if (altDataReceived) { // altimeter data was received
        flashWriteBufferSize = sprintf((char*) &flashWriteBuffer[0], "%s%f,%f,%f,", flashWriteBuffer, altTemperature, altPressure, altAltitude);
      }
#endif

#ifdef GPS_ENABLED
    //
    // Get data from GPS
    //
    gpsDataReceived = gpsReceive(&gpsSentence[0]);
    if (gpsDataReceived) { // gps data was received
      flashWriteBufferSize = sprintf((char*) &flashWriteBuffer[0], "%s%s,", flashWriteBuffer, gpsSentence);
    }
#endif

    if (flashWriteBufferSize > 0) { // Is there anything to write?
      flashWriteBuffer[flashWriteBufferSize-1] = '\n'; // Overwrite last comma with a \n
      flashWriteBuffer[flashWriteBufferSize] = '\r';   // Add a \r
      flashWriteBuffer[flashWriteBufferSize+1] = '\0'; // Terminate string with a null

      //
      // Write data to flash
      //
      FreeSpaceAvailable = flashstoreWriteRecord(&flashWriteBuffer[0], flashWriteBufferSize);

#ifdef DEBUG_FLASH
      //
      // Send write buffer over UART for debugging
      //
      UARTprintf("%s", flashWriteBuffer);
      delay(DEBUG_FLASH_DELAY);
#endif
    }
  } // main while end

  //
  // Out of flash memory. Output flash memory to console
  //
  UARTprintf("Accelerometer,Temperature,Pressure,Altitude,Identifier,Time,Latitude,Longitude,Fix Quality,Num of Sats,HDOP,GPS Altitude,Height of geoid above WGS84 ellipsoid, Time since DGPS,DGPS reference,Checksum\n\r");
  while (true) {
    setStatus(OUT_OF_FLASH);
    flashPackedChar = flashstoreGetData(flashCurrAddr);
    if((flashPackedChar & 0xFFFFFF00) == flashHeader) {
      flashRecordSize = flashPackedChar & 0xFF;
      for (index = 0; index < flashRecordSize; index++) {
        flashCurrAddr += FLASH_STORE_BLOCK_WRITE_SIZE;
        flashPackedChar = flashstoreGetData(flashCurrAddr);
        unpack(flashPackedChar, &flashWriteBuffer[0]);
        UARTprintf("%s", flashWriteBuffer);
      }
    }
    flashCurrAddr += FLASH_STORE_BLOCK_WRITE_SIZE;
    if (flashCurrAddr >= FLASH_STORE_END_ADDR) {
      flashCurrAddr = FLASH_STORE_START_ADDR;
      delay(5000);
      UARTprintf("Accelerometer,Temperature,Pressure,Altitude,Identifier,Time,Latitude,Longitude,Fix Quality,Num of Sats,HDOP,GPS Altitude,Height of geoid above WGS84 ellipsoid, Time since DGPS,DGPS reference,Checksum\n\r");
    }
  }
}

//*****************************************************************************
//
// Misc Functions
//
//*****************************************************************************
void
delay(uint32_t ui32ms) { // Delay in milliseconds
  MAP_IntMasterDisable();
  MAP_SysCtlDelay(MAP_SysCtlClockGet() / (3000 / ui32ms));
  MAP_IntMasterEnable();
}
bool
setStatus(StatusCode_t scStatus) { // Set status code
  if (!statusBusy) {
    statusCode = scStatus;
    return true;
  }
  return false;
}
void
Timer0IntHandler(void) { // Timer interrupt to handle status codes
  //
  // Disable interrupts to prevent loops
  //
  MAP_IntMasterDisable();

  //
  // Clear the timer interrupt
  //
  ROM_TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);

  if (!statusBusy) { // If a status code has already been initiated, do not overwrite
    statusBusy = true; // Set the busy state for status codes

    //
    // set color and delay
    //
    switch (statusCode) {
      case INITIALIZING: { // device is initializing
        statusColor = WHITE_LED;
        statusBlinkDelay[0] = STATUS_DASH;
        statusBlinkDelay[1] = STATUS_DASH;
        statusBlinkDelay[2] = STATUS_DASH;
        break;
      }
      case RUNNING: { // code is running
        statusColor = GREEN_LED;
        statusBlinkDelay[0] = STATUS_DOT;
        statusBlinkDelay[1] = STATUS_DOT;
        statusBlinkDelay[2] = STATUS_DOT;
        break;
      }
      case DRL_ERR: { // driver library encountered an error
        statusColor = RED_LED;
        statusBlinkDelay[0] = STATUS_DOT;
        statusBlinkDelay[1] = STATUS_DOT;
        statusBlinkDelay[2] = STATUS_DOT;
        break;
      }
      case ALT_CRC_ERR: { // altimeter calibration error
        statusColor = RED_LED;
        statusBlinkDelay[0] = STATUS_DOT;
        statusBlinkDelay[1] = STATUS_DOT;
        statusBlinkDelay[2] = STATUS_DASH;
        break;
      }
      case OUT_OF_FLASH: { // out of flash memory
        statusColor = BLUE_LED;
        statusBlinkDelay[0] = STATUS_DASH;
        statusBlinkDelay[1] = STATUS_DASH;
        statusBlinkDelay[2] = STATUS_DASH;
        break;
      }
      case ALT_ADC_CONV_ERR: { // ALT_ADC_CONV error
        statusColor = YELLOW_LED;
        statusBlinkDelay[0] = STATUS_DOT;
        statusBlinkDelay[1] = STATUS_DOT;
        statusBlinkDelay[2] = STATUS_DOT;
        break;
      }
      case ALT_ADC_R_WRITE_ERR: { // ALT_ADC_READ write error
        statusColor = YELLOW_LED;
        statusBlinkDelay[0] = STATUS_DOT;
        statusBlinkDelay[1] = STATUS_DOT;
        statusBlinkDelay[2] = STATUS_DASH;
        break;
      }
      case ALT_ADC_R_READ_ERR: { // ALT_ADC_READ read error
          statusColor = YELLOW_LED;
          statusBlinkDelay[0] = STATUS_DOT;
          statusBlinkDelay[1] = STATUS_DASH;
          statusBlinkDelay[2] = STATUS_DOT;
        break;
      }
      case ALT_PROM_R_WRITE_ERR: { // ALT_PROM_READ write error
        statusColor = YELLOW_LED;
        statusBlinkDelay[0] = STATUS_DOT;
        statusBlinkDelay[1] = STATUS_DASH;
        statusBlinkDelay[2] = STATUS_DASH;
        break;
      }
      case ALT_PROM_R_READ_ERR: { // ALT_PROM_READ read error
        statusColor = YELLOW_LED;
        statusBlinkDelay[0] = STATUS_DASH;
        statusBlinkDelay[1] = STATUS_DOT;
        statusBlinkDelay[2] = STATUS_DOT;
        break;
      }
      case ALT_RESET_ERR: { // ALT_RESET error
        statusColor = YELLOW_LED;
        statusBlinkDelay[0] = STATUS_DASH;
        statusBlinkDelay[1] = STATUS_DOT;
        statusBlinkDelay[2] = STATUS_DASH;
        break;
      }
    } //switch(code)
  }

  //
  // Configure LED
  //
  statusLEDOn = !statusLEDOn;
  if (statusLEDOn) {
    LEDOff(WHITE_LED); // Turn all LEDS off
  } else {
    if (statusBeepIndex < 3) {
      LEDOn(statusColor);
      statusDelayIndex++;
      if (statusBlinkDelay[statusBeepIndex]) {
          statusDelayIndex = 0;
          statusBeepIndex++;
      }
    } else if (statusBeepIndex < 3+STATUS_DELIMTER) {
      statusBeepIndex++;
    } else {
      statusBeepIndex = 0;
      statusBusy = false;
      statusCode = RUNNING;
    }
  }

  MAP_IntMasterEnable();
} //Timer0IntHandler()

//*****************************************************************************
//
// Initialize Peripherals
//
//*****************************************************************************
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
  MAP_ADCSequenceDisable(ADC0_BASE, 0);
  MAP_ADCSequenceConfigure(ADC0_BASE, 0, ADC_TRIGGER_PROCESSOR, 0);
  MAP_ADCSequenceStepConfigure(ADC0_BASE, 0, 0, ADC_CTL_CH0 | ADC_CTL_IE | ADC_CTL_END);
  MAP_ADCHardwareOversampleConfigure(ADC0_BASE, 64);
  MAP_ADCSequenceEnable(ADC0_BASE, 0);

  // Enable ADC interupts
  MAP_ADCIntEnable(ADC0_BASE, 0);
  MAP_ADCIntClear(ADC0_BASE, 0);
}
void
altInit(void) {
  //
  // Enable the I2C module used by the altimeter
  //
  I2CInit(ALT_BASE, ALT_SPEED);

  // Reset altimeter
  while (!altReset(ALT_BASE, ALT_ADDRESS)) {}
}
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
gyroInit(void) {
  //
  // Enable the I2C module used by the gyro
  //
  I2CInit(GYRO_BASE, GYRO_SPEED);

  gyroStartup(GYRO_BASE, GYRO_ADDRESS);
}
void
I2CInit(uint32_t ui32Base, bool bSpeed) {
  switch(ui32Base) {
    case I2C0_BASE: {
      //
      // Enable Peripherals used by I2C0
      //
      MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_I2C0);
      MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);

      //
      // Enable pin PB2 for I2C0 I2C0SCL
      //
      MAP_GPIOPinConfigure(GPIO_PB2_I2C0SCL);
      MAP_GPIOPinTypeI2CSCL(GPIO_PORTB_BASE, GPIO_PIN_2);

      //
      // Enable pin PB3 for I2C0 I2C0SDA
      //
      MAP_GPIOPinConfigure(GPIO_PB3_I2C0SDA);
      MAP_GPIOPinTypeI2C(GPIO_PORTB_BASE, GPIO_PIN_3);

      break;
    }
    case I2C1_BASE: {
      //
      // Enable Peripherals used by I2C1
      //
      MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_I2C1);
      MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

      //
      // Enable pin PA6 for I2C1 I2C1SCL
      //
      MAP_GPIOPinConfigure(GPIO_PA6_I2C1SCL);
      MAP_GPIOPinTypeI2CSCL(GPIO_PORTA_BASE, GPIO_PIN_6);

      //
      // Enable pin PA7 for I2C1 I2C1SDA
      //
      MAP_GPIOPinConfigure(GPIO_PA7_I2C1SDA);
      MAP_GPIOPinTypeI2C(GPIO_PORTA_BASE, GPIO_PIN_7);

      break;
    }
    case I2C2_BASE: {
      //
      // Enable Peripherals used by I2C2
      //
      MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_I2C2);
      MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);

      //
      // Enable pin PE4 for I2C2 I2C2SCL
      //
      MAP_GPIOPinConfigure(GPIO_PE4_I2C2SCL);
      MAP_GPIOPinTypeI2CSCL(GPIO_PORTE_BASE, GPIO_PIN_4);

      //
      // Enable pin PE5 for I2C2 I2C2SDA
      //
      MAP_GPIOPinConfigure(GPIO_PE5_I2C2SDA);
      MAP_GPIOPinTypeI2C(GPIO_PORTE_BASE, GPIO_PIN_5);

      break;
    }
    case I2C3_BASE: {
      //
      // Enable Peripherals used by I2C3
      //
      MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_I2C3);
      MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);

      //
      // Enable pin PD1 for I2C3 I2C3SDA
      //
      MAP_GPIOPinConfigure(GPIO_PD1_I2C3SDA);
      MAP_GPIOPinTypeI2C(GPIO_PORTD_BASE, GPIO_PIN_1);

      //
      // Enable pin PD0 for I2C3 I2C3SCL
      //
      MAP_GPIOPinConfigure(GPIO_PD0_I2C3SCL);
      MAP_GPIOPinTypeI2CSCL(GPIO_PORTD_BASE, GPIO_PIN_0);

      break;
    }
  }

  //
  // Enable the supplied I2C Base Clock
  //
  MAP_I2CMasterInitExpClk(ui32Base, MAP_SysCtlClockGet(), bSpeed);

  //
  // Enable supplied I2C Base Master Block
  //
  MAP_I2CMasterEnable(ui32Base);
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
SSIInit(uint32_t ui32Base, uint32_t ui32Protocol, uint32_t ui32Mode, uint32_t ui32BitRate, uint32_t ui32DataWidth) {
  switch (ui32Base) {
    case SSI0_BASE: {
      MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_SSI0);
      MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

      //
      // Enable pin PA2 for SSI0 SSI0CLK
      //
      MAP_GPIOPinConfigure(GPIO_PA2_SSI0CLK);
      MAP_GPIOPinTypeSSI(GPIO_PORTA_BASE, GPIO_PIN_2);

      //
      // Enable pin PA4 for SSI0 SSI0RX
      //
      MAP_GPIOPinConfigure(GPIO_PA4_SSI0RX);
      MAP_GPIOPinTypeSSI(GPIO_PORTA_BASE, GPIO_PIN_4);

      //
      // Enable pin PA5 for SSI0 SSI0TX
      //
      MAP_GPIOPinConfigure(GPIO_PA5_SSI0TX);
      MAP_GPIOPinTypeSSI(GPIO_PORTA_BASE, GPIO_PIN_5);

      //
      // Enable pin PA3 for SSI0 SSI0FSS
      //
      MAP_GPIOPinConfigure(GPIO_PA3_SSI0FSS);
      MAP_GPIOPinTypeSSI(GPIO_PORTA_BASE, GPIO_PIN_3);
    }
    case SSI1_BASE: {
      MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_SSI1);
      MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);

      //
      // Enable pin PF1 for SSI1 SSI1TX
      //
      MAP_GPIOPinConfigure(GPIO_PF1_SSI1TX);
      MAP_GPIOPinTypeSSI(GPIO_PORTF_BASE, GPIO_PIN_1);

      //
      // Enable pin PF0 for SSI1 SSI1RX
      // First open the lock and select the bits we want to modify in the GPIO commit register.
      //
      HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK) = GPIO_LOCK_KEY;
      HWREG(GPIO_PORTF_BASE + GPIO_O_CR) = 0x1;

      //
      // Now modify the configuration of the pins that we unlocked.
      //
      MAP_GPIOPinConfigure(GPIO_PF0_SSI1RX);
      MAP_GPIOPinTypeSSI(GPIO_PORTF_BASE, GPIO_PIN_0);

      //
      // Enable pin PF3 for SSI1 SSI1FSS
      //
      MAP_GPIOPinConfigure(GPIO_PF3_SSI1FSS);
      MAP_GPIOPinTypeSSI(GPIO_PORTF_BASE, GPIO_PIN_3);

      //
      // Enable pin PF2 for SSI1 SSI1CLK
      //
      MAP_GPIOPinConfigure(GPIO_PF2_SSI1CLK);
      MAP_GPIOPinTypeSSI(GPIO_PORTF_BASE, GPIO_PIN_2);
    }
    case SSI2_BASE: {
      MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_SSI2);
      MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);

      //
      // Enable pin PB4 for SSI2 SSI2CLK
      //
      MAP_GPIOPinConfigure(GPIO_PB4_SSI2CLK);
      MAP_GPIOPinTypeSSI(GPIO_PORTB_BASE, GPIO_PIN_4);

      //
      // Enable pin PB5 for SSI2 SSI2FSS
      //
      MAP_GPIOPinConfigure(GPIO_PB5_SSI2FSS);
      MAP_GPIOPinTypeSSI(GPIO_PORTB_BASE, GPIO_PIN_5);

      //
      // Enable pin PB6 for SSI2 SSI2RX
      //
      MAP_GPIOPinConfigure(GPIO_PB6_SSI2RX);
      MAP_GPIOPinTypeSSI(GPIO_PORTB_BASE, GPIO_PIN_6);

      //
      // Enable pin PB7 for SSI2 SSI2TX
      //
      MAP_GPIOPinConfigure(GPIO_PB7_SSI2TX);
      MAP_GPIOPinTypeSSI(GPIO_PORTB_BASE, GPIO_PIN_7);
    }
    case SSI3_BASE: {
      MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_SSI3);
      MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);

      //
      // Enable pin PD2 for SSI3 SSI3RX
      //
      MAP_GPIOPinConfigure(GPIO_PD2_SSI3RX);
      MAP_GPIOPinTypeSSI(GPIO_PORTD_BASE, GPIO_PIN_2);

      //
      // Enable pin PD3 for SSI3 SSI3TX
      //
      MAP_GPIOPinConfigure(GPIO_PD3_SSI3TX);
      MAP_GPIOPinTypeSSI(GPIO_PORTD_BASE, GPIO_PIN_3);

      //
      // Enable pin PD1 for SSI3 SSI3FSS
      //
      MAP_GPIOPinConfigure(GPIO_PD1_SSI3FSS);
      MAP_GPIOPinTypeSSI(GPIO_PORTD_BASE, GPIO_PIN_1);

      //
      // Enable pin PD0 for SSI3 SSI3CLK
      //
      MAP_GPIOPinConfigure(GPIO_PD0_SSI3CLK);
      MAP_GPIOPinTypeSSI(GPIO_PORTD_BASE, GPIO_PIN_0);
    }
  }

  //
  // Enable the supplied SSI Base Clock
  //
  MAP_SSIConfigSetExpClk(ui32Base, MAP_SysCtlClockGet(), ui32Protocol,
                         ui32Mode, ui32BitRate, ui32DataWidth);

  //
  // Enable supplied SSI Base Master Block
  //
  MAP_SSIEnable(ui32Base);
}
void
statusCodeInterruptInit(void) {
  //
  // Enable the perifpherals used by the timer interrupts
  //
  MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);

  //
  // Enable processor interrupts.
  //
  MAP_IntMasterEnable();

  //
  // Configure the 32-bit periodic timer for the status code length define.
  //
  MAP_TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);
  MAP_TimerLoadSet(TIMER0_BASE, TIMER_A, MAP_SysCtlClockGet() / (1000 / STATUS_CODE_LENGTH));

  //
  // Setup the interrupts for the timer timeouts.
  //
  MAP_IntEnable(INT_TIMER0A);
  MAP_TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);

  //
  // Enable the timer.
  //
  MAP_TimerEnable(TIMER0_BASE, TIMER_A);
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
  MAP_GPIOPinWrite(GPIO_PORTF_BASE, ui8Color, 0x0);
}
  // Receive Functions
void
accelReceive(uint32_t* ui32ptrData) {
  MAP_ADCProcessorTrigger(ADC0_BASE, 0);
  while(!MAP_ADCIntStatus(ADC0_BASE, 0, false)) {} // wait for a2d conversion
  MAP_ADCIntClear(ADC0_BASE, 0);
  MAP_ADCSequenceDataGet(ADC0_BASE, 0, ui32ptrData);
}
bool
altReceive(uint32_t ui32Base, uint8_t ui8AltAddr, uint8_t ui8OSR, uint16_t ui16Calibration[8], float* fTemp, float* fPressure, float* fAltitude)
{
  // Digital pressure and temp
  uint32_t D1 = 0;   // ADC value of the pressure conversion
  uint32_t D2 = 0;   // ADC value of the temperature conversion

  // Temp difference, offsets, and sensitivities
  int32_t  dT    = 0; // difference between actual and measured temperature
  int32_t  T2    = 0; // second order temperature offset
  int64_t  OFF   = 0; // offset at actual temperature
  int64_t  OFF2  = 0; // second order offset at actual temperature
  int64_t  SENS  = 0; // sensitivity at actual temperature
  int64_t  SENS2 = 0; // second order sensitivity at actual temperature

  if (altADCConversion(ui32Base, ui8AltAddr, ALT_ADC_D1+ui8OSR, &D1) && altADCConversion(ui32Base, ui8AltAddr, ALT_ADC_D2+ui8OSR, &D2)) {
    // Calculate 1st order pressure and temperature

    // Calculate 1st order temp difference, offset, and sensitivity (MS5607 1st order algorithm)
    dT=D2-ui16Calibration[5]*pow(2,8);
    OFF=ui16Calibration[2]*pow(2,17)+dT*ui16Calibration[4]/pow(2,6);
    SENS=ui16Calibration[1]*pow(2,16)+dT*ui16Calibration[3]/pow(2,7);

    // Calculate temperature
    *fTemp=(2000+(dT*ui16Calibration[6])/pow(2,23))/100;

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
    // TODO: Convert to meters
    *fAltitude = (1-pow((*fPressure/1013.25),.190284))*145366.45;
    return true;
  }
  return false;
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
        ui8Buffer[ui32bIndex] = ','; ui32bIndex++; // Delimit checksum data
        ui8Buffer[ui32bIndex] = MAP_UARTCharGet(UART4_BASE); ui32bIndex++; // Collect 1st checksum number
        ui8Buffer[ui32bIndex] = MAP_UARTCharGet(UART4_BASE); ui32bIndex++; // Collect 2nd checksum number

        // Add null terminator to end of GPS data
        ui8Buffer[ui32bIndex] = '\0';
        return true;
      }
    } //If char == $
  }
  return false;
}
bool
gyroReceive(uint32_t ui32Base, uint8_t ui8GyroAddr) {
  // TODO: Create Gyro Recieve code
  return false;
}
  // Altimeter Functions
bool
altADCConversion(uint32_t ui32Base, uint8_t ui8AltAddr, uint8_t ui8Cmd, uint32_t* ui32ptrData) {
  uint32_t ret[3];

  if (!I2CWrite(ALT_BASE, ALT_ADDRESS, ALT_ADC_CONV+ui8Cmd)) {
    setStatus(ALT_ADC_CONV_ERR);
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
  if (!I2CWrite(ALT_BASE, ALT_ADDRESS, ALT_ADC_READ)) {
    setStatus(ALT_ADC_R_WRITE_ERR);
    UARTprintf("ALT_ADC_READ write error\n\r");
    return false;
  }

  //
  // Start receiving data from altimeter
  //
  if (!I2CBurstRead(ALT_BASE, ALT_ADDRESS, &ret[0], 3)) {
    setStatus(ALT_ADC_R_READ_ERR);
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
altProm(uint32_t ui32Base, uint8_t ui8AltAddr, uint16_t ui16nProm[8]) {
  uint32_t i;
  uint32_t ret[2];
  for (i = 0; i < 8; i++) {
    if (!I2CWrite(ui32Base, ui8AltAddr, ALT_PROM_READ+(i*2))) {
      setStatus(ALT_PROM_R_WRITE_ERR);
      UARTprintf("ALT_PROM_READ write error\n\r");
      return false;
    }
    if (!I2CBurstRead(ui32Base, ui8AltAddr, &ret[0], 2)) {
      while (!setStatus(ALT_PROM_R_READ_ERR)) {}
      UARTprintf("ALT_PROM_READ read error\n\r");
      return false;
    }
    ui16nProm[i] = ((256 * ret[0]) + ret[1]);
  }
  return true;
}
bool
altReset(uint32_t ui32Base, uint8_t ui8AltAddr) {
  if (!I2CWrite(ui32Base, ui8AltAddr, ALT_RESET)) {
    while (!setStatus(ALT_RESET_ERR)) {}
    UARTprintf("ALT_RESET write error\n\r");
    return false;
  }
  delay(ALT_RESET_DELAY);
  return true;
}
  // Gyro Functions
void
gyroStartup(uint32_t ui32Base, uint8_t ui8GyroAddr) {
  uint8_t ui8Data[2];
  ui8Data[0] = GYRO_CTRL_REG1;
  ui8Data[1] = 0x1F;
  I2CBurstWrite(ui32Base, ui8GyroAddr, ui8Data, 2);
  ui8Data[0] = GYRO_CTRL_REG3;
  ui8Data[1] = 0x08;
  I2CBurstWrite(ui32Base, ui8GyroAddr, ui8Data, 2);
  ui8Data[0] = GYRO_CTRL_REG4;
  ui8Data[1] = 0x80;
  I2CBurstWrite(ui32Base, ui8GyroAddr, ui8Data, 2);
  delay(GYRO_STARTUP_DELAY);
}
  // I2C Functions
bool
I2CRead(uint32_t ui32Base, uint8_t ui8SlaveAddr, uint32_t* ui32ptrData) {
  //
  // Wait for the I2C Bus to finish
  //
  while(MAP_I2CMasterBusBusy(ui32Base)) {}

  //
  // Tell the master module what address it will place on the bus when
  // reading from the slave.
  //
  MAP_I2CMasterSlaveAddrSet(ui32Base, ui8SlaveAddr, I2C_MODE_READ);

  //
  // Disable Interrupts to prevent I2C comm failure
  //
  MAP_IntMasterDisable();

  //
  // Tell the master to read data.
  //
  MAP_I2CMasterControl(ui32Base, I2C_MASTER_CMD_SINGLE_RECEIVE);

  //
  // Wait until master module is done receiving.
  //
  while(MAP_I2CMasterBusy(ui32Base)) {}

  //
  // Reenable Interrupts
  //
  MAP_IntMasterEnable();

  //
  // Check for errors.
  //
  if(MAP_I2CMasterErr(ui32Base) != I2C_MASTER_ERR_NONE)
    return false;

  //
  // Get data
  //
  *ui32ptrData = MAP_I2CMasterDataGet(ui32Base);

  //
  // return the data from the master.
  //
  return true;
}
bool
I2CWrite(uint32_t ui32Base, uint8_t ui8SlaveAddr, uint8_t ui8SendData) {
  //
  // Wait for the I2C Bus to finish
  //
  while(MAP_I2CMasterBusBusy(ui32Base)) {}

  //
  // Tell the master module what address it will place on the bus when
  // writing to the slave.
  //
  MAP_I2CMasterSlaveAddrSet(ui32Base, ui8SlaveAddr, I2C_MODE_WRITE);

  //
  // Place the command to be sent in the data register.
  //
  MAP_I2CMasterDataPut(ui32Base, ui8SendData);

  //
  // Disable Interrupts to prevent I2C comm failure
  //
  MAP_IntMasterDisable();

  //
  // Initiate send of data from the master.
  //
  MAP_I2CMasterControl(ui32Base, I2C_MASTER_CMD_SINGLE_SEND);

  //
  // Wait until master module is done transferring.
  //
  while(MAP_I2CMasterBusy(ui32Base)) {}

  //
  // Reenable Interrupts
  //
  MAP_IntMasterEnable();

  //
  // Check for errors.
  //
  if(MAP_I2CMasterErr(ui32Base) != I2C_MASTER_ERR_NONE)
    return false;

  //
  // Return 1 if there is no error.
  //
  return true;
}
bool
I2CBurstRead(uint32_t ui32Base, uint8_t ui8SlaveAddr, uint32_t* ui32ptrReadData, uint32_t ui32Size) {
  //
  // Use I2C single read if theres only 1 item to receive
  //
  if (ui32Size == 1)
    return I2CRead(ui32Base, ui8SlaveAddr, &ui32ptrReadData[0]);

  uint32_t ui32ByteCount;        // local variable used for byte counting/state determination
  uint32_t MasterOptionCommand; // used to assign the control commands

  //
  // Wait for the I2C Bus to finish
  //
  while(MAP_I2CMasterBusBusy(ui32Base)) {}

  //
  // Tell the master module what address it will place on the bus when
  // reading from the slave.
  //
  MAP_I2CMasterSlaveAddrSet(ui32Base, ui8SlaveAddr, I2C_MODE_READ);

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
    // Disable Interrupts to prevent I2C comm failure
    //
    MAP_IntMasterDisable();

    //
    // Initiate read of data from the slave.
    //
    MAP_I2CMasterControl(ui32Base, MasterOptionCommand);

    //
    // Wait until master module is done reading.
    //
    while(MAP_I2CMasterBusy(ui32Base)) {}

    //
    // Reenable Interrupts
    //
    MAP_IntMasterEnable();

    //
    // Check for errors.
    //
    if (MAP_I2CMasterErr(ui32Base) != I2C_MASTER_ERR_NONE)
      return false;

    //
    // Move byte from register
    //
    ui32ptrReadData[ui32ByteCount] = MAP_I2CMasterDataGet(ui32Base);
  }

  //
  // Return 1 if there is no error.
  //
  return true;
}
bool
I2CBurstWrite(uint32_t ui32Base, uint8_t ui8SlaveAddr, uint8_t ui8SendData[], uint32_t ui32Size) {
  //
  // Use I2C single write if theres only 1 item to send
  //
  if (ui32Size == 1)
    return I2CWrite(ui32Base, ui8SlaveAddr, ui8SendData[0]);

  uint32_t uiByteCount;         // local variable used for byte counting/state determination
  uint32_t MasterOptionCommand; // used to assign the control commands

  //
  // Wait for the I2C Bus to finish
  //
  while(MAP_I2CMasterBusBusy(ui32Base)) {}

  //
  // Tell the master module what address it will place on the bus when
  // writing to the slave.
  //
  MAP_I2CMasterSlaveAddrSet(ui32Base, ui8SlaveAddr, I2C_MODE_WRITE);

  //
  // Wait until master module is done transferring.
  //
  while(MAP_I2CMasterBusy(ui32Base)) {}

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
    MAP_I2CMasterDataPut(ui32Base, ui8SendData[uiByteCount]);

    //
    // Disable Interrupts to prevent I2C comm failure
    //
    MAP_IntMasterDisable();

    //
    //
    // Initiate send of data from the master.
    //
    MAP_I2CMasterControl(ui32Base, MasterOptionCommand);

    //
    //
    // Wait until master module is done transferring.
    //
    while(MAP_I2CMasterBusy(ui32Base)) {}

    //
    // Reenable Interrupts
    //
    MAP_IntMasterEnable();

    //
    // Check for errors.
    //
    if(MAP_I2CMasterErr(ui32Base) != I2C_MASTER_ERR_NONE) return false;
  }

  //
  // Return 1 if there is no error.
  //
  return true;
}
