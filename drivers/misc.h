/*
 * misc.h
 *
 *  Created on: Feb 17, 2015
 *      Author: Ryan
 */

#ifndef MISC_H_
#define MISC_H_

#include <stdbool.h>
#include <stdint.h>
#include <time.h>

#include "inc/hw_gpio.h"
#include "inc/hw_memmap.h"

#include "driverlib/gpio.h"
#include "driverlib/hibernate.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "driverlib/sysctl.h"

//*****************************************************************************
//
// If building with a C++ compiler, make all of the definitions in this header
// have a C binding.
//
//*****************************************************************************
#ifdef __cplusplus
extern "C" {
#endif

extern void COMPortInit(uint32_t ui32Baud,  void (*pfnHandler)(void), uint32_t ui32IntFlags);
extern void FPUInit(void);
extern void RTCEnable(void);
extern void WaitForButtonPress(uint8_t ui8Pins);
inline bool ButtonsPressed(uint8_t ui8Buttons) {
  return (bool) (~MAP_GPIOPinRead(GPIO_PORTF_BASE, ui8Buttons) & ui8Buttons);
}

//*****************************************************************************
//
// Mark the end of the C bindings section for C++ compilers.
//
//*****************************************************************************
#ifdef __cplusplus
}
#endif

#endif /* MISC_H_ */
