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

#define GPS_BAUD 9600
#define GPS_UART_CONFIG UART_CONFIG_WLEN_8 | UART_CONFIG_PAR_NONE | UART_CONFIG_STOP_ONE

//*****************************************************************************
//
// Module function prototypes.
//
//*****************************************************************************
extern void gpsInit(uint32_t ui32Base, uint32_t ui32Baud, uint32_t ui32Config, uint32_t ui32SenseBase, uint8_t ui8SensePin);
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
