/*
 * misc.c
 *
 *  Created on: Feb 17, 2015
 *      Author: Ryan
 */

#include "driverlib/gpio.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "driverlib/sysctl.h"

#include "inc/hw_gpio.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"

#include "misc.h"
#include "uart.h"
#include "uartstdio.h"

bool consoleEnabled = false;

void
buttonsInit(void) {
  //
  // Enable the GPIO port to which the pushbuttons are connected.
  //
  ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
  //
  // Unlock PF0 so we can change it to a GPIO input
  // Once we have enabled (unlocked) the commit register then re-lock it
  // to prevent further changes.  PF0 is muxed with NMI thus a special case.
  //
  HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK) = GPIO_LOCK_KEY;
  HWREG(GPIO_PORTF_BASE + GPIO_O_CR)  |= 0x01;
  HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK) = 0;

  //
  // Set each of the button GPIO pins as an input with a pull-up.
  //
  ROM_GPIODirModeSet(GPIO_PORTF_BASE, ALL_BUTTONS, GPIO_DIR_MODE_IN);
  MAP_GPIOPadConfigSet(GPIO_PORTF_BASE, ALL_BUTTONS, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);
}
bool
buttonsPressed(uint8_t ui8Buttons) {
  if (~MAP_GPIOPinRead(GPIO_PORTF_BASE, ui8Buttons) & ui8Buttons)
    return true;
  return false;
}
bool
consoleIsEnabled(void) {
  return consoleEnabled;
}
void
consoleInit(void) {
  UARTInit(CONSOLE_BASE);

  //
  // Configure UARTstdio Library
  //
  UARTStdioConfig(0, 115200, MAP_SysCtlClockGet());

  consoleEnabled = true;
}
void
delay(uint32_t ui32ms) { // Delay in milliseconds
  MAP_IntMasterDisable();
  MAP_SysCtlDelay(MAP_SysCtlClockGet() / (3000 / ui32ms));
  MAP_IntMasterEnable();
}
void
FPUInit(void) {
  //
  // Enable the floating-point unit
  //
  MAP_FPUEnable();

  //
  // Configure the floating-point unit to perform lazy stacking of the floating-point state.
  //
  MAP_FPULazyStackingEnable();
}
void
LEDClear(void) {
  LEDOff(WHITE_LED);
}
void
LEDInit(void) {
  //
  // Enable the peripherals used by the on-board LED.
  //
  MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);

  //
  // Enable the GPIO pins for the RGB LED.
  //
  MAP_GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, RED_LED);
  MAP_GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GREEN_LED);
  MAP_GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, BLUE_LED);
}
void
LEDOn(uint8_t ui8Color) {
  MAP_GPIOPinWrite(GPIO_PORTF_BASE, ui8Color, ui8Color);
}
void
LEDOff(uint8_t ui8Color) {
  MAP_GPIOPinWrite(GPIO_PORTF_BASE, ui8Color, 0x0);
}
