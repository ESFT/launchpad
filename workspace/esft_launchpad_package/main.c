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
#include "driverlib/interrupt.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/timer.h"

#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_nvic.h"
#include "inc/hw_types.h"

#include "peripherals/fatfs/diskio.h"
#include "peripherals/fatfs/ff.h"
#include "peripherals/RFM12B/RFM12B.h"
#include "peripherals/tinygps/tinygps.h"
#include "peripherals/altimeter.h"
#include "peripherals/accel250.h"
#include "peripherals/gpio.h"
#include "peripherals/gps.h"
#include "peripherals/gyro.h"
#include "peripherals/i2c.h"
#include "peripherals/compass.h"
#include "peripherals/misc.h"
#include "peripherals/status.h"
#include "peripherals/uartstdio.h"

//*****************************************************************************
//
// Peripheral defines
//
//*****************************************************************************

// I2C Options
#define I2C_BASE  I2C0_BASE
#define I2C_SPEED I2C_SPEED_400

// 250G Accelerometer
//#define ACCEL_250_ENABLED

// Altimeter
//#define ALT_ENABLED
#define ALT_BASE    I2C_BASE
#define ALT_ADDRESS ALT_ADDRESS_CSB_LO

// Compass
//#define COMPASS_ENABLED
#define COMPASS_BASE I2C_BASE
#define COMPASS_X_AXIS 0 // Array index that contains corrected z-axis when mounted
#define COMPASS_Y_AXIS 1 // Array index that contains corrected y-axis when mounted
#define COMPASS_Z_AXIS 2 // Array index that contains corrected z-axis when mounted
#define COMPASS_HARD_IRON_X 0 // hard iron estimate for the X axis
#define COMPASS_HARD_IRON_Y 0 // hard iron estimate for the Y axis
#define COMPASS_HARD_IRON_Z 0 // hard iron estimate for the Z axis

// Console
#define CONSOLE_ENABLED
#define CONSOLE_DEBUG
#define CONSOLE_DEBUG_DELAY 500 // Flash output delay in ms (Slows down for human readability)

// ematch
#define EMATCH_BASE        GPIO_PORTC_BASE
#define EMATCH_DROUGE_PRIM GPIO_PIN_4
#define EMATCH_DROUGE_BACK GPIO_PIN_5
#define EMATCH_MAIN_PRIM   GPIO_PIN_6
#define EMATCH_MAIN_BACK   GPIO_PIN_7
#define EMATCH_STRENGTH    GPIO_STRENGTH_8MA     // 8 mA output
#define EMATCH_PIN_TYPE    GPIO_PIN_TYPE_STD_WPD // Weak pulldown
#define EMATCH_DROGUE_PRIM_FIRE_ALT_DIFF 20 // In meters
#define EMATCH_DROGUE_BACK_FIRE_ALT_DIFF  40 // In meters
#define EMATCH_MAIN_PRIM_FIRE_ALT_DIFF   100 // In meters
#define EMATCH_MAIN_BACK_FIRE_ALT_DIFF    1500 // In meters

// Flash
#define CONSOLE_DEBUG

// GPS
#define GPS_ENABLED
#define GPS_BASE                UART1_BASE // Be sure to change the interrupt function in startup file
#define GPS_NAV_LOCK_SENSE_BASE GPIO_PORTC_BASE
#define GPS_NAV_LOCK_SENSE_PIN  GPIO_PIN_6
//#define GPS_WAIT_FOR_LOCK

// Gyro
//#define GYRO_ENABLED
#define GYRO_BASE            I2C_BASE
#define GYRO_ADDRESS         GYRO_ADDRESS_SDO_LO
#define GYRO_DRDY_SENSE_BASE GPIO_PORTB_BASE
#define GYRO_DRDY_SENSE_PIN  GPIO_PIN_5
#define GYRO_X_AXIS 0 // Array index that contains corrected X-axis when mounted
#define GYRO_Y_AXIS 1 // Array index that contains corrected y-axis when mounted
#define GYRO_Z_AXIS 2 // Array index that contains corrected z-axis when mounted

// Limit SW GPIO
#define LIMIT_SW_BASE     GPIO_PORTE_BASE
#define LIMIT_SW_DROUGE   GPIO_PIN_1
#define LIMIT_SW_MAIN     GPIO_PIN_2
#define LIMIT_SW_PIN_TYPE GPIO_PIN_TYPE_STD_WPU // Weak pullup

// Status codes
#define STATUS_CODES_ENABLED
#define STATUS_CODE_TIMER_BASE TIMER0_BASE

// Transceiver
//#define TRANSCEIVER_ENABLED
#define TRANSCEIVER_NODEID    2
#define TRANSCEIVER_NETWORKID 99
#define TRANSCEIVER_GATEWAYID 1
#define TRANSCEIVER_END "KR0KCT"

// Prototypes
void softReset(void);
void waitForSwitchPress(uint8_t ui8Pins);
void ematchInit(void);
void limitSWInit(void);
void ematchFire(uint8_t ui8Pins);
bool limitSWPressed(uint8_t ui8Pins);

static bool _bTick = false;

static bool   _bAltimeterActive = false;
static float* _fAltMaxAltRecordedPtr;
static float* _fAltCurrAltRecordedPtr;
static float* _fGPSMaxAltRecordedPtr;
static float* _fGPSCurrAltRecordedPtr;

static bool   _bDroguePrimFired = false;
static bool   _bDrogueBackFired = false;
static bool   _bMainPrimFired   = false;
static bool   _bMainBackFired   = false;

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

  //
  // Enable processor interrupts.
  //
  MAP_IntMasterEnable();

  //
  // Configure SysTick for a 200Hz interrupt.
  // FatFs driver wants a 10 ms tick (every other tick)
  // Chute deployment system wants a 5ms tick (every tick)
  //
  MAP_SysTickPeriodSet(MAP_SysCtlClockGet() / 200);
  MAP_SysTickEnable();
  MAP_SysTickIntEnable();

  //
  // Enable LED control
  //
  LEDInit();

  //
  // Enable button control
  //
  buttonsInit();

  //
  // Enable ematch control
  //
  ematchInit();

  //
  // Enable seperation detection
  //
  limitSWInit();

  StatusCode_t status;
#ifdef STATUS_CODES_ENABLED
  //
  // Enable status codes
  //
  statusCodeInterruptInit(STATUS_CODE_TIMER_BASE);
#endif

  //
  // Enable the FPU
  //
  FPUInit();

#ifdef CONSOLE_ENABLED
  //
  // Enable the Console IO
  //
  consoleInit();
#ifdef CONSOLE_DEBUG
  if (consoleIsEnabled()) UARTprintf("%s", "Console flash debug is enabled!\n\r");
#endif
#endif


#ifdef ACCEL_250_ENABLED
  //
  // Accelerometer variables
  //
  bool  accel250DataReceived; // 250G Accelerometer data has been received
  float accel250Data = 0;  // 250G Accelerometer data in m/s^2

  //
  // Enable the accelerometer
  //
  accel250Init(&accel250Data);
#endif

#ifdef ALT_ENABLED
  //
  // Altimeter variables
  //
  bool  altDataReceived; // Altimeter data has been received
  AltData_t altData; // Altimeter data
  float fAltMaxAltRecorded;
  float fAltCurrAltRecorded;

  _fAltMaxAltRecordedPtr = &fAltMaxAltRecorded;
  _fAltCurrAltRecordedPtr = &fAltCurrAltRecorded;

  //
  // Enable the altimeter
  //
  do {
    status = altInit(ALT_BASE, ALT_ADDRESS, I2C_SPEED, altDataPtr);
    setStatus(status);
  } while (status != INITIALIZING);

#endif

#ifdef GPS_ENABLED
  //
  // GPS Variables
  //
  bool     gpsDataReceived = false; // GPS data has been received
  int8_t*   gpsGPGGAPtr = gpsGPGGA();
  GPSData_t gpsData; // GPS data
  float fGPSCurrAltRecorded;
  _fGPSCurrAltRecordedPtr = &fGPSCurrAltRecorded;
  _fGPSMaxAltRecordedPtr = gpsMaxAlt();

  //
  // Enable the GPS
  //
  gpsInit(GPS_BASE, GPS_BAUD, GPS_UART_CONFIG, &gpsData);

#ifdef GPS_WAIT_FOR_LOCK
  while (!MAP_GPIOPinRead(GPS_NAV_LOCK_SENSE_BASE, GPS_NAV_LOCK_SENSE_PIN)) {
    setStatus(GPS_NAV_LOCK_HOLD);
  }
  while (!gpsAvailable) {}
#endif
#endif

#ifdef GYRO_ENABLED
  //
  // Gyro Variables
  //
  bool gyroDataReceived = false; // Gyro data has been received
  float gyroDPS[3]; // Gyro degrees per second, each axis

  //
  // Enable the gyro
  //
  do {
    status = gyroInit(GYRO_BASE, GYRO_ADDRESS, I2C_SPEED, GYRO_DRDY_SENSE_BASE, GYRO_DRDY_SENSE_PIN, &gyroDPS[0]);
    setStatus(status);
  } while (status != INITIALIZING);
#endif

#ifdef COMPASS_ENABLED
  //
  // Compass
  //
  bool  compassAccelReceived;
  bool  compassMagReceived;
  float compassAccel[3]; // Accelerometer values in m/s^2
  float compassMag[3];   // Magnetometer values in micro-teslas
  float compassHeading[3]; // eCompass calulcated headings

  //
  // Enable the gyro
  //
  do {
    status = compassInit(COMPASS_BASE, I2C_SPEED);
    setStatus(status);
  } while (status != INITIALIZING);
#endif

#ifdef TRANSCEIVER_ENABLED
  RFM12BInitialize(TRANSCEIVER_NODEID, RF12_433MHZ, TRANSCEIVER_NETWORKID, 0, 0, RF12_3v15);
  uint32_t transBuffSize = 0;
  uint8_t transBuff[128] = "";
#endif

  //
  // mSD storage variables
  //
  FATFS    flashMount;             // FatFS object
  FIL      flashFile;              // File object
  char     flashFilename[80];      // Filename
  int32_t  flashFilenameIndex = 0; // Appended index number
  bool     flashSpaceAvail = true;
  uint8_t  flashWriteBuffer[256] = {0}; // Buffer of the data to write
  int32_t  flashWriteBufferSize  =  0 ; // Length of the data to write

  while (f_mount(&flashMount, "", 1) != FR_OK) {
    setStatusDefault(MMC_MOUNT_ERR);
  }

  do {
    sprintf(&flashFilename[0], "data%d.txt", flashFilenameIndex);
    flashFilenameIndex++;
  } while (f_stat(&flashFilename[0], NULL) != FR_NO_FILE);

  while (f_open(&flashFile, (TCHAR*) &flashFilename[0], FA_WRITE | FA_CREATE_ALWAYS) != FR_OK) {
    setStatusDefault(MMC_OPEN_ERR);
  }

  if (flashMount.free_clust < 1) {
    flashSpaceAvail = false;
  } else {
    setStatusDefault(INITIALIZING_HOLD);
    waitForSwitchPress(SW1_BUTTON);
  }

  //
  ///////////////////Gather Data/////////////////////////////
  //
  setStatusDefault(RUNNING);
  while(flashSpaceAvail) {

#ifdef ACCEL_250_ENABLED
    //
    // Get data from accelerometer
    //
    accel250DataReceived = accel250Receive();
#endif

#ifdef ALT_ENABLED
    //
    // Get data from altimeter
    //
    if (_bAltimeterActive) {
      altDataReceived = altReceive(ALT_ADC_4096, &altData);
      *_fAltCurrAltRecordedPtr = altData.altitude;
      if (*_fAltCurrAltRecordedPtr > *_fAltMaxAltRecordedPtr) *_fAltMaxAltRecordedPtr = *_fAltCurrAltRecordedPtr;
    }
#endif

#ifdef GPS_ENABLED
    //
    // Check if new GPS data is available
    //
    gpsDataReceived = gpsAvailable();
#endif

#ifdef GYRO_ENABLED
    //
    // Get data from gyro
    //
    gyroDataReceived = gyroReceive();
#endif

#ifdef COMPASS_ENABLED
    //
    // Get data from magnetometer/accelerometer
    //
    compassAccelReceived = compassAccelReadXYZ(&compassAccel[0]);
    compassMagReceived = compassMagReadXYZ(&compassMag[0]);
#endif

#ifdef ACCEL_250_ENABLED
    if (accel250DataReceived) { // 250G accel data was received
      flashWriteBufferSize = sprintf((char*) &flashWriteBuffer[0], "%f,", accel250Data);
    } else {
      flashWriteBufferSize = sprintf((char*) &flashWriteBuffer[0], ",");
      setStatus(ACC250_ADC_CONV_ERR);
    }
#endif

#ifdef ALT_ENABLED
    if (_bAltimeterActive && altDataReceived) { // alt data was received
      flashWriteBufferSize += sprintf((char*) &flashWriteBuffer[flashWriteBufferSize], "%d,%f,%f,%f,", _bAltimeterActive, altData.temperature, altData.pressure, altData.altitude);
    } else {
      flashWriteBufferSize += sprintf((char*) &flashWriteBuffer[flashWriteBufferSize], ",,,,");
      setStatus(ALT_ADC_CONV_ERR);
    }
#endif

#ifdef GPS_ENABLED
    flashWriteBufferSize += sprintf((char*) &flashWriteBuffer[flashWriteBufferSize], "%d,%d,%d,%d,%d,%d,%d,%d,%u.l,%f,%f,%f,%f,%s,%f,",
      gpsDataReceived, gpsData.year, (uint8_t) gpsData.month, (uint8_t) gpsData.day, (uint8_t) gpsData.hour, (uint8_t) gpsData.minute, (uint8_t) gpsData.second, (uint8_t) gpsData.hundredths,
      (unsigned long) gpsData.fix_age, gpsData.latitude, gpsData.longitude, gpsData.altitude, gpsData.course, (char*) gpsData.cardinal, gpsData.speed);

    *_fGPSCurrAltRecordedPtr = gpsData.altitude;
    if (*_fGPSCurrAltRecordedPtr > *_fGPSMaxAltRecordedPtr) *_fGPSMaxAltRecordedPtr = *_fGPSCurrAltRecordedPtr;
#ifndef ALT_ENABLED
    *_fAltCurrAltRecordedPtr = *_fGPSCurrAltRecordedPtr;
    *_fAltMaxAltRecordedPtr = *_fGPSMaxAltRecordedPtr;
#endif
#endif

#ifdef GYRO_ENABLED
    if (gyroDataReceived) { // gyro data was received
      flashWriteBufferSize += sprintf((char*) &flashWriteBuffer[flashWriteBufferSize], "%f,%f,%f,", gyroDPS[GYRO_X_AXIS], gyroDPS[GYRO_Y_AXIS], gyroDPS[GYRO_Z_AXIS]);
    } else {
      flashWriteBufferSize += sprintf((char*) &flashWriteBuffer[flashWriteBufferSize], ",,,");
    }
#endif

#ifdef COMPASS_ENABLED
    if (compassAccelReceived) { // compass accelerometer data was received
      flashWriteBufferSize += sprintf((char*) &flashWriteBuffer[flashWriteBufferSize], "%f,%f,%f,", compassAccel[COMPASS_X_AXIS], compassAccel[COMPASS_Y_AXIS], compassAccel[COMPASS_Z_AXIS]);
    } else {
      flashWriteBufferSize += sprintf((char*) &flashWriteBuffer[flashWriteBufferSize], ",,,");
    }
    if (compassMagReceived) { // compass magnetometer data was received
      flashWriteBufferSize += sprintf((char*) &flashWriteBuffer[flashWriteBufferSize], "%f,%f,%f,", compassMag[COMPASS_X_AXIS], compassMag[COMPASS_Y_AXIS], compassMag[COMPASS_Z_AXIS]);
    } else {
      flashWriteBufferSize += sprintf((char*) &flashWriteBuffer[flashWriteBufferSize], ",,,");
    }
#ifdef ACCEL_250_ENABLED
    if (accel250DataReceived && compassAccelReceived && compassMagReceived) {
      compassCalculateHeading(compassMag[COMPASS_X_AXIS], compassMag[COMPASS_Y_AXIS], compassMag[COMPASS_Z_AXIS], COMPASS_HARD_IRON_X, COMPASS_HARD_IRON_Y, COMPASS_HARD_IRON_Z, compassAccel[COMPASS_X_AXIS], compassAccel[COMPASS_Y_AXIS], accel250Data, &compassHeading[0]);
      flashWriteBufferSize += sprintf((char*) &flashWriteBuffer[flashWriteBufferSize], "%f,%f,%f,", compassHeading[0], compassHeading[1], compassHeading[2]);
    } else {
      flashWriteBufferSize = sprintf((char*) &flashWriteBuffer[0], ",,,");
    }
#else
    if (compassAccelReceived && compassMagReceived) {
      compassCalculateHeading(compassMag[COMPASS_X_AXIS], compassMag[COMPASS_Y_AXIS], compassMag[COMPASS_Z_AXIS], COMPASS_HARD_IRON_X, COMPASS_HARD_IRON_Y, COMPASS_HARD_IRON_Z, compassAccel[COMPASS_X_AXIS], compassAccel[COMPASS_Y_AXIS], compassAccel[COMPASS_Z_AXIS], &compassHeading[0]);
      flashWriteBufferSize += sprintf((char*) &flashWriteBuffer[flashWriteBufferSize], "%f,%f,%f,", compassHeading[0], compassHeading[1], compassHeading[2]);
    } else {
      flashWriteBufferSize += sprintf((char*) &flashWriteBuffer[flashWriteBufferSize], ",,,");
    }
#endif
#endif

    if (flashWriteBufferSize > 0) { // Is there anything to write?
      flashWriteBuffer[flashWriteBufferSize-1] = '\n'; // Overwrite last comma with a \n
      flashWriteBuffer[flashWriteBufferSize] = '\r';   // Overwrite string termination with a a \r
      flashWriteBuffer[flashWriteBufferSize+1] = '\0'; // Append a new string termination

#ifdef CONSOLE_DEBUG
      //
      // Send write buffer over UART for debugging
      //
      if (consoleIsEnabled()) UARTprintf("%s", flashWriteBuffer);
      delay(CONSOLE_DEBUG_DELAY);
#endif

      //
      // Write data to flash
      //
      if (f_puts((TCHAR*) &flashWriteBuffer[0], &flashFile) == EOF) flashSpaceAvail = false;

      f_sync(&flashFile);

      //
      // Reset write buffer after data is written
      //
      flashWriteBuffer[0]  = 0; // Set first value to null to "erase" string
      flashWriteBufferSize = 0; // Reset the buffer size to 0
    }

#ifdef TRANSCEIVER_ENABLED
#ifdef GPS_ENABLED
    transBuffSize = sprintf((char*) &transBuff[0], "%s%s", gpsGPGGAPtr, "KR0KCT");
#endif
#endif
  } // main while end

  //
  // Out of flash memory. Output flash memory to console
  //
  f_close(&flashFile);
  f_mount(NULL, "", 0);

  while (true) {
    setStatusDefault(OUT_OF_FLASH);
  }
}

//*****************************************************************************
//
// Fault handlers declared in tm4c123gh6pm_startup_ccs.c
//
//*****************************************************************************
void busFaultHandler(void) {
  uint32_t i=0;
  setStatusDefault(BUS_FAULT);
  while (i<3) {
    while (!setStatus(BUS_FAULT)) {};
    i++;
  }
  softReset();
}
void usageFaultHandler(void) {
  uint32_t i=0;
  setStatusDefault(USAGE_FAULT);
  while (i<3) {
    while (!setStatus(USAGE_FAULT)) {};
    i++;
  }
  softReset();
}
//*****************************************************************************
//
// This is the handler for this SysTick interrupt.  FatFs requires a timer tick
// every 10 ms for timing purposes. Declared in tm4c123gh6pm_startup_ccs.c
//
//*****************************************************************************
void
sysTickHandler(void)
{
  //
  // Call the FatFs tick timer every 10ms
  //
  if (_bTick) disk_timerproc();

  //
  // chute deployment control. Called every tick
  //
  bool swPressed = limitSWPressed(LIMIT_SW_DROUGE | LIMIT_SW_MAIN);
  if ((swPressed & LIMIT_SW_DROUGE) && !_bDroguePrimFired && ((*_fGPSMaxAltRecordedPtr - EMATCH_DROGUE_PRIM_FIRE_ALT_DIFF) > *_fGPSCurrAltRecordedPtr)) {
    _bDroguePrimFired = true;
    ematchFire(EMATCH_DROUGE_PRIM);
  } else if ((swPressed & LIMIT_SW_DROUGE) && !_bDrogueBackFired && ((*_fGPSMaxAltRecordedPtr - EMATCH_DROGUE_BACK_FIRE_ALT_DIFF) > *_fGPSCurrAltRecordedPtr))  {
    _bDrogueBackFired = true;
    ematchFire(EMATCH_DROUGE_BACK);
  } else if ((swPressed & LIMIT_SW_MAIN) && !_bMainPrimFired && ((*_fAltMaxAltRecordedPtr - EMATCH_MAIN_PRIM_FIRE_ALT_DIFF) > *_fAltCurrAltRecordedPtr)) {
    _bMainPrimFired = true;
    ematchFire(EMATCH_MAIN_PRIM);
  } else if ((swPressed & LIMIT_SW_MAIN) && !_bMainPrimFired && ((*_fAltMaxAltRecordedPtr - EMATCH_MAIN_BACK_FIRE_ALT_DIFF) > *_fAltCurrAltRecordedPtr)) {
    _bMainBackFired = true;
    ematchFire(EMATCH_MAIN_BACK);
  }

  _bTick = !_bTick;
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
// Soft Reset (No power loss)
void
softReset(void) {
  HWREG(NVIC_APINT) = NVIC_APINT_VECTKEY | NVIC_APINT_SYSRESETREQ;
}
void
waitForSwitchPress(uint8_t ui8Pins) {
  while (!buttonsPressed(ui8Pins)) {};
  while (buttonsPressed(ui8Pins)) {};
}
void
ematchInit(void) {
  gpioOutputInit(EMATCH_BASE, EMATCH_DROUGE_PRIM | EMATCH_DROUGE_BACK | EMATCH_MAIN_PRIM | EMATCH_MAIN_BACK, EMATCH_STRENGTH, EMATCH_PIN_TYPE);
}
void
limitSWInit(void) {
  gpioInputInit(LIMIT_SW_BASE, LIMIT_SW_DROUGE | LIMIT_SW_MAIN, GPIO_PIN_TYPE_STD_WPU);
}
void
ematchFire(uint8_t ui8Pins) {
  MAP_GPIOPinWrite(EMATCH_BASE, ui8Pins, ui8Pins);
}
bool
limitSWPressed(uint8_t ui8Pins) {
  if (!MAP_GPIOPinRead(LIMIT_SW_BASE, ui8Pins)) return true;
  return false;
}
