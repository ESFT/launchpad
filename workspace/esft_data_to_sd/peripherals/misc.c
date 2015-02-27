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
#include "inc/hw_memmap.h"
#include "misc.h"
#include "uart.h"
#include "uartstdio.h"

bool consoleEnabled = false;

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
