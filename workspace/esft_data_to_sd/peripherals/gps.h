/*
 * gps.h
 *
 *  Created on: Feb 17, 2015
 *      Author: Ryan
 */

#ifndef GPS_H_
#define GPS_H_

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

//*****************************************************************************
//
// Module function prototypes.
//
//*****************************************************************************
extern void gpsInit(uint32_t ui32Base, uint32_t ui32Baud, uint32_t ui32Config);
extern bool gpsReceive(uint8_t* ui8Buffer);

//*****************************************************************************
//
// Mark the end of the C bindings section for C++ compilers.
//
//*****************************************************************************
#ifdef __cplusplus
}
#endif

#endif /* GPS_H_ */
