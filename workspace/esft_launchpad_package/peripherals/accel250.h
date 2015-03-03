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

#ifdef __cplusplus
extern "C"
{
#endif

// TODO: Get correct Zero G Vout. Current value is theory
#define ACCEL_250_ZERO_G_VOUT ((float) 1.65)
// TODO: Calculate correct G resolution. Current value is theory.
#define ACCEL_250_G_RESOLUTION ((float) 0.0066)

//*****************************************************************************
//
// Module function prototypes.
//
//*****************************************************************************
extern void accel250Init(void);
extern StatusCode_t accel250Receive(float* fptrForce);

//*****************************************************************************
//
// Mark the end of the C bindings section for C++ compilers.
//
//*****************************************************************************
#ifdef __cplusplus
}
#endif

#endif /* __ACCEL_H__ */
