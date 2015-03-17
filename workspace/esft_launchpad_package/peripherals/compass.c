/*
 * mag_accel.c
 *
 *  Created on: Mar 3, 2015
 *      Author: Ryan
 */

#include <math.h>

#include "driverlib/gpio.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"

#include "gpio.h"
#include "compass.h"
#include "i2c.h"
#include "misc.h"
#include "sensor_constants.h"
#include "status.h"
#include "uartstdio.h"

#define M_PI 3.14159265358979323846

static uint32_t compass_ui32Base;
static uint8_t  compass_ui8AccelAddr = COMPASS_ADDRESS_ACCEL;
static uint8_t  compass_ui8MagAddr = COMPASS_ADDRESS_MAG;
static uint32_t compass_ui32AccelSenseBase;
static uint8_t  compass_ui8AccelSensePin;
static uint32_t compass_ui32MagSenseBase;
static uint8_t  compass_ui8MagSensePin;

static float   compass_accel_MG_LSB = 0.0120; // 1, 2, 4 or 12 mg per lsb
static float   compass_mag_Gauss_LSB_XY = 1100.0; // Varies with gain
static float   compass_mag_Gauss_LSB_Z = 980.00; // Varies with gain

StatusCode_t
compassInit(uint32_t ui32Base, bool bSpeed, uint32_t ui32AccelSenseBase, uint8_t ui8AccelSensePin, uint32_t ui32MagSenseBase, uint8_t ui8MagSensePin) {
  compass_ui32Base = ui32Base;
  compass_ui32AccelSenseBase = ui32AccelSenseBase;
  compass_ui8AccelSensePin = ui8AccelSensePin;
  compass_ui32MagSenseBase = ui32MagSenseBase;
  compass_ui8MagSensePin = ui8MagSensePin;

  //
  // Enable the I2C module used by the compass
  //
  I2CInit(compass_ui32Base, bSpeed);
  gpioInputInit(compass_ui32AccelSenseBase, compass_ui8AccelSensePin, GPIO_PIN_TYPE_STD);
  gpioInputInit(compass_ui32MagSenseBase, compass_ui8MagSensePin, GPIO_PIN_TYPE_STD);

  //
  // Initialize sub modules on the compass
  //
  if (!compassAccelDataConfig(COMPASS_ACCEL_BATCH_UPDATE, COMPASS_ACCEL_16G, COMPASS_ACCEL_HR_EN)) return COMPASS_ACCEL_STARTUP;
  if (!compassAccelPowerOn(COMPASS_ACCEL_ODR_N1P344_L5P376)) return COMPASS_ACCEL_STARTUP;

  if (!compassSetMagGain(COMPASS_MAG_GAIN_5P6)) return COMPASS_MAG_STARTUP;
  if (!compassMagPowerOn(COMPASS_MAG_SINGLE)) return COMPASS_MAG_STARTUP;
  return INITIALIZING;
}
/***************************************************************************
 ACCELEROMETER
 ***************************************************************************/
bool
compassAccelReadXYZRaw(int16_t* i16Raw) {
  uint32_t i;
  uint8_t ui8Data[6] = { 0 };
  /*
  uint8_t ui8Status;
  if (!I2CWrite(compass_ui32Base, compass_ui8AccelAddr, COMPASS_ACCEL_STATUS_REG)) return false;
  if (!I2CRead(compass_ui32Base, compass_ui8AccelAddr, &ui8Status)) return false;
  if (!(ui8Status & COMPASS_ACCEL_ZYXDA)) return false;
  */
  if (!MAP_GPIOPinRead(compass_ui32AccelSenseBase, compass_ui8AccelSensePin)) return false;  // No new data is available
  if (!I2CWrite(compass_ui32Base, compass_ui8AccelAddr, COMPASS_ACCEL_OUT_X_L | COMPASS_ACCEL_AUTO_INCR)) return false;
  if (!I2CBurstRead(compass_ui32Base, compass_ui8AccelAddr, &ui8Data[0], 6)) return false;
  for (i = 0; i < 3; i++) {
    i16Raw[i] = (int16_t)(ui8Data[(i*2)] | (ui8Data[(i*2)+1] << 8)) >> 4;
  }
  return true;
}
bool
compassAccelPowerOn(uint8_t ui8ODR) {
  uint8_t ui8Reg1Data = ui8ODR|COMPASS_ACCEL_NORMAL|COMPASS_ACCEL_ZEN|COMPASS_ACCEL_YEN|COMPASS_ACCEL_XEN;
  uint8_t ui8Data[2] = { COMPASS_ACCEL_CTRL_REG1, ui8Reg1Data };
  if (!I2CBurstWrite(compass_ui32Base, compass_ui8MagAddr, &ui8Data[0], 2)) return false;
  return true;
}
bool
compassAccelIntConfig(uint8_t ui8Interrupts) {
  uint8_t ui8Data[2] = { COMPASS_ACCEL_CTRL_REG3, ui8Interrupts };
  if (!I2CBurstWrite(compass_ui32Base, compass_ui8MagAddr, &ui8Data[0], 2)) return false;
  return true;
}
bool
compassAccelDataConfig(uint8_t ui8BDU, uint8_t ui8Sensitivity, uint8_t ui8Resolution) {
  uint8_t ui8Reg4Data = (ui8BDU|COMPASS_ACCEL_LITTLE_ENDIAN|ui8Sensitivity|ui8Resolution)&0xF9; // BitAND to ensure sub(2,1) are 0;
  uint8_t ui8Data[2] = { COMPASS_ACCEL_CTRL_REG4, ui8Reg4Data };
  if (!I2CBurstWrite(compass_ui32Base, compass_ui8MagAddr, &ui8Data[0], 2)) return false;
  switch (ui8Reg4Data & COMPASS_ACCEL_16G) {
    case COMPASS_ACCEL_2G: {
      compass_accel_MG_LSB = 0.001;
      break;
    }
    case COMPASS_ACCEL_4G: {
      compass_accel_MG_LSB = 0.002;
      break;
    }
    case COMPASS_ACCEL_8G: {
      compass_accel_MG_LSB = 0.004;
      break;
    }
    case COMPASS_ACCEL_16G: {
      compass_accel_MG_LSB = 0.012;
      break;
    }
  }
  return true;
}
bool
compassAccelReceive(float *fAccel) {
  int16_t i16Raw[3] = { 0 }, i;
  if (!compassAccelReadXYZRaw(&i16Raw[0])) return false;
  for (i = 0; i < 3; i++) {
    fAccel[i] = i16Raw[i] * compass_accel_MG_LSB * SENSORS_GRAVITY_STANDARD;
  }
  return true;
}

/***************************************************************************
 MAGNETOMETER
 ***************************************************************************/
bool
compassMagReadXYZRaw(int16_t *i16Raw) {
  uint32_t i;
  uint8_t ui8Data[6]={0};
  /*
  uint8_t ui8Status;
  if (!I2CWrite(compass_ui32Base, compass_ui8MagAddr, COMPASS_MAG_STATUS_REG)) return false;
  if (!I2CRead(compass_ui32Base, compass_ui8MagAddr, &ui8Status)) return false;
  if (!(ui8Status & COMPASS_MAG_DRDY)) return false;
  */
  if (MAP_GPIOPinRead(compass_ui32MagSenseBase, compass_ui8MagSensePin)) return false;  // No new data is available
  if (!I2CWrite(compass_ui32Base, compass_ui8MagAddr, COMPASS_MAG_OUT_X_H)) return false;
  if (!I2CBurstRead(compass_ui32Base, compass_ui8MagAddr, &ui8Data[0], 6)) return false;
  for (i = 0; i < 3; i++) {
    i16Raw[i] = ((int16_t)ui8Data[(i*2)] << 8) | ui8Data[(i*2)+1];
  }
  return true;
}
bool
compassMagPowerOn(uint8_t ui8ConversionMode) {
  uint8_t ui8Data[2] = { COMPASS_MAG_MR_REG, ui8ConversionMode };
  // Enable the magnetometer
  if (!I2CBurstWrite(compass_ui32Base, compass_ui8MagAddr, &ui8Data[0], 2)) return false;
  return true;
}
bool
compassSetMagGain(uint8_t gain) {
  uint8_t ui8Data[2] = { COMPASS_MAG_CRB_REG, gain };
  if (!I2CBurstWrite(compass_ui32Base, compass_ui8MagAddr, &ui8Data[0], 2)) return false;

  switch(gain) {
    case COMPASS_MAG_GAIN_1P3: {
      compass_mag_Gauss_LSB_XY = 1100;
      compass_mag_Gauss_LSB_Z  = 980;
      break;
    }
    case COMPASS_MAG_GAIN_1P9: {
      compass_mag_Gauss_LSB_XY = 855;
      compass_mag_Gauss_LSB_Z  = 760;
      break;
    }
    case COMPASS_MAG_GAIN_2P5: {
      compass_mag_Gauss_LSB_XY = 670;
      compass_mag_Gauss_LSB_Z  = 600;
      break;
    }
    case COMPASS_MAG_GAIN_4: {
      compass_mag_Gauss_LSB_XY = 450;
      compass_mag_Gauss_LSB_Z  = 400;
      break;
    }
    case COMPASS_MAG_GAIN_4P7: {
      compass_mag_Gauss_LSB_XY = 400;
      compass_mag_Gauss_LSB_Z  = 355;
      break;
    }
    case COMPASS_MAG_GAIN_5P6: {
      compass_mag_Gauss_LSB_XY = 330;
      compass_mag_Gauss_LSB_Z  = 295;
      break;
    }
    case COMPASS_MAG_GAIN_8P1: {
      compass_mag_Gauss_LSB_XY = 230;
      compass_mag_Gauss_LSB_Z  = 205;
      break;
    }
  }
  return true;
}
bool
compassMagReceive(float *fMag) {
  int16_t i16Raw[3] = { 0 };

  /* Read new data */
  if (!compassMagReadXYZRaw(&i16Raw[0])) return false; // No data available

  fMag[0] = i16Raw[0] / compass_mag_Gauss_LSB_XY * SENSORS_GAUSS_CONVERSION_STANDARD;
  fMag[1] = i16Raw[1] / compass_mag_Gauss_LSB_XY * SENSORS_GAUSS_CONVERSION_STANDARD;
  fMag[2] = i16Raw[2] / compass_mag_Gauss_LSB_Z  * SENSORS_GAUSS_CONVERSION_STANDARD;
  return true;
}
/* tilt-compensated e-Compass code */
void
compassCalculateHeading(float fBx, float fBy, float fBz, float fVx, float fVy, float fVz, float fGx, float fGy, float fGz, float *fAngles) {
  /* stack variables */
  /* fBx, fBy, fBz: the three components of the magnetometer sensor */
  /* fVx, fVy, fVz: the hard iron estimates for the magnetometer sensor */
  /* fGx, fGy, fGz: the three components of the accelerometer sensor */

  /* local variables */
  int16_t fSin, fCos; /* sine and cosine */
  float fCBx, fCBy; /* magnetic field readings corrected for PCB orientation */

  /* subtract the hard iron offset */
  fBx -= fVx; // see Eq 16
  fBy -= fVy; // see Eq 16
  fBz -= fVz; // see Eq 16

  /* calculate current roll angle Phi */
  fAngles[0] = atan(fGy/fGz); // Eq 13

  /* calculate sin and cosine of roll angle Phi */
  fSin = asin(fGy/fGz) * 180 / M_PI; // Eq 13: sin = opposite / hypotenuse
  fCos = acos(fGz/fGy) * 180 / M_PI; // Eq 13: cos = adjacent / hypotenuse

  /* de-rotate by roll angle Phi */
  fCBy = fBy * fCos - fBz * fSin; // Eq 19 y component
  fBz  = fBy * fSin + fBz * fCos; // By*sin(Phi)+Bz*cos(Phi)
  fBz  = fGy * fSin + fGz * fCos; // Eq 15 denominator

  /* calculate current pitch angle Theta */
  fAngles[1] = atan((-fGx)/fGz) * 180 / M_PI; // Eq 15

  /* restrict pitch angle to range -90 to 90 degrees */
  if (fAngles[1] > 90) {
    fAngles[1] = ( 180 - fAngles[1]);
  } else if (fAngles[1] < -90) {
    fAngles[1] = (-180 - fAngles[1]);
  }

  /* calculate sin and cosine of pitch angle Theta */
  fSin = -asin(fGx/fGz); // Eq 15: sin = opposite / hypotenuse
  fCos = -acos(fGz/fGx); // Eq 15: cos = adjacent / hypotenuse

  /* correct cosine if pitch not in range -90 to 90 degrees */
  if (fCos < 0) fCos = -fCos;

  /* de-rotate by pitch angle Theta */
  fCBx = ( fBx) * fCos + fBz * fSin; // Eq 19: x component

  /* calculate current yaw = e-compass angle Psi */
  fAngles[2] = atan((-fCBy)/fCBx) * 180 / M_PI; // Eq 22
}

