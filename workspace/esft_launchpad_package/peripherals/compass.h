/***************************************************************************
  This is a library for the LSM303 Compass

  Designed specifically to work with the Adafruit LSM303DLHC Breakout

 ***************************************************************************/
#ifndef __COMPASS_H__
#define __COMPASS_H__

#include <stdbool.h>
#include <stdint.h>

#include "status.h"

#define COMPASS_ADDRESS_ACCEL 0x19
#define COMPASS_ADDRESS_MAG   0x1E
#define COMPASS_STARTUP_DELAY 1

//
// Accelerometer
//
#define COMPASS_ACCEL_AUTO_INCR     0x80
#define COMPASS_ACCEL_CTRL_REG1     0x20
#define COMPASS_ACCEL_CTRL_REG2     0x21
#define COMPASS_ACCEL_CTRL_REG3     0x22
#define COMPASS_ACCEL_CTRL_REG4     0x23
#define COMPASS_ACCEL_CTRL_REG5     0x24
#define COMPASS_ACCEL_CTRL_REG6     0x25
#define COMPASS_ACCEL_REFERENCE     0x26
#define COMPASS_ACCEL_STATUS_REG    0x27
#define COMPASS_ACCEL_OUT_X_L       0x28
#define COMPASS_ACCEL_OUT_X_H       0x29
#define COMPASS_ACCEL_OUT_Y_L       0x2A
#define COMPASS_ACCEL_OUT_Y_H       0x2B
#define COMPASS_ACCEL_OUT_Z_L       0x2C
#define COMPASS_ACCEL_OUT_Z_H       0x2D
#define COMPASS_ACCEL_FIFO_CTRL_REG 0x2E
#define COMPASS_ACCEL_FIFO_SRC_REG  0x2F
#define COMPASS_ACCEL_INT1_CFG      0x30
#define COMPASS_ACCEL_INT1_SOURCE   0x31
#define COMPASS_ACCEL_INT1_THS      0x32
#define COMPASS_ACCEL_INT1_DURATION 0x33
#define COMPASS_ACCEL_INT2_CFG      0x34
#define COMPASS_ACCEL_INT2_SOURCE   0x35
#define COMPASS_ACCEL_INT2_THS      0x36
#define COMPASS_ACCEL_INT2_DURATION 0x37
#define COMPASS_ACCEL_CLICK_CFG     0x38
#define COMPASS_ACCEL_CLICK_SRC     0x39
#define COMPASS_ACCEL_CLICK_THS     0x3A
#define COMPASS_ACCEL_TIME_LIMIT    0x3B
#define COMPASS_ACCEL_TIME_LATENCY  0x3C
#define COMPASS_ACCEL_TIME_WINDOW   0x3D

// Control Register 1
#define COMPASS_ACCEL_ODR_PD            0x00<<4 // 0000
#define COMPASS_ACCEL_ODR1              0x01<<4 // 0001
#define COMPASS_ACCEL_ODR10             0x02<<4 // 0010
#define COMPASS_ACCEL_ODR25             0x03<<4 // 0011
#define COMPASS_ACCEL_ODR50             0x04<<4 // 0100
#define COMPASS_ACCEL_ODR100            0x05<<4 // 0101
#define COMPASS_ACCEL_ODR200            0x06<<4 // 0110
#define COMPASS_ACCEL_ODR400            0x07<<4 // 0111
#define COMPASS_ACCEL_ODR_L1P620        0x08<<4 // 1000
#define COMPASS_ACCEL_ODR_N1P344_L5P376 0x09<<4 // 1001

#define COMPASS_ACCEL_LP                0x1<<3
#define COMPASS_ACCEL_NORMAL            0x0<<3

#define COMPASS_ACCEL_ZEN               0x1<<2
#define COMPASS_ACCEL_YEN               0x1<<1
#define COMPASS_ACCEL_XEN               0x1

// Control Register 3
#define COMPASS_ACCEL_I1_CLICK 0x01<<7
#define COMPASS_ACCEL_I1_AOI1  0x01<<6
#define COMPASS_ACCEL_I1_AOI2  0x01<<5
#define COMPASS_ACCEL_I1_DRDY1 0x01<<4
#define COMPASS_ACCEL_I1_DRDY2 0x01<<3
#define COMPASS_ACCEL_I1_WTM   0x01<<2
#define COMPASS_ACCEL_I2_ORUN  0x01<<1

// Control Register 4
#define COMPASS_ACCEL_CONTINUOUS_UPDATE  0x00<<7
#define COMPASS_ACCEL_BATCH_UPDATE       0x01<<7
#define COMPASS_ACCEL_BIG_ENDIAN         0x00<<6
#define COMPASS_ACCEL_LITTLE_ENDIAN      0x01<<6
#define COMPASS_ACCEL_2G                 0x00<<4
#define COMPASS_ACCEL_4G                 0x01<<4
#define COMPASS_ACCEL_8G                 0x02<<4
#define COMPASS_ACCEL_16G                0x03<<4
#define COMPASS_ACCEL_HR_EN              0x01<<3
#define COMPASS_ACCEL_WIRE_INT_4         0x00
#define COMPASS_ACCEL_WIRE_INT_3         0x01

// Control Register 5
#define COMPASS_ACCEL_BOOT_NORMAL       0x00<<7
#define COMPASS_ACCEL_BOOT_MEM          0x01<<7
#define COMPASS_ACCEL_FIFO_DISABLE      0x00<<6
#define COMPASS_ACCEL_FIFO_ENABLE       0x01<<6
#define COMPASS_ACCEL_LIR_INT1_DISABLE  0x00<<3
#define COMPASS_ACCEL_LIR_INT1_ENABLE   0x01<<3
#define COMPASS_ACCEL_D4D_INT1_DISABLE  0x00<<2
#define COMPASS_ACCEL_D4D_INT1_ENABLE   0x01<<2
#define COMPASS_ACCEL_LIR_INT2_DISABLE  0x00<<1
#define COMPASS_ACCEL_LIR_INT2_ENABLE   0x01<<1
#define COMPASS_ACCEL_D4D_INT2_DISABLE  0x00
#define COMPASS_ACCEL_D4D_INT2_ENABLE   0x01

// Control Register 6
#define COMPASS_ACCEL_I2_CLICK_ENABLE   0x01<<7
#define COMPASS_ACCEL_I2_CLICK_DISABLE  0x00<<7
#define COMPASS_ACCEL_I2_INT1_ENABLE    0x01<<6
#define COMPASS_ACCEL_I2_INT1_DISABLE   0x00<<6
#define COMPASS_ACCEL_I2_INT2_ENABLE    0x01<<5
#define COMPASS_ACCEL_I2_INT2_DISABLE   0x00<<5
#define COMPASS_ACCEL_BOOT_I1_ENABLE    0x01<<4
#define COMPASS_ACCEL_BOOT_I1_DISABLE   0x00<<4
#define COMPASS_ACCEL_P2_ACT_ENABLE     0x01<<3
#define COMPASS_ACCEL_P2_ACT_DISABLE    0x00<<3
#define COMPASS_ACCEL_H_LACTIVE_ENABLE  0x01<<1
#define COMPASS_ACCEL_H_LACTIVE_DISABLE 0x00<<1

//Status register
#define COMPASS_ACCEL_ZYXOR 0x01<<7
#define COMPASS_ACCEL_ZOR   0x01<<6
#define COMPASS_ACCEL_YOR   0x01<<5
#define COMPASS_ACCEL_XOR   0x01<<4
#define COMPASS_ACCEL_ZYXDA 0x01<<3
#define COMPASS_ACCEL_ZDA   0x01<<2
#define COMPASS_ACCEL_YDA   0x01<<1
#define COMPASS_ACCEL_XDA   0x01

//
// Magnetometer
//
#define COMPASS_MAG_CRA_REG    0x00
#define COMPASS_MAG_CRB_REG    0x01
#define COMPASS_MAG_MR_REG     0x02
#define COMPASS_MAG_OUT_X_H    0x03
#define COMPASS_MAG_OUT_X_L    0x04
#define COMPASS_MAG_OUT_Z_H    0x05
#define COMPASS_MAG_OUT_Z_L    0x06
#define COMPASS_MAG_OUT_Y_H    0x07
#define COMPASS_MAG_OUT_Y_L    0x08
#define COMPASS_MAG_STATUS_REG 0x09
#define COMPASS_MAG_IRA_REG    0x0A
#define COMPASS_MAG_IRB_REG    0x0B
#define COMPASS_MAG_IRC_REG    0x0C
#define COMPASS_MAG_TEMP_OUT_H 0x31
#define COMPASS_MAG_TEMP_OUT_L 0x32

// CRA Register
#define COMPASS_MAG_TEMP_ENABLE  0x01<<7
#define COMPASS_MAG_TEMP_DISABLE 0x00<<7
#define COMPASS_ODRP75           0x01<<2
#define COMPASS_ODR1P5           0x02<<2
#define COMPASS_ODR3             0x03<<2
#define COMPASS_ODR7P5           0x04<<2
#define COMPASS_ODR15            0x05<<2
#define COMPASS_ODR30            0x06<<2
#define COMPASS_ODR75            0x07<<2
#define COMPASS_ODR220           0x08<<2

// CRB Register
#define COMPASS_MAG_GAIN_1P3 0x01<<5  // +/- 1.3
#define COMPASS_MAG_GAIN_1P9 0x02<<5  // +/- 1.9
#define COMPASS_MAG_GAIN_2P5 0x03<<5  // +/- 2.5
#define COMPASS_MAG_GAIN_4   0x04<<5  // +/- 4.0
#define COMPASS_MAG_GAIN_4P7 0x05<<5  // +/- 4.7
#define COMPASS_MAG_GAIN_5P6 0x06<<5  // +/- 5.6
#define COMPASS_MAG_GAIN_8P1 0x07<<5  // +/- 8.1

// MR Register
#define COMPASS_MAG_CONTINOUS 0x00
#define COMPASS_MAG_SINGLE    0x01
#define COMPASS_MAG_SLEEP     0x01<<1

// Status Register
#define COMPASS_MAG_DRDY 0x01
#define COMPASS_MAG_LOCK 0x02

//*****************************************************************************
//
// If building with a C++ compiler, make all of the definitions in this header
// have a C binding.
//
//*****************************************************************************
#ifdef __cplusplus
extern "C"
{
#endif

extern StatusCode_t compassInit(uint32_t ui32Base, bool bSpeed);

//
// Accelerometer
//

/*
 @brief  Read status of Accelerometer
*/
extern bool compassAccelStatus(uint8_t* ui8Status);
/*
 @brief  Reads the raw data from the sensor
*/
extern bool compassAccelReadXYZ(float *fAccel);
/*
 @brief  Receive pointer to raw value array
*/
extern int16_t* compassAccelRetrieveRaw(void);
/*
 @brief  Enable the accelerometer
*/
extern bool compassAccelPowerOn(uint8_t ui8ODR);
/*
 @brief  Configure data out params of the accelerometer
*/
extern bool compassAccelDataConfig(uint8_t ui8BDU, uint8_t ui8Sensitivity, uint8_t ui8Resolution);
/*
 @brief  Configure accelerometer interrupts
*/
extern bool compassAccelIntConfig(uint8_t ui8Interrupts);

//
// Magnetometer
//

/*
 @brief  Read status of Magnetometer
*/
extern bool compassMagStatus(uint8_t* ui8Status);
/*
 @brief  Reads the raw data from the sensor
*/
extern bool compassMagReadXYZ(float *fMag);
/*
 @brief  Receive pointer to raw value array
*/
extern int16_t* compassMagRetrieveRaw(void);
/*!
 @brief  Enable the magnetometer
*/
extern bool compassMagPowerOn(uint8_t ui8ConversionMode);
/*
 @brief  Sets the magnetometer's gain
*/
extern bool compassMagSetGain(uint8_t gain);
/*
 @brief  Enable the temp sensor and set the ODR
*/
extern bool compassMagSetODRTemp(bool bTempEn, uint8_t ui8ODR);

//
// eCompass
//

/*
 * Brief: tilt-compensated e-Compass code
 * fBx, fBy, fBz: the three components of the magnetometer sensor
 * fVx, fVy, fVz: the hard iron estimates for the magnetometer sensor
 * fGx, fGy, fGz: the three components of the accelerometer sensor
 * Code contained gathered from http://cache.freescale.com/files/sensors/doc/app_note/AN4248.pdf
 */
extern void compassCalculateHeading(float fBx, float fBy, float fBz, float fVx, float fVy, float fVz, float fGx, float fGy, float fGz, float *fAngles);

//*****************************************************************************
//
// Mark the end of the C bindings section for C++ compilers.
//
//*****************************************************************************
#ifdef __cplusplus
}
#endif

#endif // __COMPASS_H__
