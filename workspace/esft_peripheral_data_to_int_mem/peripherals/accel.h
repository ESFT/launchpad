/*
 * accel.h
 *
 *  Created on: Feb 17, 2015
 *      Author: Ryan
 */

#ifndef ACCEL_H_
#define ACCEL_H_

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

// TODO: Get correct Zero G Vout. Current value is theory
#define ACCEL_ZERO_G_VOUT 1.65
// TODO: Calculate correct G resolution. Current value is theory.
#define ACCEL_G_RESOLUTION 0.0066

//*****************************************************************************
//
// Module function prototypes.
//
//*****************************************************************************
extern void accelInit(void);
extern void accelReceive(uint32_t ui32Base, float* fptrForce);

//*****************************************************************************
//
// Mark the end of the C bindings section for C++ compilers.
//
//*****************************************************************************
#ifdef __cplusplus
}
#endif

#endif /* ACCEL_H_ */
