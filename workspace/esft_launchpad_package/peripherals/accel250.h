/*
 * accel.h
 *
 *  Created on: Feb 17, 2015
 *      Author: Ryan
 */

#ifndef __ACCEL_250_H__
#define __ACCEL_H__

#include <stdbool.h>
#include <stdint.h>

#include "status.h"

//*****************************************************************************
//
// If building with a C++ compiler, make all of the definitions in this header
// have a C binding.
//
//*****************************************************************************
#ifdef __cplusplus
extern "C" {
#endif

// TODO: Get correct Zero G Vout. Current value is theory
#define ACCEL_250_ZERO_G_VOUT (1.581F)
// TODO: Calculate correct G resolution. Current value is theory.
#define ACCEL_250_G_RESOLUTION (0.1223443223443223F)

//*****************************************************************************
//
// Module function prototypes.
//
//*****************************************************************************
extern void accel250Init(float* fForcePtr, StatusCode_t* status);
extern bool accel250Receive(void);

//*****************************************************************************
//
// Mark the end of the C bindings section for C++ compilers.
//
//*****************************************************************************
#ifdef __cplusplus
}
#endif

#endif /* __ACCEL_H__ */
