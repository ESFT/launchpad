/*
 * gpio.c
 *
 *  Created on: Feb 22, 2015
 *      Author: Ryan
 */

#include "driverlib/gpio.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "driverlib/sysctl.h"

#include "inc/hw_gpio.h"
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"

#include "gpio.h"

void
gpioSysCtlInit(uint32_t ui32Port) {
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
uint32_t
gpioIntInit(uint32_t ui32Port, uint8_t ui8Pins, uint32_t ui32IntType) {
  uint32_t ui32IntFlags = 0;
  MAP_GPIOIntTypeSet(ui32Port, ui8Pins, ui32IntType);

  switch (ui32Port) {
    case GPIO_PORTA_BASE: {
      MAP_IntEnable(INT_GPIOA);
    }
    case GPIO_PORTB_BASE: {
      MAP_IntEnable(INT_GPIOB);
    }
    case GPIO_PORTC_BASE: {
      MAP_IntEnable(INT_GPIOC);
    }
    case GPIO_PORTD_BASE: {
      MAP_IntEnable(INT_GPIOD);
    }
    case GPIO_PORTE_BASE: {
      MAP_IntEnable(INT_GPIOE);
    }
    case GPIO_PORTF_BASE: {
      MAP_IntEnable(INT_GPIOF);
    }
    case GPIO_PORTG_BASE: {
      MAP_IntEnable(INT_GPIOG);
    }
    case GPIO_PORTH_BASE: {
      MAP_IntEnable(INT_GPIOH);
    }
  }

  if (ui8Pins & GPIO_PIN_0) {
    ui32IntFlags |= GPIO_INT_PIN_0;
  }
  if (ui8Pins & GPIO_PIN_1) {
    ui32IntFlags |= GPIO_INT_PIN_1;
  }
  if (ui8Pins & GPIO_PIN_2) {
    ui32IntFlags |= GPIO_INT_PIN_2;
  }
  if (ui8Pins & GPIO_PIN_3) {
    ui32IntFlags |= GPIO_INT_PIN_3;
  }
  if (ui8Pins & GPIO_PIN_4) {
    ui32IntFlags |= GPIO_INT_PIN_4;
  }
  if (ui8Pins & GPIO_PIN_5) {
    ui32IntFlags |= GPIO_INT_PIN_5;
  }
  if (ui8Pins & GPIO_PIN_6) {
    ui32IntFlags |= GPIO_INT_PIN_6;
  }
  if (ui8Pins & GPIO_PIN_7) {
    ui32IntFlags |= GPIO_INT_PIN_7;
  }
  MAP_GPIOIntEnable(ui32Port, ui32IntFlags);
  return ui32IntFlags;
}
