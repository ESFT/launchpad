//*****************************************************************************
//
// gyro.h
//
// This file defines commands for the L3G4200D 3-Axis Gyroscope Module
//
// Information in this file was gathered from the parallax product page
// http://www.parallax.com/product/27911
//
//*****************************************************************************

#ifndef __GYRO_H__
#define __GYRO_H__

#include <stdint.h>

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

//
// Gyro Addresses
//
#define GYRO_ADDRESS_SDO_LO 0x68
#define GYRO_ADDRESS_SDO_HI 0x69

//
// Gyro Startup Delay
//
#define GYRO_STARTUP_DELAY 100

//
// Output register mapping
//
#define GYRO_WHO_AM_I      0x0F
#define GYRO_CTRL_REG1     0x20
#define GYRO_CTRL_REG2     0x21
#define GYRO_CTRL_REG3     0x22
#define GYRO_CTRL_REG4     0x23
#define GYRO_CTRL_REG5     0x24
#define GYRO_REFERENCE     0x25
#define GYRO_OUT_TEMP      0x26
#define GYRO_STATUS_REG    0x27
#define GYRO_OUT_X_L       0x28
#define GYRO_OUT_X_H       0x29
#define GYRO_OUT_Y_L       0x2A
#define GYRO_OUT_Y_H       0x2B
#define GYRO_OUT_Z_L       0x2C
#define GYRO_OUT_Z_H       0x2D
#define GYRO_FIFO_CTRL_REG 0x2E
#define GYRO_FIFO_SRC_REG  0x2F
#define GYRO_INT1_CFG      0x30
#define GYRO_INT1_SRC      0x31
#define GYRO_INT1_TSH_XH   0x32
#define GYRO_INT1_TSH_XL   0x33
#define GYRO_INT1_TSH_YH   0x34
#define GYRO_INT1_TSH_YL   0x35
#define GYRO_INT1_TSH_ZH   0x36
#define GYRO_INT1_TSH_ZL   0x37
#define GYRO_INT1_DURATION 0x38

//*****************************************************************************
//
// Mark the end of the C bindings section for C++ compilers.
//
//*****************************************************************************
#ifdef __cplusplus
}
#endif

#endif // __flashStore_H__
