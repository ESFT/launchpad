/*
 * gpio.h
 *
 *  Created on: Feb 22, 2015
 *      Author: Ryan
 */

#ifndef __GPIO_H__
#define __GPIO_H__

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

extern void gpioInputInit(uint32_t ui32Port, uint8_t ui8Pins, uint32_t ui32PinType);
extern void gpioOutputInit(uint32_t ui32Port, uint8_t ui8Pins, uint32_t ui32Strength, uint32_t ui32PinType);
extern void gpioOutputODInit(uint32_t ui32Port, uint8_t ui8Pins, uint32_t ui32Strength, uint32_t ui32PinType);
extern uint32_t gpioIntInit(uint32_t ui32Port, uint8_t ui8Pins, uint32_t ui32IntType);

//*****************************************************************************
//
// Mark the end of the C bindings section for C++ compilers.
//
//*****************************************************************************
#ifdef __cplusplus
}
#endif


#endif /* __GPIO_H__ */
