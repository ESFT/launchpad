/*
 * status.h
 *
 *  Created on: Feb 17, 2015
 *      Author: Ryan
 */

#ifndef STATUS_H_
#define STATUS_H_

#include <stdbool.h>
#include <stdint.h>

//*****************************************************************************
//
// Status Codes (Uses LED)
// See ESFT Status Codes Spreadsheet for more information
//
//*****************************************************************************

// Defines the length of time between interrupts in milliseconds
#define STATUS_CODE_LENGTH 250

// length of "dot" and "dash" blinks as well as the delimiter length in terms of interrupt count
#define STATUS_DOT  1
#define STATUS_DASH 3
#define STATUS_DELIMTER 3

// Status Codes
typedef enum STATUSCODE {
    INITIALIZING, INITIALIZING_HOLD, // Initialization
    RUNNING, // Running
    MMC_MOUNT_ERR, MMC_OPEN_ERR, OUT_OF_FLASH, // Flash status
    DRL_ERR, BUS_FAULT, USAGE_FAULT, // Fault status
    ACC250_ADC_CONV_ERR, // 250G accelerometer status
    ALT_RESET_ERR, ALT_PROM_ERR, ALT_CRC_ERR, ALT_ADC_CONV_ERR, // Altimeter status
    COMPASS_ACCEL_STARTUP, COMPASS_MAG_STARTUP, // Compass status
    GPS_NAV_LOCK_HOLD, // GPS status
    GYRO_STARTUP_ERR // Gyro status
  } StatusCode_t;

//*****************************************************************************
//
// If building with a C++ compiler, make all of the definitions in this header
// have a C binding.
//
//*****************************************************************************
#ifdef __cplusplus
extern "C" {
#endif

extern bool setStatus(StatusCode_t scStatus);
extern void setStatusDefault(StatusCode_t scDefStatus);
extern void statusCodeInterruptInit(uint32_t ui32Base);
extern void statusIntHandler(void);

//*****************************************************************************
//
// Mark the end of the C bindings section for C++ compilers.
//
//*****************************************************************************
#ifdef __cplusplus
}
#endif

#endif /* STATUS_H_ */
