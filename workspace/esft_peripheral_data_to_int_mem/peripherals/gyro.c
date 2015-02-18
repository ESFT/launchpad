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

void
gyroCalibrate(uint32_t ui32Base, uint8_t ui8GyroAddr, uint32_t ui32SampleCount, uint32_t ui32SigmaMultiple, uint32_t* ui32zRate, uint32_t* ui32Threshold, uint32_t* ui32Raw, float* fDPS) {
  uint64_t ui64Sums[3]={0}, ui64Sigma[3]={0};
  uint32_t i, j, averageRate;
  for (i=0;i<ui32SampleCount;i++) {
    while(!gyroReceive(ui32Base, ui8GyroAddr, ui32zRate, ui32Threshold, ui32Raw, fDPS)) {};
    for (j=0;j<3;j++) {
      ui64Sums[j]+=ui32Raw[j];
      ui64Sums[j]+=pow(ui32Raw[j],2);
    }
  }
  for (i=0;i<3;i++) {
    averageRate=ui64Sums[j]/ui32SampleCount;
    // Per STM docs, we store the average of the samples for each axis and subtract them when we use the data.
    ui32zRate[i]=averageRate;
    // Per STM docs, we create a threshold for each axis based on the standard deviation of the samples times 3.
    ui32Threshold[j]=sqrt(((double) (ui64Sigma[j]) / ui32SampleCount) - pow(averageRate,2)) * ui32SigmaMultiple;
  }
}
void
gyroInit(uint32_t ui32Base, uint8_t ui8GyroAddr, bool bSpeed) {
  //
  // Enable the I2C module used by the gyro
  //
  I2CInit(ui32Base, bSpeed);

  delay(GYRO_STARTUP_DELAY);

  while (!gyroStartup(ui32Base, ui8GyroAddr)) {}
}
bool
gyroReceive(uint32_t ui32Base, uint8_t ui8GyroAddr, uint32_t* ui32zRate, uint32_t* ui32Threshold, uint32_t* ui32Raw, float* fDPS) {
  uint32_t ui32Status = 0x0; // Gyro Status
  uint32_t ui32Data, ui32Delta[3], i;
  uint8_t ui38Register=GYRO_OUT_X_L;
  if (!I2CWrite(ui32Base, ui8GyroAddr, GYRO_STATUS_REG)) {
    return false;
  }
  if (!I2CRead(ui32Base, ui8GyroAddr, &ui32Status)) {
    return false;
  }
  if ( (ui32Status & 0x8) == 0x8) { // If new data is available
    for (i=0;i<3;i++) {
      if (!I2CWrite(ui32Base, ui8GyroAddr, ui38Register)) {
        return false;
      }
      if (!I2CRead(ui32Base, ui8GyroAddr, &ui32Data)) {
        return false;
      }
      ui32Raw[i] = ui32Data;
      if (!I2CWrite(ui32Base, ui8GyroAddr, ui38Register+1)) {
        return false;
      }
      if (!I2CRead(ui32Base, ui8GyroAddr, &ui32Data)) {
        return false;
      }
      ui32Raw[i] |= ui32Data<<8;
      ui38Register += 2;
    }
    for (i=0;i<3;i++) {
      ui32Delta[i]=ui32Raw[i]-ui32zRate[i];      // Use the calibration data to modify the sensor value.
      if (abs(ui32Delta[i]) < ui32Threshold[i])
        ui32Delta[i]=0;
      fDPS[i]= GYRO_500_SENS * ui32Delta[i];      // Multiply the sensor value by the sensitivity factor to get degrees per second.
    }
    return true;
  }
  return false;
}
bool
gyroStartup(uint32_t ui32Base, uint8_t ui8GyroAddr) {
  return true;
}
