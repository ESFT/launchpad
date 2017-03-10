//*****************************************************************************
//
// Standard Includes
//
//*****************************************************************************

#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <userlib/spi.h>

#include "inc/hw_gpio.h"
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_nvic.h"
#include "inc/hw_types.h"

#include "driverlib/debug.h"
#include "driverlib/fpu.h"
#include "driverlib/gpio.h"
#include "driverlib/i2c.h"
#include "driverlib/interrupt.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "driverlib/ssi.h"
#include "driverlib/sysctl.h"
#include "driverlib/systick.h"
#include "driverlib/timer.h"
#include "driverlib/uart.h"
#include "driverlib/udma.h"

#include "sensorlib/comp_dcm.h"
#include "sensorlib/i2cm_drv.h"

#include "sensorlib/hw_bmp180.h"
#include "sensorlib/hw_mpu9150.h"
#include "sensorlib/hw_ak8975.h"

#include "sensorlib/ak8975.h"
#include "sensorlib/bmp180.h"
#include "sensorlib/mpu9150.h"

#include "utils/uartstdio.h"

#include "boards_drivers/ek-tm4c123gxl/buttons.h"

#include "userlib/gpio.h"
#include "userlib/i2c.h"
#include "userlib/spim_drv.h"
#include "userlib/sensor_constants.h"
#include "userlib/timer.h"

#include "fatfs/diskio.h"
#include "fatfs/ff.h"
#include "tinygps/tinygps.h"

#include "drivers/accel250.h"
#include "drivers/led.h"
#include "drivers/gps.h"
#include "drivers/misc.h"
#include "drivers/rocketdata.h"

//*****************************************************************************
//
// Peripheral defines
//
//*****************************************************************************
// System setup
#define ACCEL250_ENABLE    false

#define BMP180_ENABLE      true

#define CONSOLE_OUTPUT     false

#define DATA_PRETTY_PRINT  false

#define EMATCH_ENABLED     false

#define GPS_ENABLE         true

#define GPS_ALLOW_BYPASS   false

#define GPS_WAIT_FOR_LOCK  true

#define INIT_WAIT          false

#define MPU9150_ENABLE     true

#define RFM69_ENABLE       false

#define SDCARD_ENABLE      true

#define SENSORHUB_ENABLE   true

#define TEST_MODE          false

// Accel 250

// Backup Deployment Detection GPIO
#define BACKUP_DEPLOYMENT_DETECTOR_BASE GPIO_PORTE_BASE
#define BACKUP_DEPLOYMENT_DROUGE        GPIO_PIN_1
#define BACKUP_DEPLOYMENT_MAIN          GPIO_PIN_2
#define BACKUP_DEPLOYMENT_PIN_TYPE      GPIO_PIN_TYPE_STD_WPD

// BMP180
#if SENSORHUB_ENABLE
#define BMP180_I2C_BASE    I2C3_BASE
#define BMP180_I2C_SPEED   I2C_SPEED_400
#define BMP180_I2C_ADDRESS 0x77
#define BMP180_I2C_INT     INT_I2C3
#else
#define BMP180_I2C_BASE    I2C1_BASE
#define BMP180_I2C_SPEED   I2C_SPEED_400
#define BMP180_I2C_ADDRESS 0x77
#define BMP180_I2C_INT     INT_I2C1
#endif
#define BMP180_DEPLOYMENT_ENABLE_DELAY 5

// Comp DCM
#define COMPASS_DCM_UPDATE_TIMER_BASE TIMER1_BASE
#define COMPASS_DCM_UPDATE_RATE   200 // in hz
#define COMPASS_DCM_ACCEL_WEIGHT 0.2f
#define COMPASS_DCM_GYRO_WEIGHT  0.6f
#define COMPASS_DCM_MAG_WEIGHT   0.2f
#define COMPASS_ROLL   0
#define COMPASS_PITCH  1
#define COMPASS_YAW    2

// eMatch
#define EMATCH_BASE        GPIO_PORTC_BASE
#define EMATCH_DROUGE_PRIM GPIO_PIN_4
#define EMATCH_DROUGE_BACK GPIO_PIN_5
#define EMATCH_MAIN_PRIM   GPIO_PIN_6
#define EMATCH_MAIN_BACK   GPIO_PIN_7
#define EMATCH_DETECT_METHOD_ALTITUDE_DIFF_PRIMARY_DROGUE   30  // In meters below maximum recorded altitude
#define EMATCH_DETECT_METHOD_ALTITUDE_DIFF_BACKUP_DROGUE    50  // In meters below maximum recorded altitude
#define EMATCH_DETECT_METHOD_ALTITUDE_PRIMARY_MAIN         300  // In meters, armed after drogue deployment
#define EMATCH_DETECT_METHOD_ALTITUDE_BACKUP_MAIN          280  // In meters, armed after drogue deployment
#define EMATCH_DETECT_METHOD_TIMER_PRIMARY_DROGUE   22 // The rocket will forcibly deploy the primary drogue after time elapsed (in seconds)
#define EMATCH_DETECT_METHOD_TIMER_BACKUP_DROGUE    24 // The rocket will forcibly deploy the backup drogue after time elapsed (in seconds)
#define EMATCH_DETECT_METHOD_TIMER_PRIMARY_MAIN   9999 // The rocket will forcibly deploy the primary main after time elapsed (in seconds)
#define EMATCH_DETECT_METHOD_TIMER_BACKUP_MAIN    9999 // The rocket will forcibly deploy the backup main after time elapsed (in seconds)

// GPS
#define GPS_BASE                UART1_BASE
#define GPS_NAV_LOCK_SENSE_BASE GPIO_PORTB_BASE
#define GPS_NAV_LOCK_SENSE_PIN  GPIO_PIN_5
#define GPS_BAUD                9600
#define GPS_UART_CONFIG         (UART_CONFIG_WLEN_8 | UART_CONFIG_PAR_NONE | UART_CONFIG_STOP_ONE)

// Launch Conditions
#define LAUNCH_CONDITION_ALT_DIFF      30 // in m
#define LAUNCH_CONDITION_ACCELERATION 150 // in m/s^2

// MPU9150
#if SENSORHUB_ENABLE
#define MPU9150_I2C_BASE    I2C3_BASE
#define MPU9150_I2C_INT     INT_I2C3
#define MPU9150_I2C_SPEED   I2C_SPEED_400
#define MPU9150_I2C_ADDRESS 0x68
#define MPU9150_INT_BASE    GPIO_PORTB_BASE
#define MPU9150_INT_PIN     GPIO_PIN_2
#define MPU9150_X_AXIS 0 // Array index that contains corrected x-axis when mounted (0 = x, 1 = y, z = 2)
#define MPU9150_FLIP_CORRECTED_X_AXIS true
#define MPU9150_Y_AXIS 1 // Array index that contains corrected y-axis when mounted (0 = x, 1 = y, z = 2)
#define MPU9150_FLIP_CORRECTED_Y_AXIS false
#define MPU9150_Z_AXIS 2 // Array index that contains corrected z-axis when mounted (0 = x, 1 = y, z = 2)
#define MPU9150_FLIP_CORRECTED_Z_AXIS false
#else
#define MPU9150_I2C_BASE    I2C0_BASE
#define MPU9150_I2C_INT     INT_I2C0
#define MPU9150_I2C_SPEED   I2C_SPEED_400
#define MPU9150_I2C_ADDRESS 0x68
#define MPU9150_INT_BASE GPIO_PORTB_BASE
#define MPU9150_INT_PIN  GPIO_PIN_4
#define MPU9150_X_AXIS 2 // Array index that contains corrected x-axis when mounted (0 = x, 1 = y, z = 2)
#define MPU9150_FLIP_CORRECTED_X_AXIS false
#define MPU9150_Y_AXIS 1 // Array index that contains corrected y-axis when mounted (0 = x, 1 = y, z = 2)
#define MPU9150_FLIP_CORRECTED_Y_AXIS false
#define MPU9150_Z_AXIS 0 // Array index that contains corrected z-axis when mounted (0 = x, 1 = y, z = 2)
#define MPU9150_FLIP_CORRECTED_Z_AXIS false
#endif

// RFM69HW
#define RFM69HW_SPI_BASE        SSI3_BASE
#define RFM69HW_SPI_INT         INT_SSI3
#define RFM69HW_SPI_SPEED       8000000 // In Hz
#define RFM69HW_SPI_DEVICE_BASE GPIO_PORTD_BASE
#define RFM69HW_SPI_DEVICE_PIN  GPIO_PIN_1
#define RFM69HW_SPI_UDMA_TX_CH  UDMA_CH15_SSI3TX
#define RFM69HW_SPI_UDMA_RX_CH  UDMA_CH14_SSI3RX

// Data Log
#define DATA_WRITE_TIMER_BASE TIMER2_BASE
#define DATA_WRITE_RATE 100 // in hz

// Console

// SD Card
#define SDCARD_FILENAME "h2tbl"
#define SDCARD_EXT      "csv"
#define SDCARD_COMMENT "H2 TBL"
#define SDCARD_TIMERPROC_TIMER_BASE TIMER0_BASE
#define SDCARD_TIMERPROC_RATE 100 // in hz

// System settings

// Systick
#define SYSTICK_RATE 200 // in hz

//*****************************************************************************
//
// Function prototypes
//
//*****************************************************************************
inline void EMatchInit(void);
inline void BackupDeploymentDetectorInit(void);
#if BMP180_ENABLE
void BMP180ConfigAppCallback(void *pvCallbackData, uint_fast8_t ui8Status);
void BMP180DataAppCallback(void *pvCallbackData, uint_fast8_t ui8Status);
void BMP180I2CInit(void);
uint_fast8_t BMP180AppI2CWait(char *pcFilename, uint_fast32_t ui32Line);
#endif
#if MPU9150_ENABLE
void MPU9150ConfigAppCallback(void *pvCallbackData, uint_fast8_t ui8Status);
void MPU9150DataAppCallback(void *pvCallbackData, uint_fast8_t ui8Status);
void MPU9150I2CInit(void);
uint_fast8_t MPU9150AppI2CWait(char *pcFilename, uint_fast32_t ui32Line);
#endif
#if RFM69_ENABLE
void RFM69AppCallback(void *pvCallbackData, uint_fast8_t ui8Status);
void RFM69SPIInit(void);
uint_fast8_t RFM69AppSSIWait(char *pcFilename, uint_fast32_t ui32Line);
#endif
#if SDCARD_ENABLE
uint_fast8_t SDCardInit(void);
#endif

// Int Handlers
void BMP180I2CMIntHandler(void);
void BusFaultHandler(void);
void CompDCMTimerIntHandler(void);
void FatFSTimerIntHandler(void);
void MPU9150I2CMIntHandler(void);
void MPU9150DataRdyIntHandler(void);
void RFM69SPIIntHandler(void);
void SDCardWriteIntHandler(void);
void UsageFaultHandler(void);
void SystickIntHandler(void);

//*****************************************************************************
//
// Global vars
//
//*****************************************************************************
// Global instance structures for data acquisition
FATFS *g_psFlashMount;
FIL *g_psFlashFile;
tBMP180 *g_psBMP180Inst;
tCompDCM *g_psCompDCMInst;
tI2CMInstance *g_psBMP180I2CInst, *g_psMPU9150I2CInst;
tMPU9150 *g_psMPU9150Inst;
tRocketPacket *g_psRocketPacket;
tRocketData *g_psRocketData;
tSPIMInstance *g_psRFM69SPIInst;
volatile bool g_vbWrite = 0;

//
// Global flags to alert main that Sensor transactions are complete, an error has occurred, or data is ready
//
volatile uint_fast8_t g_vui8BMP180DoneFlag, g_vui8BMP180ErrorFlag;
volatile uint_fast8_t g_vui8MPU9150DoneFlag, g_vui8MPU9150ErrorFlag;
volatile uint_fast8_t g_vui8RFM69DoneFlag, g_vui8RFM69ErrorFlag;

//*****************************************************************************
//
// Main function
//
//*****************************************************************************
int32_t main(void) {

  // Initialize global pointers to correct structures
  tCompDCM sCompDCMInst = { 0, };
  g_psCompDCMInst = &sCompDCMInst;
  tMPU9150 sMPU9150Inst = { 0, };
  g_psMPU9150Inst = &sMPU9150Inst;
  tBMP180 sBMP180Inst = { 0, };
  g_psBMP180Inst = &sBMP180Inst;
  tSPIMInstance sRFM69SPIInst = { 0, };
  g_psRFM69SPIInst = &sRFM69SPIInst;
  tRocketPacket sRocketPacket = { 0xFFFFFFFFFFFFFFFF, sizeof(sRocketData), }
  tRocketData sRocketData = { 0, };
  g_psRocketData = &sRocketData;
  FATFS sFlashMount = { 0, };
  g_psFlashMount = &sFlashMount;
  FIL sFlashFile = { 0, };
  g_psFlashFile = &sFlashFile;

  //
  ///////////////////Initialize//////////////////////////////
  //

  //
  // Set the clocking to run at 80MHz.
  //
  MAP_SysCtlClockSet(SYSCTL_SYSDIV_2_5 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);

  //
  // Set Fault Interrupts in the NVIC
  //
  IntRegister(FAULT_BUS, BusFaultHandler);
  IntRegister(FAULT_USAGE, UsageFaultHandler);

  //
  // Enable processor interrupts.
  //
  MAP_IntMasterEnable();

  //
  // Enable the FPU
  //
  FPUInit();

  //
  // Enable LED Control
  //
  LEDInit();
  LEDOn(WHITE_LED);

  //
  // Enable the RTC
  //
  RTCEnable();

  //
  // Configure SysTick for a 200 hz interrupt.
  //
  MAP_SysTickPeriodSet(MAP_SysCtlClockGet() / SYSTICK_RATE);
  SysTickIntRegister(SystickIntHandler);
  MAP_SysTickIntEnable();
  MAP_SysTickEnable();

  //
  // Enable button control
  //
  ButtonsInit();

  //
  // Enable e-match control
  //
  EMatchInit();

  //
  // Enable Backup Deployment Triggered Detection
  //
  BackupDeploymentDetectorInit();

  //
  // Enable the Console IO
  //
  COMPortInit(115200, 0x0, 0x0);

  //
  // Enable the accelerometer
  //
#if ACCEL250_ENABLE
  Accel250Init();
#endif

  //
  // Enable RFM69 Radio
  //
#if RFM69_ENABLE
  RFM69SPIInit();
#endif

  //
  // Enable I2C devices driven by sensor library (MPU9150 and BMP180)
  //
#if (SENSORHUB_ENABLE)
  tI2CMInstance sSensorhubI2CInst = { 0 };
  g_psBMP180I2CInst = &sSensorhubI2CInst;
  g_psMPU9150I2CInst = &sSensorhubI2CInst;
#else
  tI2CMInstance sBMP180I2CInst =
  { 0};
  tI2CMInstance sMPU9150I2CInst =
  { 0};
  g_psBMP180I2CInst = &sBMP180I2CInst;
  g_psMPU9150I2CInst = &sMPU9150I2CInst;
#endif
#if MPU9150_ENABLE
  LEDOn(MAGENTA_LED);
  MPU9150I2CInit();
#endif
#if BMP180_ENABLE
  LEDOn(CYAN_LED);
  BMP180I2CInit();
#endif

//
// Setup Data Logging Timer
//
  TimerFullWidthInit(DATA_WRITE_TIMER_BASE, TIMER_CFG_PERIODIC,
  DATA_WRITE_RATE);
  TimerFullWidthIntInit(DATA_WRITE_TIMER_BASE, -1, TIMER_TIMA_TIMEOUT, SDCardWriteIntHandler);

//
// Setup the SDCard
//
#if SDCARD_ENABLE
  LEDOn(BLUE_LED);
  SDCardInit();
#endif

#if GPS_ENABLE
  //
  // Wait for GPS
  //
  LEDOn(RED_LED);
  GPSInit(GPS_BASE, GPS_BAUD, GPS_UART_CONFIG, &(g_psRocketData->sGPSData),
  GPS_NAV_LOCK_SENSE_BASE, GPS_NAV_LOCK_SENSE_PIN);
#if GPS_WAIT_FOR_LOCK
#if GPS_ALLOW_BYPASS
  while (!GPSAvailable() && !ButtonsPressed(RIGHT_BUTTON));
#else
  while (!GPSAvailable())
    ;
#endif
#endif
#endif

  LEDOn(YELLOW_LED);
#if INIT_WAIT
  //
  // Wait for button press to arm system
  //
  WaitForButtonPress(LEFT_BUTTON);
#endif

#if GPS_ENABLE
  g_psRocketData->fAltitudeInitial = g_psRocketData->fAltitude_Alt;
#else
  g_psRocketData->fAltitudeInitial = g_psRocketData->fAltitude_Alt;
#endif

  g_psRocketData->bLaunchArmed = true;

#if TEST_MODE
  g_psRocketData->ui32LaunchTime = g_psRocketData->ui32ElapsedTime;
#endif

  //
  ///////////////////Gather Data/////////////////////////////
  //
  while (true) {
    if (g_psRocketData->ui32ElapsedTime % 2) {
      LEDOn(GREEN_LED);
    } else {
      LEDOff();
    }
    if (g_vbWrite) {
      g_vbWrite = false;
#if SDCARD_ENABLE
      if (f_puts(pcDataOutput, g_psFlashFile) == EOF) SDCardInit();
      f_sync(g_psFlashFile);
#endif
#if CONSOLE_OUTPUT
      char pcDataOutput[4096];
      sprintf(pcDataOutput,
          "%u,%d,%d/%u/%u|%u:%u:%u.%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%f,%f,%f,%u,%f,%f,%u,%llu,%f,%f,%f,%f,%f,%c,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f\r\n",
          g_psRocketData->bLaunchArmed,
          g_psRocketData->sGPSData.i32Year, g_psRocketData->sGPSData.ui8Month,
          g_psRocketData->sGPSData.ui8Day, g_psRocketData->sGPSData.ui8Hour,
          g_psRocketData->sGPSData.ui8Minute,
          g_psRocketData->sGPSData.ui8Second,
          g_psRocketData->sGPSData.ui8Hundredths,
          g_psRocketData->ui32ElapsedTime, g_psRocketData->ui32LaunchTime,
          g_psRocketData->ui32BackDrogueTime, g_psRocketData->ui32BackMainTime,
          g_psRocketData->ui32SelfDroguePrimTime,
          g_psRocketData->ui32SelfDrogueBackTime,
          g_psRocketData->ui32SelfMainPrimTime,
          g_psRocketData->ui32SelfMainBackTime,
          g_psRocketData->ui8ParachuteDeployment, g_psRocketData->fTemp,
          g_psRocketData->fPres, g_psRocketData->fAltitudeInitial,
          g_psRocketData->bAltimeterArmed, g_psRocketData->fAltitude_Alt,
          g_psRocketData->fAltitudeMax_Alt, g_psRocketData->sGPSData.bNavLock,
          g_psRocketData->sGPSData.ui64FixAge,
          g_psRocketData->sGPSData.fLatitude,
          g_psRocketData->sGPSData.fLongitude,
          g_psRocketData->sGPSData.fAltitude, g_psRocketData->fAltitudeMax_GPS,
          g_psRocketData->sGPSData.fCourse, g_psRocketData->sGPSData.i8Cardinal,
          g_psRocketData->sGPSData.fSpeed, g_psRocketData->fAccel[0],
          g_psRocketData->fAccel[1], g_psRocketData->fAccel[2],
          g_psRocketData->fGyro[0], g_psRocketData->fGyro[1],
          g_psRocketData->fGyro[2], g_psRocketData->fMag[0],
          g_psRocketData->fMag[1], g_psRocketData->fMag[2],
          g_psRocketData->fQuaternion[0], g_psRocketData->fQuaternion[1],
          g_psRocketData->fQuaternion[2], g_psRocketData->fQuaternion[3],
          SENSORS_RADIANS_TO_DEGREES(g_psRocketData->fRoll),
          SENSORS_RADIANS_TO_DEGREES(g_psRocketData->fPitch),
          SENSORS_RADIANS_TO_DEGREES(g_psRocketData->fYaw));
      UARTprintf("%s", pcDataOutput);
#endif
    }
  }
}

inline void BackupDeploymentDetectorInit(void) {
  GPIOInputInit(BACKUP_DEPLOYMENT_DETECTOR_BASE,
  BACKUP_DEPLOYMENT_DROUGE | BACKUP_DEPLOYMENT_MAIN,
  BACKUP_DEPLOYMENT_PIN_TYPE);
}

inline void EMatchInit(void) {
  GPIOOutputInit(
  EMATCH_BASE,
  EMATCH_DROUGE_PRIM | EMATCH_DROUGE_BACK | EMATCH_MAIN_PRIM | EMATCH_MAIN_BACK,
  GPIO_STRENGTH_12MA, GPIO_PIN_TYPE_STD_WPD);
}

//*****************************************************************************
//
// BMP 180 Functions
//
//*****************************************************************************
#if BMP180_ENABLE
void BMP180ConfigAppCallback(void* pvCallbackData, uint_fast8_t ui8Status) {
//*****************************************************************************
//
// BMP180 Sensor callback function.  Called at the end of BMP180 sensor
// driver configuration transactions. This is called from I2C interrupt context.
//
//*****************************************************************************

  if (ui8Status == I2CM_STATUS_SUCCESS) g_vui8BMP180DoneFlag = 1;

  //
  // Store the most recent status in case it was an error condition
  //
  g_vui8BMP180ErrorFlag = ui8Status;
}

void BMP180DataAppCallback(void* pvCallbackData, uint_fast8_t ui8Status) {
//*****************************************************************************
//
// BMP180 Sensor callback function.  Called at the end of BMP180 sensor
// driver data transactions. This is called from I2C interrupt context.
//
//*****************************************************************************
  if (ui8Status == I2CM_STATUS_SUCCESS) {
    g_vui8BMP180DoneFlag = 1;
    BMP180DataTemperatureGetFloat(g_psBMP180Inst, &(g_psRocketData->fTemp));
    BMP180DataPressureGetFloat(g_psBMP180Inst, &(g_psRocketData->fPres));
    g_psRocketData->fAltitude_Alt = SENSORS_PA_TO_PRESSURE_ALTITUDE(g_psRocketData->fPres);

    // Record the maximum Alt Altitude achieved
    if (g_psRocketData->bAltimeterArmed && (g_psRocketData->fAltitude_Alt > g_psRocketData->fAltitudeMax_Alt))
      g_psRocketData->fAltitudeMax_Alt = g_psRocketData->fAltitude_Alt;
  }

  //
  // Store the most recent status in case it was an error condition
  //
  g_vui8BMP180ErrorFlag = ui8Status;

  BMP180DataRead(g_psBMP180Inst, BMP180DataAppCallback, g_psBMP180Inst);
}

uint_fast8_t BMP180AppI2CWait(char *pcFilename, uint_fast32_t ui32Line) {
  //
  // Put the processor to sleep while we wait for the I2C driver to
  // indicate that the transaction is complete.
  //
  while (!g_vui8BMP180DoneFlag && !g_vui8BMP180ErrorFlag)
    ;

  //
  // clear the data flag for next use.
  //
  g_vui8BMP180DoneFlag = 0;

  return g_vui8BMP180ErrorFlag;
}

inline void BMP180I2CMFaultHandler(void) {
  //
  // Clear bus
  //
  I2CClearBus(BMP180_I2C_BASE);
}

void BMP180I2CInit(void) {
  if (!(g_psBMP180I2CInst->ui32Base)) {
    //
    // Enable I2C & associated GPIO ports
    //
    while (!(I2CInit(BMP180_I2C_BASE, BMP180_I2C_SPEED) == I2C_BUS_OK))
      ;
    I2CIntRegister(BMP180_I2C_BASE, BMP180I2CMIntHandler);

    //
    // Initialize I2C Peripheral
    //
    I2CMInit(g_psBMP180I2CInst, BMP180_I2C_BASE, BMP180_I2C_INT, 0xff, 0xff,
    MAP_SysCtlClockGet());
  }

  do {
    //
    // Initialize the MPU9150 Driver.
    //
    BMP180Init(g_psBMP180Inst, g_psBMP180I2CInst, BMP180_I2C_ADDRESS, BMP180ConfigAppCallback, g_psBMP180Inst);

    //
    // Wait for transaction to complete
    // If transaction fails, restart configuration process
    //

    if (BMP180AppI2CWait(__FILE__, __LINE__)) continue;
  } while (g_vui8BMP180ErrorFlag); // If we made it this far and the error flag is 0, configuration is good.

  BMP180DataRead(g_psBMP180Inst, BMP180DataAppCallback, g_psBMP180Inst);
}
#endif

//*****************************************************************************
//
// MPU 9150 Functions
//
//*****************************************************************************
#if MPU9150_ENABLE
void MPU9150ConfigAppCallback(void *pvCallbackData, uint_fast8_t ui8Status) {
//*****************************************************************************
//
// MPU9150 Sensor callback function.  Called at the end of MPU9150 sensor
// driver config transactions. This is called from I2C interrupt context.
//
//*****************************************************************************

  if (ui8Status == I2CM_STATUS_SUCCESS) g_vui8MPU9150DoneFlag = 1;

  //
  // Store the most recent status in case it was an error condition
  //
  g_vui8MPU9150ErrorFlag = ui8Status;
}

void MPU9150DataAppCallback(void *pvCallbackData, uint_fast8_t ui8Status) {
//*****************************************************************************
//
// MPU9150 Sensor callback function.  Called at the end of MPU9150 sensor
// driver data transactions. This is called from I2C interrupt context.
//
//*****************************************************************************
  if (ui8Status == I2CM_STATUS_SUCCESS) {
    g_vui8MPU9150DoneFlag = 1;
    MPU9150DataAccelGetFloat(g_psMPU9150Inst, &(g_psRocketData->fAccel[MPU9150_X_AXIS]), &(g_psRocketData->fAccel[MPU9150_Y_AXIS]),
        &(g_psRocketData->fAccel[MPU9150_Z_AXIS]));
    MPU9150DataGyroGetFloat(g_psMPU9150Inst, &(g_psRocketData->fGyro[MPU9150_X_AXIS]), &(g_psRocketData->fGyro[MPU9150_Y_AXIS]),
        &(g_psRocketData->fGyro[MPU9150_Z_AXIS]));
    MPU9150DataMagnetoGetFloat(g_psMPU9150Inst, &(g_psRocketData->fMag[MPU9150_X_AXIS]), &(g_psRocketData->fMag[MPU9150_Y_AXIS]),
        &(g_psRocketData->fMag[MPU9150_Z_AXIS]));

    //
    // Convert Gs to m/s
    //
    g_psRocketData->fAccel[MPU9150_X_AXIS] = SENSORS_EARTH_G_TO_MS2(g_psRocketData->fAccel[MPU9150_X_AXIS]);
    g_psRocketData->fAccel[MPU9150_Y_AXIS] = SENSORS_EARTH_G_TO_MS2(g_psRocketData->fAccel[MPU9150_Y_AXIS]);
    g_psRocketData->fAccel[MPU9150_Z_AXIS] = SENSORS_EARTH_G_TO_MS2(g_psRocketData->fAccel[MPU9150_Z_AXIS]);

#if ACCEL250_ENABLE
    // If our X axis accelerometer is saturated (powered flight or chute deployment) swap to the 250G accelerometer.
    if (g_psRocketData->fAccel[MPU9150_X_AXIS] > 117.72) {
      Accel250DataGetFloat(&(g_psRocketData->fAccel[MPU9150_X_AXIS]));
      g_psRocketData->fAccel[MPU9150_X_AXIS] = SENSORS_EARTH_G_TO_MS2(g_psRocketData->fAccel[MPU9150_X_AXIS]);
    }
#endif

#if MPU9150_FLIP_CORRECTED_X_AXIS
    g_psRocketData->fAccel[0] *= -1;
    g_psRocketData->fGyro[0] *= -1;
    g_psRocketData->fMag[0] *= -1;
#endif
#if MPU9150_FLIP_CORRECTED_Y_AXIS
    g_psRocketData->fAccel[1] *= -1;
    g_psRocketData->fGyro[1] *= -1;
    g_psRocketData->fMag[1] *= -1;
#endif
#if MPU9150_FLIP_CORRECTED_Z_AXIS
    g_psRocketData->fAccel[2] *= -1;
    g_psRocketData->fGyro[2] *= -1;
    g_psRocketData->fMag[2] *= -1;
#endif
  }

  //
  // Store the most recent status in case it was an error condition
  //
  g_vui8MPU9150ErrorFlag = ui8Status;
}

uint_fast8_t MPU9150AppI2CWait(char *pcFilename, uint_fast32_t ui32Line) {
  //
  // Put the processor to sleep while we wait for the I2C driver to
  // indicate that the transaction is complete.
  //
  while (!g_vui8MPU9150DoneFlag && !g_vui8MPU9150ErrorFlag)
    ;

  //
  // clear the data flag for next use.
  //
  g_vui8MPU9150DoneFlag = 0;

  return g_vui8MPU9150ErrorFlag;
}

inline void MPU9150I2CMFaultHandler(void) {
  //
  // Clear bus
  //
  I2CClearBus(MPU9150_I2C_BASE);
}

void MPU9150I2CInit(void) {
  if (!(g_psMPU9150I2CInst->ui32Base)) {
    //
    // Enable I2C & associated GPIO ports
    //
    while (!(I2CInit(MPU9150_I2C_BASE, MPU9150_I2C_SPEED) == I2C_BUS_OK))
      ;
    I2CIntRegister(MPU9150_I2C_BASE, MPU9150I2CMIntHandler);

    //
    // Initialize I2C Peripheral
    //
    I2CMInit(g_psMPU9150I2CInst, MPU9150_I2C_BASE, MPU9150_I2C_INT, 0xff, 0xff,
    MAP_SysCtlClockGet());
  }

  GPIOInputInit(MPU9150_INT_BASE, MPU9150_INT_PIN, GPIO_PIN_TYPE_STD);
  GPIOIntInit(MPU9150_INT_BASE, MPU9150_INT_PIN, GPIO_FALLING_EDGE, -1, MPU9150DataRdyIntHandler);

  do {
    //
    // Initialize the MPU9150 Driver.
    //
    MPU9150Init(g_psMPU9150Inst, g_psMPU9150I2CInst, MPU9150_I2C_ADDRESS, MPU9150ConfigAppCallback, g_psMPU9150Inst);

    //
    // Wait for transaction to complete
    // If transaction fails, restart configuration process
    //
    if (MPU9150AppI2CWait(__FILE__, __LINE__)) continue;

    //
    // Write application specific sensor configuration such as filter settings
    // and sensor range settings.
    //
    g_psMPU9150Inst->pui8Data[0] = MPU9150_CONFIG_DLPF_CFG_260_256;
    g_psMPU9150Inst->pui8Data[1] = MPU9150_GYRO_CONFIG_FS_SEL_2000;
    g_psMPU9150Inst->pui8Data[2] = (MPU9150_ACCEL_CONFIG_ACCEL_HPF_5HZ | MPU9150_ACCEL_CONFIG_AFS_SEL_16G);
    MPU9150Write(g_psMPU9150Inst, MPU9150_O_CONFIG, g_psMPU9150Inst->pui8Data, 3, MPU9150ConfigAppCallback, g_psMPU9150Inst);

    //
    // Wait for transaction to complete
    // If transaction fails, restart configuration process
    //
    if (MPU9150AppI2CWait(__FILE__, __LINE__)) continue;

    //
    // Configure the data ready interrupt pin output of the MPU9150.
    //
    g_psMPU9150Inst->pui8Data[0] = MPU9150_INT_PIN_CFG_INT_LEVEL | MPU9150_INT_PIN_CFG_INT_RD_CLEAR | MPU9150_INT_PIN_CFG_LATCH_INT_EN;
    g_psMPU9150Inst->pui8Data[1] = MPU9150_INT_ENABLE_DATA_RDY_EN;
    MPU9150Write(g_psMPU9150Inst, MPU9150_O_INT_PIN_CFG, g_psMPU9150Inst->pui8Data, 2, MPU9150ConfigAppCallback, g_psMPU9150Inst);

    //
    // Wait for transaction to complete
    //
    if (MPU9150AppI2CWait(__FILE__, __LINE__)) continue;
  } while (g_vui8MPU9150ErrorFlag); // If we made it this far and the error flag is 0, configuration is good.

  //
  // Initialize the DCM system. 50 hz sample rate.
  //
  CompDCMInit(g_psCompDCMInst, 1.0f / COMPASS_DCM_UPDATE_RATE,
  COMPASS_DCM_ACCEL_WEIGHT,
  COMPASS_DCM_GYRO_WEIGHT,
  COMPASS_DCM_MAG_WEIGHT);
  TimerFullWidthInit(COMPASS_DCM_UPDATE_TIMER_BASE, TIMER_CFG_PERIODIC,
  COMPASS_DCM_UPDATE_RATE);
  TimerFullWidthIntInit(COMPASS_DCM_UPDATE_TIMER_BASE, -1, TIMER_TIMA_TIMEOUT, CompDCMTimerIntHandler);
}
#endif

//*****************************************************************************
//
// RFM69 Functions
//
//*****************************************************************************
#if RFM69_ENABLE
void RFM69AppCallback(void *pvCallbackData, uint_fast8_t ui8Status) {
  if (ui8Status == SPIM_STATUS_SUCCESS) {
    g_vui8RFM69DoneFlag = 1;
  }

  //
  // Store the most recent status in case it was an error condition
  //
  g_vui8RFM69ErrorFlag = ui8Status;
}

uint_fast8_t RFM69AppSPIWait(char *pcFilename, uint_fast32_t ui32Line) {
  //
  // Put the processor to sleep while we wait for the SSI driver to
  // indicate that the transaction is complete.
  //
  while ((g_vui8RFM69DoneFlag == 0) && (g_vui8RFM69ErrorFlag == 0))
  ;

  //
  // clear the data flag for next use.
  //
  g_vui8RFM69DoneFlag = 0;

  return g_vui8RFM69ErrorFlag;
}

void RFM69SPIInit(void) {
  SSIInit(RFM69HW_SPI_BASE, SSI_FRF_MOTO_MODE_0, SSI_MODE_MASTER,
      RFM69HW_SPI_SPEED, 8);
  GPIOOutputInit(RFM69HW_SPI_DEVICE_BASE, RFM69HW_SPI_DEVICE_PIN,
      GPIO_STRENGTH_4MA, GPIO_PIN_TYPE_STD_WPU);
  SSIIntRegister(RFM69HW_SPI_BASE, RFM69SPIIntHandler);

  //
  // Initialize the SPI driver
  //
  SPIMInit(g_psRFM69SPIInst, RFM69HW_SPI_BASE, RFM69HW_SPI_INT,
      RFM69HW_SPI_UDMA_TX_CH, RFM69HW_SPI_UDMA_RX_CH,
      MAP_SysCtlClockGet(), RFM69HW_SPI_SPEED);

  //
  // Initialize the RFM69 Driver.
  //
  uint8_t write[10] = {0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9};
  uint8_t read[10] = {0x1};
  SPIMTransfer(g_psRFM69SPIInst, RFM69HW_SPI_DEVICE_BASE,
      RFM69HW_SPI_DEVICE_PIN, write, read, 10, RFM69AppCallback, g_psRFM69SPIInst);
  RFM69AppSPIWait(__FILE__, __LINE__);

  uint8_t write2[10] = {0x9, 0x9, 0x7, 0x6, 0x5, 0x4, 0x3, 0x2, 0x1, 0x0};
  uint8_t read2[10] = {0x1};
  SPIMTransfer(g_psRFM69SPIInst, RFM69HW_SPI_DEVICE_BASE,
      RFM69HW_SPI_DEVICE_PIN, write2, read2, 10, RFM69AppCallback, g_psRFM69SPIInst);
  RFM69AppSPIWait(__FILE__, __LINE__);

  // TODO: MAKE DRIVER
}
#endif

//*****************************************************************************
//
// SD Card Functions
//
//*****************************************************************************
#if SDCARD_ENABLE
uint_fast8_t SDCardInit() {
  char pcFilename[250];
  uint_fast8_t vui8Index = 0;
  TimerFullWidthInit(SDCARD_TIMERPROC_TIMER_BASE, TIMER_CFG_PERIODIC,
  SDCARD_TIMERPROC_RATE);
  TimerFullWidthIntInit(SDCARD_TIMERPROC_TIMER_BASE, -1, TIMER_TIMA_TIMEOUT, FatFSTimerIntHandler);

  while (f_mount(g_psFlashMount, "", 1) != FR_OK)
    ;
  sprintf(pcFilename, "%s.%s", SDCARD_FILENAME, SDCARD_EXT);
  for (vui8Index = 1; f_stat(pcFilename, NULL) != FR_NO_FILE && vui8Index < 9999; vui8Index++) {
    sprintf(pcFilename, "%s%d.%s", SDCARD_FILENAME, vui8Index, SDCARD_EXT);
  }
  f_unlink(pcFilename);
  while (f_open(g_psFlashFile, pcFilename, FA_WRITE | FA_CREATE_ALWAYS) != FR_OK)
    ;

  if (f_puts("ESFT\n"SDCARD_COMMENT"\n", g_psFlashFile) == EOF) {
    //
    // Out of flash memory
    //
    f_close(g_psFlashFile);
    f_mount(NULL, "", 0);
    return (0);
  } else {
    f_sync(g_psFlashFile);
    return (1);
  }
}
#endif

//*****************************************************************************
//
// Int handlers
//
//*****************************************************************************
void BMP180I2CMIntHandler(void) {
  //
  // Pass through to the I2CM interrupt handler provided by sensor library.
  // This is required to be at application level so that I2CMIntHandler can
  // receive the instance structure pointer as an argument.
  //
  I2CMIntHandler(g_psBMP180I2CInst);
}

void BusFaultHandler(void) {
  LEDOn(RED_LED);
  while (true)
    ;
}

void CompDCMTimerIntHandler(void) {
  static bool bCompDCMStarted = false;

  MAP_TimerIntClear(COMPASS_DCM_UPDATE_TIMER_BASE, TIMER_TIMA_TIMEOUT); // Clear the timer interrupt

  CompDCMAccelUpdate(g_psCompDCMInst, g_psRocketData->fAccel[0], g_psRocketData->fAccel[1], g_psRocketData->fAccel[2]);
  CompDCMGyroUpdate(g_psCompDCMInst, g_psRocketData->fGyro[0], g_psRocketData->fGyro[1], g_psRocketData->fGyro[2]);
  CompDCMMagnetoUpdate(g_psCompDCMInst, g_psRocketData->fMag[0], g_psRocketData->fMag[1], g_psRocketData->fMag[2]);

  // Check if this is our first data ever.
  //
  if (bCompDCMStarted) {
    //
    // DCM Is already started.  Perform the incremental update.
    //
    CompDCMUpdate(g_psCompDCMInst);
  } else {
    //
    // Set flag indicating that DCM is started.
    // Perform the seeding of the DCM with the first data set.
    //
    bCompDCMStarted = true;
    CompDCMStart(g_psCompDCMInst);
  }

  CompDCMComputeEulers(g_psCompDCMInst, &g_psRocketData->fRoll, &g_psRocketData->fPitch, &g_psRocketData->fYaw);
  CompDCMComputeQuaternion(g_psCompDCMInst, g_psRocketData->fQuaternion);
}

void FatFSTimerIntHandler(void) {
  MAP_TimerIntClear(SDCARD_TIMERPROC_TIMER_BASE, TIMER_TIMA_TIMEOUT); // Clear the timer interrupt
  disk_timerproc();
}

void MPU9150I2CMIntHandler(void) {
  //
  // Pass through to the I2CM interrupt handler provided by sensor library.
  // This is required to be at application level so that I2CMIntHandler can
  // receive the instance structure pointer as an argument.
  //
  I2CMIntHandler(g_psMPU9150I2CInst);
}

void MPU9150DataRdyIntHandler(void) {
  unsigned long ulStatus;

  ulStatus = GPIOIntStatus(MPU9150_INT_BASE, true);

  //
  // Clear all the pin interrupts that are set
  //
  GPIOIntClear(MPU9150_INT_BASE, ulStatus);

  //
  // MPU9150 Data is ready for retrieval and processing.
  //
  if (ulStatus & MPU9150_INT_PIN) MPU9150DataRead(g_psMPU9150Inst, MPU9150DataAppCallback, g_psMPU9150Inst);
}

void RFM69SPIIntHandler(void) {
  SPIMIntHandler(g_psRFM69SPIInst);
}

void SDCardWriteIntHandler(void) {
  MAP_TimerIntClear(DATA_WRITE_TIMER_BASE, TIMER_TIMA_TIMEOUT); // Clear the timer interrupt
  g_vbWrite = true;
}

void UsageFaultHandler(void) {
  LEDOn(RED_LED);
  while (true)
    ;
}

void SystickIntHandler(void) {
  //*****************************************************************************
  //
  // This is the handler for this SysTick interrupt.
  //
  // The GPS Altimeter data is updated every systick
  //
  // Recovery system called every systick
  //
  //*****************************************************************************
  g_psRocketData->ui32ElapsedTime = MAP_HibernateRTCGet();

  // If launch has been armed, we haven't set a launch time, and either we have reached launch acceleration or our altitude has significantly changed, set our launch time
  if (g_psRocketData->bLaunchArmed && !g_psRocketData->ui32LaunchTime
      && ((g_psRocketData->fAccel[MPU9150_X_AXIS] > LAUNCH_CONDITION_ACCELERATION)
          || (GPSAvailable() && g_psRocketData->sGPSData.fAltitude > (g_psRocketData->fAltitudeInitial + LAUNCH_CONDITION_ALT_DIFF)))) {
    g_psRocketData->ui32LaunchTime = g_psRocketData->ui32ElapsedTime;
  }

  //
  // Chute deployment control
  //

  // if we have launched (Launch time is recorded), start recovery system checks
  if (g_psRocketData->ui32LaunchTime) {

    // Read backup system's fire state
    uint_fast8_t ui8DetectorState = MAP_GPIOPinRead(
    BACKUP_DEPLOYMENT_DETECTOR_BASE,
    BACKUP_DEPLOYMENT_DROUGE | BACKUP_DEPLOYMENT_MAIN);

    // check if the backup drogue has fired
    if (!(g_psRocketData->sParachuteDeployment.bBackupDrogueFired) && (ui8DetectorState & BACKUP_DEPLOYMENT_DROUGE)) {
      g_psRocketData->sParachuteDeployment.bBackupDrogueFired = true;
      g_psRocketData->ui32BackDrogueTime = g_psRocketData->ui32ElapsedTime;
    }

    // check if the backup main has fired
    if (!(g_psRocketData->sParachuteDeployment.bBackupMainFired) && (ui8DetectorState & BACKUP_DEPLOYMENT_MAIN)) {
      g_psRocketData->sParachuteDeployment.bBackupMainFired = true;
      g_psRocketData->ui32BackMainTime = g_psRocketData->ui32ElapsedTime;
    }

    // Record the maximum GPS Altitude achieved
    if (GPSAvailable() && (g_psRocketData->sGPSData.fAltitude != TINYGPS_INVALID_F_ALTITUDE)
        && (g_psRocketData->sGPSData.fAltitude > g_psRocketData->fAltitudeMax_GPS)) {
      g_psRocketData->fAltitudeMax_GPS = g_psRocketData->sGPSData.fAltitude;
    }

    // Only record maximum altimeter altitude when we have launched and the altimeter is active (after drogue deploy) due to pressure issues inside the rocket during powered flight
    if (g_psRocketData->bAltimeterArmed && (g_psRocketData->fAltitudeMax_Alt < g_psRocketData->fAltitude_Alt)) {
      g_psRocketData->fAltitudeMax_Alt = g_psRocketData->fAltitude_Alt;
    }

    // If we haven't fired off D_prim, check that subsystem
    // Thus,
    // If we haven't deployed D_prim, we have reached apogee, and we fall back below the D_prim alt diff threshold, fire off D_prim
    // - OR -
    // If we are past due time for firing D_prim, fire off D_prim
    // - OR -
    // If the backup system has fired, fire off D_prim
    if (!(g_psRocketData->sParachuteDeployment.bSelfPrimaryDrogueFired)
        && ((g_psRocketData->fAltitude_Alt < (g_psRocketData->fAltitudeMax_GPS - EMATCH_DETECT_METHOD_ALTITUDE_DIFF_PRIMARY_DROGUE))
            || (EMATCH_DETECT_METHOD_TIMER_PRIMARY_DROGUE < (g_psRocketData->ui32ElapsedTime - g_psRocketData->ui32LaunchTime))
            || (g_psRocketData->sParachuteDeployment.bBackupDrogueFired))) {
#if EMATCH_ENABLED
      MAP_GPIOPinWrite(EMATCH_BASE, EMATCH_DROUGE_PRIM, EMATCH_DROUGE_PRIM);
#endif
      g_psRocketData->sParachuteDeployment.bSelfPrimaryDrogueFired = true;
      g_psRocketData->ui32SelfDroguePrimTime = g_psRocketData->ui32ElapsedTime;
      LEDOn(WHITE_LED);
    }

    // If we haven't fired off D_back, check that subsystem
    // Thus,
    // If we haven't deployed D_back, we have reached apogee, and we pass the D_back alt diff threshold, fire off D_back.
    // - OR -
    // If we are past due time for firing D_back, fire off D_back
    // - OR -
    // If the backup system has fired, fire off D_back.
    // Also, enable the altimeter.
    if (!(g_psRocketData->sParachuteDeployment.bSelfBackupDrogueFired)
        && ((g_psRocketData->fAltitude_Alt < (g_psRocketData->fAltitudeMax_GPS - EMATCH_DETECT_METHOD_ALTITUDE_DIFF_BACKUP_DROGUE))
            || (EMATCH_DETECT_METHOD_TIMER_BACKUP_DROGUE < (g_psRocketData->ui32ElapsedTime - g_psRocketData->ui32LaunchTime))
            || (g_psRocketData->sParachuteDeployment.bBackupDrogueFired))) {
#if EMATCH_ENABLED
      MAP_GPIOPinWrite(EMATCH_BASE, EMATCH_DROUGE_BACK, EMATCH_DROUGE_BACK);
#endif
      g_psRocketData->sParachuteDeployment.bSelfBackupDrogueFired = true;
      g_psRocketData->ui32SelfDrogueBackTime = g_psRocketData->ui32ElapsedTime;
      LEDOn(WHITE_LED);
    }

    // If our drogues are deployed, check the main chute subsystems
    if ((g_psRocketData->sParachuteDeployment.bSelfBackupDrogueFired)
        && (g_psRocketData->ui32ElapsedTime > g_psRocketData->ui32SelfDrogueBackTime + BMP180_DEPLOYMENT_ENABLE_DELAY)) {

      // "Activate" our altimeter
      g_psRocketData->bAltimeterArmed = true;

      // If we haven't fired M_prim, check that subsystem
      // Thus,
      // If we have gone below M_prim fire altitude, fire off M_prim
      // - OR -
      // If we are past due time for firing M_prim, fire off M_prim
      // - OR -
      // If the backup system has fired, fire off M_prim
      if (!(g_psRocketData->sParachuteDeployment.bSelfPrimaryMainFired)
          && ((g_psRocketData->fAltitude_Alt < (g_psRocketData->fAltitudeInitial + EMATCH_DETECT_METHOD_ALTITUDE_PRIMARY_MAIN))
              || (EMATCH_DETECT_METHOD_TIMER_PRIMARY_MAIN < (g_psRocketData->ui32ElapsedTime - g_psRocketData->ui32LaunchTime))
              || (g_psRocketData->sParachuteDeployment.bBackupMainFired))) {
#if EMATCH_ENABLED
        MAP_GPIOPinWrite(EMATCH_BASE, EMATCH_MAIN_PRIM, EMATCH_MAIN_PRIM);
#endif
        g_psRocketData->sParachuteDeployment.bSelfPrimaryMainFired = true;
        g_psRocketData->ui32SelfMainPrimTime = g_psRocketData->ui32ElapsedTime;
        LEDOn(WHITE_LED);
      }

      // If we haven't fired M_back, check that subsystem
      // Thus,
      // If we have gone below M_back fire altitude, fire off M_back
      // - OR -
      // If we are past due time for firing M_back, fire off M_back
      // - OR -
      // If the backup system has fired, fire off M_back
      if (!(g_psRocketData->sParachuteDeployment.bSelfBackupMainFired)
          && ((g_psRocketData->fAltitude_Alt < (g_psRocketData->fAltitudeInitial + EMATCH_DETECT_METHOD_ALTITUDE_BACKUP_MAIN))
              || (EMATCH_DETECT_METHOD_TIMER_BACKUP_MAIN < (g_psRocketData->ui32ElapsedTime - g_psRocketData->ui32LaunchTime))
              || (g_psRocketData->sParachuteDeployment.bBackupMainFired))) {
#if EMATCH_ENABLED
        MAP_GPIOPinWrite(EMATCH_BASE, EMATCH_MAIN_BACK, EMATCH_MAIN_BACK);
#endif
        g_psRocketData->sParachuteDeployment.bSelfBackupMainFired =
        true;
        g_psRocketData->ui32SelfMainBackTime = g_psRocketData->ui32ElapsedTime;
        LEDOn(WHITE_LED);
      }
    }
  }
}
