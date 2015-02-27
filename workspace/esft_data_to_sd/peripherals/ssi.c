/*
 * ssi.c
 *
 *  Created on: Feb 17, 2015
 *      Author: Ryan
 */

#include "driverlib/gpio.h"
#include "driverlib/ssi.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "driverlib/sysctl.h"
#include "inc/hw_gpio.h"
#include "inc/hw_memmap.h"
#include "inc/hw_ssi.h"
#include "inc/hw_types.h"

#include "ssi.h"

void
SSIInit(uint32_t ui32Base, uint32_t ui32Protocol, uint32_t ui32Mode, uint32_t ui32BitRate, uint32_t ui32DataWidth) {
  switch (ui32Base) {
    case SSI0_BASE: {
      MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_SSI0);
      MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

      //
      // Enable pin PA2 for SSI0 SSI0CLK
      //
      MAP_GPIOPinConfigure(GPIO_PA2_SSI0CLK);
      MAP_GPIOPinTypeSSI(GPIO_PORTA_BASE, GPIO_PIN_2);

      //
      // Enable pin PA4 for SSI0 SSI0RX
      //
      MAP_GPIOPinConfigure(GPIO_PA4_SSI0RX);
      MAP_GPIOPinTypeSSI(GPIO_PORTA_BASE, GPIO_PIN_4);

      //
      // Enable pin PA5 for SSI0 SSI0TX
      //
      MAP_GPIOPinConfigure(GPIO_PA5_SSI0TX);
      MAP_GPIOPinTypeSSI(GPIO_PORTA_BASE, GPIO_PIN_5);

      //
      // Enable pin PA3 for SSI0 SSI0FSS
      //
      MAP_GPIOPinConfigure(GPIO_PA3_SSI0FSS);
      MAP_GPIOPinTypeSSI(GPIO_PORTA_BASE, GPIO_PIN_3);
    }
    case SSI1_BASE: {
      MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_SSI1);
      MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);

      //
      // Enable pin PF1 for SSI1 SSI1TX
      //
      MAP_GPIOPinConfigure(GPIO_PF1_SSI1TX);
      MAP_GPIOPinTypeSSI(GPIO_PORTF_BASE, GPIO_PIN_1);

      //
      // Enable pin PF0 for SSI1 SSI1RX
      // First open the lock and select the bits we want to modify in the GPIO commit register.
      //
      HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK) = GPIO_LOCK_KEY;
      HWREG(GPIO_PORTF_BASE + GPIO_O_CR) = 0x1;

      //
      // Now modify the configuration of the pins that we unlocked.
      //
      MAP_GPIOPinConfigure(GPIO_PF0_SSI1RX);
      MAP_GPIOPinTypeSSI(GPIO_PORTF_BASE, GPIO_PIN_0);

      //
      // Enable pin PF3 for SSI1 SSI1FSS
      //
      MAP_GPIOPinConfigure(GPIO_PF3_SSI1FSS);
      MAP_GPIOPinTypeSSI(GPIO_PORTF_BASE, GPIO_PIN_3);

      //
      // Enable pin PF2 for SSI1 SSI1CLK
      //
      MAP_GPIOPinConfigure(GPIO_PF2_SSI1CLK);
      MAP_GPIOPinTypeSSI(GPIO_PORTF_BASE, GPIO_PIN_2);
    }
    case SSI2_BASE: {
      MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_SSI2);
      MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);

      //
      // Enable pin PB4 for SSI2 SSI2CLK
      //
      MAP_GPIOPinConfigure(GPIO_PB4_SSI2CLK);
      MAP_GPIOPinTypeSSI(GPIO_PORTB_BASE, GPIO_PIN_4);

      //
      // Enable pin PB5 for SSI2 SSI2FSS
      //
      MAP_GPIOPinConfigure(GPIO_PB5_SSI2FSS);
      MAP_GPIOPinTypeSSI(GPIO_PORTB_BASE, GPIO_PIN_5);

      //
      // Enable pin PB6 for SSI2 SSI2RX
      //
      MAP_GPIOPinConfigure(GPIO_PB6_SSI2RX);
      MAP_GPIOPinTypeSSI(GPIO_PORTB_BASE, GPIO_PIN_6);

      //
      // Enable pin PB7 for SSI2 SSI2TX
      //
      MAP_GPIOPinConfigure(GPIO_PB7_SSI2TX);
      MAP_GPIOPinTypeSSI(GPIO_PORTB_BASE, GPIO_PIN_7);
    }
    case SSI3_BASE: {
      MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_SSI3);
      MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);

      //
      // Enable pin PD2 for SSI3 SSI3RX
      //
      MAP_GPIOPinConfigure(GPIO_PD2_SSI3RX);
      MAP_GPIOPinTypeSSI(GPIO_PORTD_BASE, GPIO_PIN_2);

      //
      // Enable pin PD3 for SSI3 SSI3TX
      //
      MAP_GPIOPinConfigure(GPIO_PD3_SSI3TX);
      MAP_GPIOPinTypeSSI(GPIO_PORTD_BASE, GPIO_PIN_3);

      //
      // Enable pin PD1 for SSI3 SSI3FSS
      //
      MAP_GPIOPinConfigure(GPIO_PD1_SSI3FSS);
      MAP_GPIOPinTypeSSI(GPIO_PORTD_BASE, GPIO_PIN_1);

      //
      // Enable pin PD0 for SSI3 SSI3CLK
      //
      MAP_GPIOPinConfigure(GPIO_PD0_SSI3CLK);
      MAP_GPIOPinTypeSSI(GPIO_PORTD_BASE, GPIO_PIN_0);
    }
  }

  //
  // Enable the supplied SSI Base Clock
  //
  MAP_SSIConfigSetExpClk(ui32Base, MAP_SysCtlClockGet(), ui32Protocol,
                         ui32Mode, ui32BitRate, ui32DataWidth);

  //
  // Enable supplied SSI Base Master Block
  //
  MAP_SSIEnable(ui32Base);
}
