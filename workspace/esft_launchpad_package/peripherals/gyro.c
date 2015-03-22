/*
 * gyro.c
 *
 *  Created on: Feb 17, 2015
 *      Author: Ryan
 */

#include <math.h>

#include "driverlib/gpio.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"

#include "gpio.h"
#include "gyro.h"
#include "i2c.h"
#include "misc.h"
#include "sensor_constants.h"
#include "status.h"

static uint32_t gyro_ui32Base;
static uint8_t gyro_ui8GyroAddr;
static uint32_t gyro_ui32SenseBase;
static uint32_t gyro_ui32IntFlags;

static float gyro_fSensitivity = 0;
static float gyro_fZeroRate[3] = { 0, 0, 0 };
static float gyro_fThreshold[3] = { 0, 0, 0 };
static int16_t gyro_i16Raw[3] = { 0, 0, 0 };
static bool gyro_dataAvailable;

float* gyro_fDPS;

void
gyroIntHandler() {
  MAP_GPIOIntClear(gyro_ui32SenseBase, gyro_ui32IntFlags);
  if (gyroReadXYZ()) {
    gyro_dataAvailable = true;
  }
}
void
gyroCalibrate(uint32_t ui32SampleCount, uint32_t ui32SigmaMultiple) {
  float fSums[3] = { 0, 0, 0 }, fSigma[3] = { 0, 0, 0 };
  uint32_t i, j;
  for (i = 0; i < ui32SampleCount; i++) {
    while (!gyroReceive()) {
    }
    for (j = 0; j < 3; j++) {
      fSums[j] += gyro_i16Raw[j];
      fSigma[j] += pow(gyro_i16Raw[j], 2);
    }
  }
  for (i = 0; i < 3; i++) {
    gyro_fZeroRate[i] = fSums[i] / (float) ui32SampleCount;
    gyro_fThreshold[i] = sqrt((fSigma[i] / (float) ui32SampleCount) - pow(gyro_fZeroRate[i], 2)) * (float) ui32SigmaMultiple;
  }
}
StatusCode_t
gyroInit(uint32_t ui32Base, uint8_t ui8GyroAddr, bool bSpeed, uint32_t ui32SenseBase, uint8_t ui8SensePin, float* fDPS) {
  gyro_ui32Base = ui32Base;
  gyro_ui8GyroAddr = ui8GyroAddr;
  gyro_ui32SenseBase = ui32SenseBase;
  gyro_fDPS = fDPS;

  //
  // Enable the I2C module used by the gyro
  //
  I2CInit(ui32Base, bSpeed);

  delay(GYRO_STARTUP_DELAY);

  // Enable interrupts for immediate data processing based on INT2 (DRDY)
  gpioInputInit(ui32SenseBase, ui8SensePin, GPIO_PIN_TYPE_STD);
  gyro_ui32IntFlags = gpioIntInit(ui32SenseBase, ui8SensePin, GPIO_RISING_EDGE);

  // 1. Test if device is there
  if (!gyroDetect()) return GYRO_STARTUP_ERR;
  // 2. Write CTRL_REG3 (Interrupts)
  if (!gyroConfigInterrupts(GYRO_I2_DRDY)) return GYRO_STARTUP_ERR;
  // 3. Write CTRL_REG4 (Data Config, Self test, SPI)
  if (!gyroDataConfig(GYRO_BATCH_UPDATE, GYRO_DPS250)) return GYRO_STARTUP_ERR;
  // 4. Write CTRL_REG5 (Boot Mode)
  if (!gyroBoot(GYRO_BOOT_NORMAL)) return GYRO_STARTUP_ERR;
  // 5. Write CTRL_REG1 (Data rate, Bandwith)
  if (!gyroPowerOn(GYRO_ODR800FC110)) return GYRO_STARTUP_ERR;

  delay(GYRO_CONFIG_DELAY);

  // Reset DRDY state
  while (!gyroReadXYZ()) {
  }

  gyroCalibrate(GYRO_NUM_SAMPLES, GYRO_SIGMA_MULTIPLE);

  return INITIALIZING;
}
bool
gyroDetect(void) {
  uint8_t ui8Data;
  if (!I2CWrite(gyro_ui32Base, gyro_ui8GyroAddr, GYRO_WHO_AM_I)) return false;
  if (!I2CRead(gyro_ui32Base, gyro_ui8GyroAddr, &ui8Data)) return false;
  return (ui8Data & 0xFF) == GYRO_ID ? true : false;
}
bool
gyroPowerOn(uint32_t ui32odr) {
  uint8_t ui8Data[2] = { GYRO_CTRL_REG1, ui32odr | GYRO_NORMAL | GYRO_ZEN | GYRO_YEN | GYRO_XEN };
  if (!I2CBurstWrite(gyro_ui32Base, gyro_ui8GyroAddr, &ui8Data[0], 2)) return false;
  return true;
}
bool
gyroPowerDown(void) {
  uint8_t ui8Data[2] = { GYRO_CTRL_REG1, GYRO_PD };
  if (!I2CBurstWrite(gyro_ui32Base, gyro_ui8GyroAddr, &ui8Data[0], 2)) return false;
  return true;
}
bool
gyroSleep(void) {
  uint8_t ui8Data[2] = { GYRO_CTRL_REG1, GYRO_SLEEP };
  if (!I2CBurstWrite(gyro_ui32Base, gyro_ui8GyroAddr, &ui8Data[0], 2)) return false;
  return true;
}
bool
gyroConfigHPF(uint32_t ui32bhr, uint32_t ui32hpf) {
  uint8_t ui8Data[2] = { GYRO_CTRL_REG2, ui32bhr | ui32hpf };
  if (!I2CBurstWrite(gyro_ui32Base, gyro_ui8GyroAddr, &ui8Data[0], 2)) return false;
  return true;
}
bool
gyroConfigInterrupts(uint32_t ui32intr) {
  uint8_t ui8Data[2] = { GYRO_CTRL_REG3, ui32intr };
  if (!I2CBurstWrite(gyro_ui32Base, gyro_ui8GyroAddr, &ui8Data[0], 2)) return false;
  return true;
}
bool
gyroDataConfig(uint32_t ui32upd, uint32_t ui32res) {
  uint8_t ui8Data[2] = { GYRO_CTRL_REG4, ui32upd | GYRO_LITTLE_ENDIAN | ui32res };
  if (!I2CBurstWrite(gyro_ui32Base, gyro_ui8GyroAddr, &ui8Data[0], 2)) return false;
  switch (ui32res) {
    case 0x00 << 4: {
      gyro_fSensitivity = GYRO_250_SENS;
      break;
    }
    case 0x01 << 4: {
      gyro_fSensitivity = GYRO_500_SENS;
      break;
    }
    case 0x02 << 4: {
      gyro_fSensitivity = GYRO_2000_SENS;
      break;
    }
  }
  return true;
}
bool
gyroSelfTest(uint32_t ui32test) {
  uint8_t ui8Data[2] = { GYRO_CTRL_REG4, ui32test };
  if (!I2CBurstWrite(gyro_ui32Base, gyro_ui8GyroAddr, &ui8Data[0], 2)) return false;
  return true;
}
bool
gyroStatus(uint8_t* ui8Status) {
  uint8_t ui8Data;
  if (!I2CWrite(gyro_ui32Base, gyro_ui8GyroAddr, GYRO_STATUS_REG)) return false;
  if (!I2CRead(gyro_ui32Base, gyro_ui8GyroAddr, &ui8Data)) return false;
  *ui8Status = (uint8_t) ui8Data;
  return true;
}
bool
gyroBoot(uint32_t ui32boot) {
  uint8_t ui8Data[2] = { GYRO_CTRL_REG5, ui32boot };
  if (!I2CBurstWrite(gyro_ui32Base, gyro_ui8GyroAddr, &ui8Data[0], 2)) return false;
  return true;
}
bool
gyroFIFO(uint32_t ui32fifo) {
  uint8_t ui8Data[2] = { GYRO_CTRL_REG5, ui32fifo };
  if (!I2CBurstWrite(gyro_ui32Base, gyro_ui8GyroAddr, &ui8Data[0], 2)) return false;
  return true;
}
bool
gyroHPF(uint32_t ui32hpf) {
  uint8_t ui8Data[2] = { GYRO_CTRL_REG5, ui32hpf };
  if (!I2CBurstWrite(gyro_ui32Base, gyro_ui8GyroAddr, &ui8Data[0], 2)) return false;
  return true;
}
bool
gyroINT1(uint32_t ui32int1) {
  uint8_t ui8Data[2] = { GYRO_CTRL_REG5, ui32int1 };
  if (!I2CBurstWrite(gyro_ui32Base, gyro_ui8GyroAddr, &ui8Data[0], 2)) return false;
  return true;
}
bool
gyroOutSel(uint32_t ui32out) {
  uint8_t ui8Data[2] = { GYRO_CTRL_REG5, ui32out };
  if (!I2CBurstWrite(gyro_ui32Base, gyro_ui8GyroAddr, &ui8Data[0], 2)) return false;
  return true;
}
bool
gyroReadXYZ(void) {
  uint32_t i;
  uint8_t ui8Data[6] = { 0 };
  float fDelta[3];
  if (!I2CWrite(gyro_ui32Base, gyro_ui8GyroAddr, GYRO_OUT_X_L | GYRO_AUTO_INCREMENT)) return false;
  if (!I2CBurstRead(gyro_ui32Base, gyro_ui8GyroAddr, &ui8Data[0], 6)) return false;
  for (i = 0; i < 3; i++) {
    gyro_i16Raw[i] = (((int16_t) ui8Data[(i * 2) + 1]) << 8) | ui8Data[(i * 2)];
    fDelta[i] = gyro_i16Raw[i] - gyro_fZeroRate[i];  // Use the calibration data to modify the sensor value.
    if (abs(fDelta[i]) < gyro_fThreshold[i])  // If data is below threshold, output 0
      fDelta[i] = 0;
    gyro_fDPS[i] = gyro_fSensitivity * fDelta[i];  // Multiply the sensor value by the sensitivity factor to get degrees per second.
  }
  return true;
}
int16_t*
gyroRetrieveXYZRaw(void) {
  return &gyro_i16Raw[0];
}
bool
gyroReceive(void) {
  bool temp = gyro_dataAvailable;
  gyro_dataAvailable = false;
  return temp;
}
bool
gyroReadTemp(int8_t *ui8temp) {
  uint8_t ui8Data;
  if (!I2CWrite(gyro_ui32Base, gyro_ui8GyroAddr, GYRO_OUT_TEMP)) return false;
  if (!I2CRead(gyro_ui32Base, gyro_ui8GyroAddr, &ui8Data)) return false;
  *ui8temp = ((int8_t) (40 - ui8Data)) * SENSORS_TEMP_CONVERSION_STANDARD;
  return true;
}
