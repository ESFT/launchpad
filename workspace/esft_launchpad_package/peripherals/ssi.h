/*
 * ssi.h
 *
 *  Created on: Feb 17, 2015
 *      Author: Ryan
 */

#ifndef SSI_H_
#define SSI_H_

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
// Module function prototypes.
//
//*****************************************************************************
void SSIInit(uint32_t ui32Base, uint32_t ui32Protocol, uint32_t ui32Mode, uint32_t ui32BitRate, uint32_t ui32DataWidth);

//*****************************************************************************
//
// Mark the end of the C bindings section for C++ compilers.
//
//*****************************************************************************
#ifdef __cplusplus
}
#endif

#endif /* SSI_H_ */
