/*
 * gyro.c
 *
 *  Created on: Feb 17, 2015
 *      Author: Ryan
 */

#include <math.h>
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "gyro.h"
#include "i2c.h"
#include "misc.h"

float    gyro_fSensitivity;
uint32_t gyro_ui32ZeroRate[3];
uint32_t gyro_ui32Threshold[3];
uint32_t gyro_ui32Base;
uint8_t  gyro_ui8GyroAddr;

void
gyroCalibrate(uint32_t ui32SampleCount, uint32_t ui32SigmaMultiple) {
  uint64_t ui64Sums[3]={0}, ui64Sigma[3]={0};
  uint32_t i, j, ui32AvgRate;
  int16_t i16Raw[3];
  for (i=0;i<ui32SampleCount;i++) {
    while(!gyroReadXYZRaw(&i16Raw[0], &i16Raw[1], &i16Raw[2])) {};
    for (j=0;j<3;j++) {
      ui64Sums[j]+=i16Raw[j];
      ui64Sums[j]+=pow(i16Raw[j],2);
    }
  }
  for (i=0;i<3;i++) {
    ui32AvgRate=ui64Sums[j]/ui32SampleCount;
    // Per STM docs, we store the average of the samples for each axis and subtract them when we use the data.
    gyro_ui32ZeroRate[i]=ui32AvgRate;
    // Per STM docs, we create a threshold for each axis based on the standard deviation of the samples times 3.
    gyro_ui32Threshold[j]=sqrt(((double) (ui64Sigma[j]) / ui32SampleCount) - pow(ui32AvgRate,2)) * ui32SigmaMultiple;
  }
}
void
gyroInit(uint32_t ui32Base, uint8_t ui8GyroAddr, bool bSpeed) {
  gyro_ui32Base = ui32Base;
  gyro_ui8GyroAddr = ui8GyroAddr;

  //
  // Enable the I2C module used by the gyro
  //
  I2CInit(gyro_ui32Base, bSpeed);

  delay(GYRO_STARTUP_DELAY);

  while (!gyroStartup()) {}
}
bool
gyroReceive(float* fDPS) {
  uint32_t ui32Delta[3], i;
  int16_t i16Raw[3];

  if (!gyroReadXYZRaw(&i16Raw[0], &i16Raw[1], &i16Raw[2])) return false;

  for (i=0;i<3;i++) {
    ui32Delta[i]=i16Raw[i]-gyro_ui32ZeroRate[i];      // Use the calibration data to modify the sensor value.
    if (abs(ui32Delta[i]) < gyro_ui32Threshold[i])
      ui32Delta[i]=0;
    fDPS[i]= gyro_fSensitivity * ui32Delta[i];      // Multiply the sensor value by the sensitivity factor to get degrees per second.
  }

  return true;
}
bool
gyroStartup() {
  if (!gyroTest()) return false;
  // 1. Write CTRL_REG2 (HPF Behavior, Cutoff Freq)
  if (!gyroConfigReg2(GYRO_NORMAL_MODE, GYRO_ODR800HPFC0P01)) return false;
  // 2. Write CTRL_REG3 (Interrupts)
  // if (!gyroConfigReg3()) return false;
  // 3. Write CTRL_REG4 (Data Config, Self test, SPI)
  if (!gyroDataConfig(GYRO_CONTINUOUS_UPDATE, GYRO_BIG_ENDIAN, GYRO_DPS500)) return false;
  // 4. Write CTRL_REG5 (Boot Mode, FIFO Behavior, HPF Enable, Int1 Selection, Out Selection)
  if (!gyroBoot(GYRO_BOOT_NORMAL)) return false;
  // 5. Write CTRL_REG1 (Power, Data rate, Bandwith, Axis)
  if (!gyroConfigReg1(GYRO_ODR800FC110)) return false;
  return true;
}

/*
 * @brief: Test connection for l3G4200D
 */
bool
gyroTest() {
  uint32_t ui32Data;
  if (!I2CWrite(gyro_ui32Base, gyro_ui8GyroAddr, GYRO_WHO_AM_I)) return false;
  if (!I2CRead(gyro_ui32Base, gyro_ui8GyroAddr, &ui32Data)) return false;
  return ((uint8_t) ui32Data) == 0xD3 ? true :  false;
}

/*
 * @brief: Configure data rate, bandwidth and enable axis measurement
 *        Acceptable values:
 *           GYRO_ODR100FC12P5
 *           GYRO_ODR100FC25
 *           GYRO_ODR200FC12P5
 *           GYRO_ODR200FC25
 *           GYRO_ODR200FC50
 *           GYRO_ODR200FC70
 *           GYRO_ODR400FC20
 *           GYRO_ODR400FC25
 *           GYRO_ODR400FC50
 *           GYRO_ODR400FC110
 *           GYRO_ODR800FC30
 *           GYRO_ODR800FC35
 *           GYRO_ODR800FC50
 *           GYRO_ODR800FC110
 */
bool
gyroConfigReg1(uint32_t ui32odr) {
  uint8_t ui8Data[2] = {GYRO_CTRL_REG1, ui32odr|GYRO_NORMAL|GYRO_ZEN|GYRO_YEN|GYRO_XEN};
  if (!I2CBurstWrite(gyro_ui32Base, gyro_ui8GyroAddr, &ui8Data[0], 2)) return false;
  return true;
}

/*
 * @brief: Power down l3G4200D
 */
bool
gyroPowerDown() {
  uint8_t ui8Data[2] = {GYRO_CTRL_REG1, GYRO_PD};
  if (!I2CBurstWrite(gyro_ui32Base, gyro_ui8GyroAddr, &ui8Data[0], 2)) return false;
  return true;
}

/*
 * @brief: Sleep l3G4200D
 */
bool
gyroSleep() {
  uint8_t ui8Data[2] = {GYRO_CTRL_REG1, GYRO_SLEEP};
  if (!I2CBurstWrite(gyro_ui32Base, gyro_ui8GyroAddr, &ui8Data[0], 2)) return false;
  return true;
}

/*
 * @brief: Configure HPF behavior and cutoff frequency
 *        GYRO_HPF behavior.
 *        GYRO_RESET_FILTER
 *        GYRO_REF_SIG_FILTER
 *        GYRO_NORMAL_MODE
 *        GYRO_AUTORESET
 *         *
 *         HPF Cut off Frequency GYRO_ODRxxxHPFCy
 *        x : 100, 200, 400,  800
 *        y : 30, 15, 8, 4, 2, 1, 0P5, 0P2, 0P1, 0P05, 0P02, 0P01
 *        For available refer Table 27 in the datasheet
 * @param[out]: none
 */
bool
gyroConfigReg2(uint32_t ui32bhr, uint32_t ui32hpf) {
  uint8_t ui8Data[2] = {GYRO_CTRL_REG2, ui32bhr|ui32hpf};
  if (!I2CBurstWrite(gyro_ui32Base, gyro_ui8GyroAddr, &ui8Data[0], 2)) return false;
  return true;
}

/*
 * @brief: Configure interrupts on l3G4200D
 * @param[in]: Interrupts to enable. A combination of the following is possible
 *        GYRO_I1_Int1
 *        GYRO_I1_BOOT
 *        GYRO_H_LACTIVE
 *        GYRO_PP_OD
 *        GYRO_I2_DRDY
 *        GYRO_I2_WTM
 *        GYRO_I2_ORUN
 *        GYRO_I2_EMPTY
 */
bool
gyroConfigReg3(uint32_t ui32intr) {
  uint8_t ui8Data[2] = {GYRO_CTRL_REG3, 0x00};
  if (!I2CBurstWrite(gyro_ui32Base, gyro_ui8GyroAddr, &ui8Data[0], 2)) return false;
  ui8Data[1] = ui32intr;
  if (!I2CBurstWrite(gyro_ui32Base, gyro_ui8GyroAddr, &ui8Data[0], 2)) return false;
  return true;
}

/*
 * @brief: Configure data O/P method
 * @param[in]: Data update:
 *          GYRO_CONTINUOUS_UPDATE/GYRO_BATCH_UPDATE
 *             Endian
 *          GYRO_BIG_ENDIAN/GYRO_LITTLE_ENDIAN
 *             Data resolution
 *          GYRO_DPSx. x = 250, 500, 2000
 */
bool
gyroDataConfig(uint32_t ui32upd, uint32_t ui32end, uint32_t ui32res) {
  uint8_t ui8Data[2] = {GYRO_CTRL_REG4, ui32upd|ui32end|ui32res};
  if (!I2CBurstWrite(gyro_ui32Base, gyro_ui8GyroAddr, &ui8Data[0], 2)) return false;
  switch (ui32res) {
    case 0x00<<4: {
      gyro_fSensitivity = GYRO_250_SENS;
      break;
    }
    case 0x01<<4: {
      gyro_fSensitivity = GYRO_500_SENS;
      break;
    }
    case 0x02<<4: {
      gyro_fSensitivity = GYRO_2000_SENS;
      break;
    }
  }
  return true;
}

/*
 * @brief: Configure self test
 * @param[in]: Self test
 *        GYRO_DISABLE, GYRO_POSITIVE_SELF_TEST, GYRO_NEUTRAL_SELF_TEST, GYRO_NEGATIVE_SELF_TEST
 */
bool
gyroSelfTest(uint32_t ui32test) {
  uint8_t ui8Data[2] = {GYRO_CTRL_REG4, ui32test};
  if (!I2CBurstWrite(gyro_ui32Base, gyro_ui8GyroAddr, &ui8Data[0], 2)) return false;
  return true;
}

/*
 * @brief: Get status
 */
bool
gyroStatus(uint8_t* ui8Status) {
  uint32_t ui32Data;
  if (!I2CWrite(gyro_ui32Base, gyro_ui8GyroAddr, GYRO_STATUS_REG)) return false;
  if (!I2CRead(gyro_ui32Base, gyro_ui8GyroAddr, &ui32Data)) return false;
  *ui8Status = (uint8_t) ui32Data;
  return true;
}

/*
 * @brief: Configure SPI
 * @param[in]: GYRO_WIRE_INT_4, GYRO_WIRE_INT_3
 */
bool
gyroSPI(uint32_t ui32spi) {
  uint8_t ui8Data[2] = {GYRO_CTRL_REG4, ui32spi};
  if (!I2CBurstWrite(gyro_ui32Base, gyro_ui8GyroAddr, &ui8Data[0], 2)) return false;
  return true;
}

/*
 * @brief: Configure boot mode
 * @param[in]: GYRO_BOOT_NORMAL, GYRO_BOOT_MEM
 */
bool
gyroBoot(uint32_t ui32boot) {
  uint8_t ui8Data[2] = {GYRO_CTRL_REG5, ui32boot};
  if (!I2CBurstWrite(gyro_ui32Base, gyro_ui8GyroAddr, &ui8Data[0], 2)) return false;
  return true;
}

/*
 * @brief: Configure FIFO behaviour
 * @param[in]: GYRO_FIFO_DISABLE, GYRO_FIFO_ENABLE
 */
bool
gyroFIFO(uint32_t ui32fifo) {
  uint8_t ui8Data[2] = {GYRO_CTRL_REG5, ui32fifo};
  if (!I2CBurstWrite(gyro_ui32Base, gyro_ui8GyroAddr, &ui8Data[0], 2)) return false;
  return true;
}

/*
 * @brief: Enable HPF
 * @param[in]: GYRO_HIGH_PASS_DISABLE, GYRO_HIGH_PASS_ENABLE
 */
bool
gyroHPF(uint32_t ui32hpf) {
  uint8_t ui8Data[2] = {GYRO_CTRL_REG5, ui32hpf};
  if (!I2CBurstWrite(gyro_ui32Base, gyro_ui8GyroAddr, &ui8Data[0], 2)) return false;
  return true;
}

/*
 * @brief: Int1 selection configuration
 * @param[in]: GYRO_NON_HPF_FILT_INT, GYRO_HPF_FILT_INT, GYRO_LPF_FILT_INT, GYRO_HPF_LPF_INT
 */
bool
gyroINT1(uint32_t ui32int1) {
  uint8_t ui8Data[2] = {GYRO_CTRL_REG5, ui32int1};
  if (!I2CBurstWrite(gyro_ui32Base, gyro_ui8GyroAddr, &ui8Data[0], 2)) return false;
  return true;
}

/*
 * @brief: Out selection configuration
 * @param[in]: GYRO_DATA_NON_HPF, GYRO_DATA_LPF, GYRO_DATA_HPF_LPF
 */
bool
gyroOutSel(uint32_t ui32out) {
  uint8_t ui8Data[2] = {GYRO_CTRL_REG5, ui32out};
  if (!I2CBurstWrite(gyro_ui32Base, gyro_ui8GyroAddr, &ui8Data[0], 2)) return false;
  return true;
}

/*
 * @brief: Read X,Y, Z  angular rates
 * @param[in]: ptr to x, y and z data points
 */
bool
gyroReadXYZRaw(int16_t *x, int16_t *y, int16_t *z) {
  uint32_t ui32Data[2];
  uint8_t ui8Status = 0x0; // Gyro Status

  if (!(gyroStatus(&ui8Status))) return false;
  if ((ui8Status & GYRO_ZYXDA) != GYRO_ZYXDA) return false; // No new data is available

  if (!I2CWrite(gyro_ui32Base, gyro_ui8GyroAddr, GYRO_OUT_X_L)) return false;
  if (!I2CBurstRead(gyro_ui32Base, gyro_ui8GyroAddr, ui32Data, 2)) return false;
  *x = (int16_t)((uint16_t)ui32Data[0]|(uint16_t)ui32Data[1]<<8);

  if (!I2CWrite(gyro_ui32Base, gyro_ui8GyroAddr, GYRO_OUT_Y_L)) return false;
  if (!I2CBurstRead(gyro_ui32Base, gyro_ui8GyroAddr, ui32Data, 2)) return false;
  *y = (int16_t)((uint16_t)ui32Data[0]|(uint16_t)ui32Data[1]<<8);

  if (!I2CWrite(gyro_ui32Base, gyro_ui8GyroAddr, GYRO_OUT_Z_L)) return false;
  if (!I2CBurstRead(gyro_ui32Base, gyro_ui8GyroAddr, ui32Data, 2)) return false;
  *z = (int16_t)((uint16_t)ui32Data[0]|(uint16_t)ui32Data[1]<<8);

  return true;
}

/*
 * @brief: Read temperature
 * @param[in]: ptr to temperature data
 */
bool
gyroReadTemp(int8_t *ui8temp) {
  uint32_t ui32Data;
  if (!I2CWrite(gyro_ui32Base, gyro_ui8GyroAddr, GYRO_OUT_TEMP)) return false;
  if (!I2CRead(gyro_ui32Base, gyro_ui8GyroAddr, &ui32Data)) return false;
  *ui8temp = (int8_t)(40 - ui32Data);
  return true;
}
