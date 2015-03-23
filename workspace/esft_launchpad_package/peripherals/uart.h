/*
 * uart.h
 *
 *  Created on: Feb 17, 2015
 *      Author: Ryan
 */

#ifndef __UART_H__
#define __UART_H__

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
extern void UARTInit(uint32_t ui32Base, uint32_t ui32Baud, uint32_t ui32Config);
extern void UARTIntInit(uint32_t ui32Base, uint32_t ui32IntFlags);

//*****************************************************************************
//
// Mark the end of the C bindings section for C++ compilers.
//
//*****************************************************************************
#ifdef __cplusplus
}
#endif

#endif /* UART_H_ */
