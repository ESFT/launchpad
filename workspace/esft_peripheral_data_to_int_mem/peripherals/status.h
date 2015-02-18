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
// If building with a C++ compiler, make all of the definitions in this header
// have a C binding.
//
//*****************************************************************************
#ifdef __cplusplus
extern "C"
{
#endif

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
    INITIALIZING, RUNNING, DRL_ERR, ALT_CRC_ERR, OUT_OF_FLASH,
    ALT_RESET_ERR, ALT_PROM_R_WRITE_ERR, ALT_PROM_R_READ_ERR,
    ALT_ADC_CONV_ERR, ALT_ADC_R_WRITE_ERR, ALT_ADC_R_READ_ERR
  } StatusCode_t;

bool setStatus(StatusCode_t scStatus);
void setStatusDefault(StatusCode_t scDefStatus);
void statusCodeInterruptInit(void);

//*****************************************************************************
//
// Mark the end of the C bindings section for C++ compilers.
//
//*****************************************************************************
#ifdef __cplusplus
}
#endif

#endif /* STATUS_H_ */
