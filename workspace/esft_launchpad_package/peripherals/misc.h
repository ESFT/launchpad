/*
 * misc.h
 *
 *  Created on: Feb 17, 2015
 *      Author: Ryan
 */

#ifndef MISC_H_
#define MISC_H_

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

// LED Colors
#define RED_LED     GPIO_PIN_1
#define BLUE_LED    GPIO_PIN_2
#define GREEN_LED   GPIO_PIN_3
#define YELLOW_LED  RED_LED   | GREEN_LED
#define MAGENTA_LED RED_LED   | BLUE_LED
#define CYAN_LED    GREEN_LED | BLUE_LED
#define WHITE_LED   RED_LED   | GREEN_LED | BLUE_LED

// Console
#define CONSOLE_BASE UART0_BASE

// Buttons
#define NUM_BUTTONS 2
#define SW1_BUTTON  GPIO_PIN_4
#define SW2_BUTTON  GPIO_PIN_0
#define ALL_BUTTONS (SW1_BUTTON | SW2_BUTTON)

extern void buttonsInit(void);
extern uint32_t buttonsPressed(uint8_t ui8Buttons);
extern bool consoleIsEnabled(void);
extern void consoleInit(void);
extern void delay(uint32_t ui32ms);
extern void FPUInit(void);
extern void LEDClear(void);
extern void LEDInit(void);
extern void LEDOn(uint8_t ui8Color);
extern void LEDOff(uint8_t ui8Color);

//*****************************************************************************
//
// Mark the end of the C bindings section for C++ compilers.
//
//*****************************************************************************
#ifdef __cplusplus
}
#endif

#endif /* MISC_H_ */
