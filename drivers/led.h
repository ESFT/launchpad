/*
 * led.h
 *
 *  Created on: Feb 23, 2017
 *      Author: clausr
 */

#ifndef __LED_H__
#define __LED_H__

#include <stdbool.h>
#include <stdint.h>
#include "inc/hw_memmap.h"

// LED Colors
#define RED_LED     GPIO_PIN_1
#define BLUE_LED    GPIO_PIN_2
#define GREEN_LED   GPIO_PIN_3
#define YELLOW_LED  RED_LED   | GREEN_LED
#define MAGENTA_LED RED_LED   | BLUE_LED
#define CYAN_LED    GREEN_LED | BLUE_LED
#define WHITE_LED   RED_LED   | GREEN_LED | BLUE_LED

extern void LEDInit(void);

inline void LEDOn(uint8_t ui8Color) {
  MAP_GPIOPinWrite(GPIO_PORTF_BASE, WHITE_LED, ui8Color);
}

inline void LEDOff() {
  MAP_GPIOPinWrite(GPIO_PORTF_BASE, WHITE_LED, 0x00);
}

#endif /* LED_H_ */
