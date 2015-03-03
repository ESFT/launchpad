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

#include "peripherals/altimeter.h"
#include "peripherals/accel250.h"
#include "peripherals/fatfs/diskio.h"
#include "peripherals/fatfs/ff.h"
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
//#define ACCEL_250_ENABLED

// I2C Speed
#define I2C_SPEED   I2C_SPEED_400

// Altimeter
#define ALT_ENABLED
#define ALT_BASE    I2C0_BASE
#define ALT_ADDRESS ALT_ADDRESS_CSB_LO

// Flash
// #define DEBUG_FLASH
#define DEBUG_FLASH_DELAY 500 // Output delay in ms

// GPS
// #define GPS_ENABLED
#define GPS_BASE UART4_BASE
#define GPS_NAV_LOCK_SENSE_BASE GPIO_PORTC_BASE
#define GPS_NAV_LOCK_SENSE_PIN  GPIO_PIN_6

// Gyro
#define GYRO_ENABLED
#define GYRO_BASE    I2C0_BASE
#define GYRO_ADDRESS GYRO_ADDRESS_SDO_LO
#define GYRO_DRDY_SENSE_BASE GPIO_PORTB_BASE
#define GYRO_DRDY_SENSE_PIN  GPIO_PIN_0

// Magnetometer, Accel
#define MAG_ENABLED
#define MAG_BASE    I2C0_BASE
#define MAG_ADDRESS ""
#define MAG_DRDY_SENSE_BASE GPIO_PORTB_BASE
#define MAG_DRDY_SENSE_PIN  GPIO_PIN_1

// Status codes
#define STATUS_CODES_ENABLED
#define STATUS_CODE_TIMER_BASE TIMER0_BASE

// ematch GPIO
#define EMATCH_BASE        GPIO_PORTD_BASE
#define EMATCH_DROUGE_PRIM GPIO_PIN_0
#define EMATCH_DROUGE_BACK GPIO_PIN_1
#define EMATCH_MAIN_PRIM   GPIO_PIN_2
#define EMATCH_MAIN_BACK   GPIO_PIN_3
#define EMATCH_STRENGTH    GPIO_STRENGTH_12MA    // 12 mA output
#define EMATCH_PIN_TYPE    GPIO_PIN_TYPE_STD_WPD // Weak pulldown

// Limit SW GPIO
#define LIMIT_SW_BASE     GPIO_PORTE_BASE
#define LIMIT_SW_DROUGE   GPIO_PIN_1
#define LIMIT_SW_MAIN     GPIO_PIN_2
#define LIMIT_SW_PIN_TYPE GPIO_PIN_TYPE_STD_WPU // Weak pullup

// Prototypes
void hardReset(void);
void waitForSwitchPress(uint8_t ui8Pins);
void ematchInit();
void limitInit();
void ematchFire(uint8_t ui8Pin);
void limitSWDetect(uint8_t ui8Pin);

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
  statusCodeInterruptInit(STATUS_CODE_TIMER_BASE);
#endif

  //
  // Enable the FPU
  //
  FPUInit();

  //
  // Enable the Console IO
  //
  consoleInit();

  //
  // Enable button control
  //
  buttonsInit();

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
    status = altInit(ALT_BASE, ALT_ADDRESS, I2C_SPEED);
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
    status = gyroInit(GYRO_BASE, GYRO_ADDRESS, I2C_SPEED, GYRO_DRDY_SENSE_BASE, GYRO_DRDY_SENSE_PIN);
    setStatus(status);
  } while (status != INITIALIZING);
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

  //
  // Configure SysTick for a 100Hz interrupt.  The FatFs driver wants a 10 ms
  // tick.
  //
  MAP_SysTickPeriodSet(ROM_SysCtlClockGet() / 100);
  MAP_SysTickEnable();
  MAP_SysTickIntEnable();

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

  setStatusDefault(INITIALIZING_HOLD);
  waitForSwitchPress(SW1_BUTTON);

  if (flashMount.free_clust < 1)
    flashSpaceAvail = false;

  //
  ///////////////////Gather Data/////////////////////////////
  //

  setStatusDefault(RUNNING);
  while(flashSpaceAvail && !(buttonsPressed(SW2_BUTTON) & SW2_BUTTON)) {
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
    //
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
      if (f_puts((TCHAR*) &flashWriteBuffer[0], &flashFile) == EOF)
        flashSpaceAvail = false;

      f_sync(&flashFile);

      //
      // Reset write buffer after data is written
      //
      flashWriteBuffer[0] = '\0'; // Set first value to null to "erase" string
      flashWriteBufferSize = 0; // Reset the buffer size to 0
    }
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
    // Call the FatFs tick timer.
    //
    disk_timerproc();
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
void
waitForSwitchPress(uint8_t ui8Pins) {
  while (!(buttonsPressed(ui8Pins) & SW1_BUTTON)) {};
  while (buttonsPressed(ui8Pins) & SW1_BUTTON) {};
}
