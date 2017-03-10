/*
 * led.c
 *
 *  Created on: Feb 23, 2017
 *      Author: clausr
 */

#include <stdbool.h>
#include <stdint.h>
#include "inc/hw_memmap.h"

#include "driverlib/gpio.h"
#include "driverlib/rom_map.h"
#include "driverlib/rom.h"
#include "driverlib/sysctl.h"

#include "led.h"


void LEDInit(void) {
  //
  // Enable the peripherals used by the on-board LED.
  //
  MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);

  //
  // Enable the GPIO pins for the RGB LED.
  //
  MAP_GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, WHITE_LED);
}

extern void LEDOn(uint8_t ui8Color);
extern void LEDOff();
