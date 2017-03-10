/*
 * accel.h
 *
 *  Created on: Feb 17, 2015
 *      Author: Ryan
 */

#ifndef __ACCEL_250_H__
#define __ACCEL_250_H__

#include <stdbool.h>
#include <stdint.h>

#include "inc/hw_memmap.h"

#include "driverlib/adc.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"

//*****************************************************************************
//
// If building with a C++ compiler, make all of the definitions in this header
// have a C binding.
//
//*****************************************************************************
#ifdef __cplusplus
extern "C" {
#endif

#define ACCEL_250_MAX_VOUT     (3.5)
#define ACCEL_250_ZERO_G_VOUT  (2.5)
#define ACCEL_250_G_RESOLUTION (0.008)

//*****************************************************************************
//
// Module function prototypes.
//
//*****************************************************************************
extern int32_t Accel250DataGetFloat(float *pfData);
extern int32_t Accel250DataGetRaw(uint32_t *pui32Data);
extern void Accel250Init(void);

inline bool Accel250IsDataAvailable(void) {
  return MAP_ADCIntStatus(ADC0_BASE, 0, false);
}

//*****************************************************************************
//
// Mark the end of the C bindings section for C++ compilers.
//
//*****************************************************************************
#ifdef __cplusplus
}
#endif

#endif /* __ACCEL_250_H__ */
