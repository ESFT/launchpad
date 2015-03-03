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

void gpioSysCtlInit(uint32_t ui32Port) {
  switch (ui32Port) {
    case GPIO_PORTA_BASE: {
      MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
      break;
    }
    case GPIO_PORTB_BASE: {
      MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
      break;
    }
    case GPIO_PORTC_BASE: {
      MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
      break;
    }
    case GPIO_PORTD_BASE: {
      MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
      break;
    }
    case GPIO_PORTE_BASE: {
      MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
      break;
    }
    case GPIO_PORTF_BASE: {
      MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
      break;
    }
  }
  MAP_SysCtlPeripheralEnable(ui32Port);
}
void
gpioInputInit(uint32_t ui32Port, uint8_t ui8Pins, uint32_t ui32PinType) {
  gpioSysCtlInit(ui32Port);
  MAP_GPIOPinTypeGPIOInput(ui32Port, ui8Pins);
  MAP_GPIOPadConfigSet(ui32Port, ui8Pins, GPIO_STRENGTH_2MA, ui32PinType);
}
void
gpioOutputInit(uint32_t ui32Port, uint8_t ui8Pins, uint32_t ui32Strength, uint32_t ui32PinType) {
  gpioSysCtlInit(ui32Port);
  MAP_GPIOPinTypeGPIOOutput(ui32Port, ui8Pins);
  MAP_GPIOPadConfigSet(ui32Port, ui8Pins, ui32Strength, ui32PinType);
}
void
gpioOutputODInit(uint32_t ui32Port, uint8_t ui8Pins, uint32_t ui32Strength, uint32_t ui32PinType) {
  gpioSysCtlInit(ui32Port);
  MAP_GPIOPinTypeGPIOOutputOD(ui32Port, ui8Pins);
  MAP_GPIOPadConfigSet(ui32Port, ui8Pins, ui32Strength, ui32PinType);
}
