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

#include "peripherals/altimeter.h"
#include "peripherals/accel.h"
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

// Accelerometer
#define ACCEL_ENABLED
#define ACCEL_BASE ADC0_BASE

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
#define GPS_BASE UART4_BASE
#define GPS_BAUD 9600
#define GPS_CONFIG UART_CONFIG_WLEN_8 | UART_CONFIG_PAR_NONE | UART_CONFIG_STOP_ONE

// Gyro
// #define GYRO_ENABLED
#define GYRO_BASE    I2C1_BASE
#define GYRO_ADDRESS GYRO_ADDRESS_SDO_LO
#define GYRO_SPEED   I2C_SPEED_400

// Determines if status codes are enabled. Comment out to disable functionality
#define STATUS_CODES_ENABLED

//*****************************************************************************
//
// Prototypes
//
//*****************************************************************************
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

#ifdef ACCEL_ENABLED
  //
  // Accelerometer variables
  //
  float accelData = 0; // Accelerometer data

  //
  // Enable the accelerometer
  //
  accelInit();
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

  //
  // Enable the altimeter
  //
  altInit(ALT_BASE, ALT_ADDRESS, ALT_SPEED);

  while (!altProm(ALT_BASE, ALT_ADDRESS, altCalibration)) {} // read coefficients
  altCRC = altCRC4(altCalibration); // calculate the CRC.
  while (altCRC != (altCalibration[7] & 0x000F)) { // If prom CRC (Last byte of coefficient 7) and calculated CRC do not match, something went wrong!
    setStatus(ALT_CRC_ERR);
  }
#endif

#ifdef GPS_ENABLED
  //
  // GPS Variables
  //
  bool     gpsDataReceived = false;  // GPS data has been received
  uint8_t  gpsSentence[128]; // GPS NMEA sentence

  //
  // Enable the GPS
  //
  gpsInit(GPS_BASE, GPS_BAUD, GPS_CONFIG);
#endif

#ifdef GYRO_ENABLED
  //
  // Gyro Variables
  //
  bool gyroDataReceived = false; // Gyro data has been received
  float gyroDPS[3];       // Gyro degrees per second, each axis

  // TODO: Setup timer to be able to accurately determine heading from DPS
  // float gyroHeading[3]; // heading[x], heading[y], heading [z]

  uint32_t gyroRaw[3];       // Raw gyro values
  uint32_t gyroZeroRate[3];  // Calibration data. Needed because the sensor does center at zero, but rather always reports a small amount of rotation on each axis.
  uint32_t gyroThreshold[3]; // Raw rate change data less than the statistically derived threshold is discarded.

  //
  // Enable the gyro
  //
  gyroInit(GYRO_BASE, GYRO_ADDRESS, GYRO_SPEED);
  gyroCalibrate(GYRO_BASE, GYRO_ADDRESS, GYRO_NUM_SAMPLES, GYRO_SIGMA_MULTIPLE, &gyroZeroRate[0], &gyroThreshold[0], &gyroRaw[0], &gyroDPS[0]);
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
  uint8_t  flashWriteBuffer[256]; // Buffer of the data to write
  int32_t  flashWriteBufferSize = 0;  // Length of the record to write

  //
  ///////////////////Gather Data/////////////////////////////
  //

  while(FreeSpaceAvailable) {
    setStatusDefault(RUNNING);
#ifdef ACCEL_ENABLED
    //
    // Get data from accelerometer
    //
    accelReceive(ACCEL_BASE, &accelData);
    flashWriteBufferSize = sprintf((char*) &flashWriteBuffer[0], "%f,", accelData);
#endif

#ifdef ALT_ENABLED
    //
    // Get data from altimeter
    //
    altDataReceived = altReceive(ALT_BASE, ALT_ADDRESS, ALT_ADC_4096, altCalibration, &altTemperature, &altPressure, &altAltitude);
    if (altDataReceived) // altimeter data was received
      flashWriteBufferSize = sprintf((char*) &flashWriteBuffer[0], "%s%f,%f,%f,", flashWriteBuffer, altTemperature, altPressure, altAltitude);
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
    gyroDataReceived = gyroReceive(GYRO_BASE, GYRO_ADDRESS, &gyroZeroRate[0], &gyroThreshold[0], &gyroRaw[0], &gyroDPS[0]);
    if (gyroDataReceived) // gps data was received
      flashWriteBufferSize = sprintf((char*) &flashWriteBuffer[0], "%s%f,%f,%f,", flashWriteBuffer, gyroDPS[0], gyroDPS[1], gyroDPS[2]);
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
      if (consoleIsEnabled()) UARTprintf("%s", flashWriteBuffer);
      delay(DEBUG_FLASH_DELAY);
#endif
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


