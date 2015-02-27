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

#include "driverlib/debug.h"
#include "driverlib/fpu.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "driverlib/sysctl.h"

#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "inc/hw_nvic.h"

#include "peripherals/altimeter.h"
#include "peripherals/accel250.h"
#include "peripherals/flashstore.h"
#include "peripherals/gyro.h"
#include "peripherals/i2c.h"
#include "peripherals/misc.h"
#include "peripherals/status.h"
#include "peripherals/uartstdio.h"

//*****************************************************************************
//
// Peripheral defines
//
//*****************************************************************************
// 250G Accelerometer
#define ACCEL_250_ENABLED

// Altimeter
#define ALT_ENABLED
#define ALT_BASE    I2C0_BASE
#define ALT_ADDRESS ALT_ADDRESS_CSB_LO
#define ALT_SPEED   I2C_SPEED_400

// Flash
#define DEBUG_FLASH
#define DEBUG_FLASH_DELAY 500 // Output delay in ms

// GPS
// #define GPS_ENABLED
#define GPS_BASE UART4_BASE
#define GPS_BAUD 9600
#define GPS_UART_CONFIG UART_CONFIG_WLEN_8 | UART_CONFIG_PAR_NONE | UART_CONFIG_STOP_ONE
#define GPS_NAV_SENSE_BASE GPIO_PORTC_BASE // Navigation lock sense base
#define GPS_NAV_SENSE_PIN GPIO_PIN_6 // Navigation lock sense pin

// Gyro
#define GYRO_ENABLED
#define GYRO_BASE    I2C1_BASE
#define GYRO_ADDRESS GYRO_ADDRESS_SDO_LO
#define GYRO_SPEED   I2C_SPEED_400
#define GYRO_DRDY_SENSE_BASE GPIO_PORTA_BASE
#define GYRO_DRDY_SENSE_PIN GPIO_PIN_5

// microSD
#define mSD_CD_SENSE_BASE GPIO_PORTD_BASE // Card Detect sense base
#define mSD_CD_SENSE_PIN GPIO_PIN_6 // Card Detect sense pin

// Status codes
#define STATUS_CODES_ENABLED

// Prototypes
void hardReset(void);

//*****************************************************************************
//
// Main function
//
//*****************************************************************************
int32_t
main(void) {
  //
  ///////////////////Initialize//////////////////////////////
  //

  //
  // Set the clocking to run at 50MHz.
  //

  MAP_SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);

  StatusCode_t status;
#ifdef STATUS_CODES_ENABLED
  //
  // Enable status codes
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

#ifdef ACCEL_250_ENABLED
  //
  // Accelerometer variables
  //
  float accel250Data = 0; // Accelerometer data

  //
  // Enable the accelerometer
  //
  accel250Init();
#endif

#ifdef ALT_ENABLED
  //
  // Altimeter variables
  //
  float altTemperature; // compensated temperature value
  float altPressure;    // compensated pressure value
  float altAltitude;    // Calculated altitude

  //
  // Enable the altimeter
  //
  do {
    status = altInit(ALT_BASE, ALT_ADDRESS, ALT_SPEED);
    setStatus(status);
  } while (status != INITIALIZING);

#endif

#ifdef GPS_ENABLED
  //
  // GPS Variables
  //
  bool     gpsDataReceived = false; // GPS data has been received
  uint8_t  gpsSentence[128]; // GPS NMEA sentence

  //
  // Enable the GPS
  //
  gpsInit(GPS_BASE, GPS_BAUD, GPS_UART_CONFIG);
#endif

#ifdef GYRO_ENABLED
  //
  // Gyro Variables
  //
  bool gyroDataReceived = false; // Gyro data has been received
  float gyroDPS[3]; // Gyro degrees per second, each axis

  // TODO: Setup timer to be able to accurately determine heading from DPS
  // float gyroHeading[3]; // heading[x], heading[y], heading [z]

  //
  // Enable the gyro
  do {
    status = gyroInit(GYRO_BASE, GYRO_ADDRESS, GYRO_SPEED, GYRO_DRDY_SENSE_BASE, GYRO_DRDY_SENSE_PIN);
    setStatus(status);
  } while (status != INITIALIZING);
#endif

  //
  // Flash storage variables
  //
  bool     FreeSpaceAvailable = flashstoreInit(false); // Initialize flash and see if space is available
  uint32_t index;
  uint32_t flashCurrAddr = FLASH_STORE_START_ADDR; // Current address in flash storage for read. Initialize to starting address
  uint32_t flashHeader = FLASH_STORE_RECORD_HEADER; // Magic Header Byte to find record beginning
  uint32_t flashPackedChar; // 4 Byte return from storage. Theoretically holds 4 packed chars
  uint32_t flashRecordSize; // Size of the current flash record
  uint8_t  flashWriteBuffer[256] = {0}; // Buffer of the data to write
  int32_t  flashWriteBufferSize = 0;  // Length of the record to write

  //
  ///////////////////Gather Data/////////////////////////////
  //

  setStatusDefault(RUNNING);
  while(FreeSpaceAvailable) {
#ifdef ACCEL_250_ENABLED
    //
    // Get data from accelerometer
    //
    status = accel250Receive(&accel250Data);
    if (status == RUNNING) { // Data receive was successful
      flashWriteBufferSize = sprintf((char*) &flashWriteBuffer[0], "%f,", accel250Data);
    } else {
      setStatus(status);
    }
#endif

#ifdef ALT_ENABLED
    //
    // Get data from altimeter
    //l
    status = altReceive(ALT_ADC_4096, &altTemperature, &altPressure, &altAltitude);
    if (status == RUNNING) { // Data receive was successful
      flashWriteBufferSize = sprintf((char*) &flashWriteBuffer[0], "%s%f,%f,%f,", flashWriteBuffer, altTemperature, altPressure, altAltitude);
    } else {
      setStatus(status);
    }
#endif

#ifdef GPS_ENABLED
    //
    // Get data from GPS
    //
    gpsDataReceived = gpsReceive(&gpsSentence[0]);
    if (gpsDataReceived) // gps data was received
      flashWriteBufferSize = sprintf((char*) &flashWriteBuffer[0], "%s%s,", flashWriteBuffer, gpsSentence);
#endif

#ifdef GYRO_ENABLED
    //
    // Get data from gyro
    //
    gyroDataReceived = gyroReceive(&gyroDPS[0]);
    if (gyroDataReceived) // gps data was received
      flashWriteBufferSize = sprintf((char*) &flashWriteBuffer[0], "%s%f,%f,%f,", flashWriteBuffer, gyroDPS[0], gyroDPS[1], gyroDPS[2]);
#endif

    if (flashWriteBufferSize > 0) { // Is there anything to write?
      flashWriteBuffer[flashWriteBufferSize-1] = '\n'; // Overwrite last comma with a \n
      flashWriteBuffer[flashWriteBufferSize] = '\r';   // Add a \r
      flashWriteBuffer[flashWriteBufferSize+1] = '\0'; // Terminate string with a null

#ifdef DEBUG_FLASH
      //
      // Send write buffer over UART for debugging
      //
      if (consoleIsEnabled()) UARTprintf("%s", flashWriteBuffer);
      delay(DEBUG_FLASH_DELAY);
#endif

      //
      // Write data to flash
      //
      FreeSpaceAvailable = flashstoreWriteRecord(&flashWriteBuffer[0], flashWriteBufferSize);

      //
      // Reset write buffer after data is written
      //
      flashWriteBuffer[0] = '\0'; // Set first value to null to terminate string and ignore all other data
      flashWriteBufferSize = 0; // Reset the buffer size to 0
    }
  } // main while end

  //
  // Out of flash memory. Output flash memory to console
  //
  while (true) {
    setStatusDefault(OUT_OF_FLASH);
    flashPackedChar = flashstoreGetData(flashCurrAddr);
    if((flashPackedChar & 0xFFFFFF00) == flashHeader) {
      flashRecordSize = flashPackedChar & 0xFF;
      for (index = 0; index < flashRecordSize; index++) {
        flashCurrAddr += FLASH_STORE_BLOCK_WRITE_SIZE;
        flashPackedChar = flashstoreGetData(flashCurrAddr);
        unpack(flashPackedChar, &flashWriteBuffer[0]);
        if (consoleIsEnabled()) UARTprintf("%s", flashWriteBuffer);
      }
    }
    flashCurrAddr += FLASH_STORE_BLOCK_WRITE_SIZE;
    if (flashCurrAddr >= FLASH_STORE_END_ADDR) {
      flashCurrAddr = FLASH_STORE_START_ADDR;
      delay(5000);
      if (consoleIsEnabled()) UARTprintf("***********************************************************************\n\r");
    }
  }
}

//*****************************************************************************
//
// Fault handlers declared in tm4c123gh6pm_startup_ccs.c
//
//*****************************************************************************
void busFaultHandler(void) {
  uint32_t i=0;
  while (i<5000) {
    setStatusDefault(BUS_FAULT);
    delay(1);
    i++;
  }
  hardReset();
}
void usageFaultHandler(void) {
  uint32_t i=0;
  while (i<5000) {
    setStatusDefault(USAGE_FAULT);
    delay(1);
    i++;
  }
  hardReset();
}

#ifdef DEBUG
//*****************************************************************************
//
// The error routine that is called if the driver library encounters an error.
//
//*****************************************************************************
void
__error__(char *pcFilename, uint32_t ui32Line) {
  while(true) {
    if (consoleIsEnabled()) UARTprintf("Error at line %d of %s\n\r", ui32Line, pcFilename);
    setStatus(DRL_ERR);
    delay(1000);
  }
}
#endif

// Hard Reset
void
hardReset(void) {
  HWREG(NVIC_APINT) = NVIC_APINT_VECTKEY | NVIC_APINT_SYSRESETREQ;
}
