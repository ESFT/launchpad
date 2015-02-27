/*
 * gpio.c
 *
 *  Created on: Feb 22, 2015
 *      Author: Ryan
 */


#include "driverlib/gpio.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "inc/hw_gpio.h"

#include "gpio.h"

void
gpioInputInit(uint32_t ui32Port, uint8_t ui8Pins, uint32_t ui32Strength, uint32_t ui32PinType) {
  MAP_SysCtlPeripheralEnable(ui32Port);
  MAP_GPIOPinTypeGPIOInput(ui32Port, ui8Pins);
  MAP_GPIOPadConfigSet(ui32Port, ui8Pins, ui32Strength, ui32PinType);
}
void
gpioOutputInit(uint32_t ui32Port, uint8_t ui8Pins, uint32_t ui32Strength, uint32_t ui32PinType) {
  MAP_SysCtlPeripheralEnable(ui32Port);
  MAP_GPIOPinTypeGPIOOutput(ui32Port, ui8Pins);
  MAP_GPIOPadConfigSet(ui32Port, ui8Pins, ui32Strength, ui32PinType);
}
void
gpioOutputODInit(uint32_t ui32Port, uint8_t ui8Pins, uint32_t ui32Strength, uint32_t ui32PinType) {
  MAP_SysCtlPeripheralEnable(ui32Port);
  MAP_GPIOPinTypeGPIOOutputOD(ui32Port, ui8Pins);
  MAP_GPIOPadConfigSet(ui32Port, ui8Pins, ui32Strength, ui32PinType);
}
