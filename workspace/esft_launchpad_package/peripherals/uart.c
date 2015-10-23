/*
 * uart.c
 *
 *  Created on: Feb 17, 2015
 *      Author: Ryan
 */

#include "uart.h"

#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/uart.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "driverlib/sysctl.h"

#include "inc/hw_gpio.h"
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_uart.h"

void
UARTInit(uint32_t ui32Base, uint32_t ui32Baud, uint32_t ui32Config) {
  switch (ui32Base) {
    case UART0_BASE: {
      //
      // Enable Peripheral Clocks
      //
      MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
      MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

      //
      // Enable pin PA1 for UART0 U0TX
      //
      MAP_GPIOPinConfigure(GPIO_PA1_U0TX);
      MAP_GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_1);

      //
      // Enable pin PA0 for UART0 U0RX
      //
      MAP_GPIOPinConfigure(GPIO_PA0_U0RX);
      MAP_GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0);

      break;
    }
    case UART1_BASE: {
      //
      // Enable Peripheral Clocks
      //
      MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_UART1);
      MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
      MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);

      //
      // Enable pin PF1 for UART1 U1CTS
      //
      MAP_GPIOPinConfigure(GPIO_PF1_U1CTS);
      MAP_GPIOPinTypeUART(GPIO_PORTF_BASE, GPIO_PIN_1);

      //
      // Enable pin PB1 for UART1 U1TX
      //
      MAP_GPIOPinConfigure(GPIO_PB1_U1TX);
      MAP_GPIOPinTypeUART(GPIO_PORTB_BASE, GPIO_PIN_1);

      //
      // Enable pin PB0 for UART1 U1RX
      //
      MAP_GPIOPinConfigure(GPIO_PB0_U1RX);
      MAP_GPIOPinTypeUART(GPIO_PORTB_BASE, GPIO_PIN_0);

      //
      // Enable pin PF0 for UART1 U1RTS
      // First open the lock and select the bits we want to modify in the GPIO commit register.
      //
      HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK) = GPIO_LOCK_KEY;
      HWREG(GPIO_PORTF_BASE + GPIO_O_CR) = 0x1;

      //
      // Now modify the configuration of the pins that we unlocked.
      //
      MAP_GPIOPinConfigure(GPIO_PF0_U1RTS);
      MAP_GPIOPinTypeUART(GPIO_PORTF_BASE, GPIO_PIN_0);

      break;
    }
    case UART2_BASE: {
      //
      // Enable Peripheral Clocks
      //
      MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_UART2);
      MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);

      //
      // Enable pin PD7 for UART2 U2TX
      // First open the lock and select the bits we want to modify in the GPIO commit register.
      //
      HWREG(GPIO_PORTD_BASE + GPIO_O_LOCK) = GPIO_LOCK_KEY;
      HWREG(GPIO_PORTD_BASE + GPIO_O_CR) = 0x80;

      //
      // Now modify the configuration of the pins that we unlocked.
      //
      MAP_GPIOPinConfigure(GPIO_PD7_U2TX);
      MAP_GPIOPinTypeUART(GPIO_PORTD_BASE, GPIO_PIN_7);

      //
      // Enable pin PD6 for UART2 U2RX
      //
      MAP_GPIOPinConfigure(GPIO_PD6_U2RX);
      MAP_GPIOPinTypeUART(GPIO_PORTD_BASE, GPIO_PIN_6);

      break;
    }
    case UART3_BASE: {
      //
      // Enable Peripheral Clocks
      //
      MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_UART3);
      MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);

      //
      // Enable pin PC7 for UART3 U3TX
      //
      MAP_GPIOPinConfigure(GPIO_PC7_U3TX);
      MAP_GPIOPinTypeUART(GPIO_PORTC_BASE, GPIO_PIN_7);

      //
      // Enable pin PC6 for UART3 U3RX
      //
      MAP_GPIOPinConfigure(GPIO_PC6_U3RX);
      MAP_GPIOPinTypeUART(GPIO_PORTC_BASE, GPIO_PIN_6);
      break;
    }
    case UART4_BASE: {
      //
      // Enable Peripheral Clocks
      //
      MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_UART4);
      MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);

      //
      // Enable pin PC4 for UART4 U4RX
      //
      MAP_GPIOPinConfigure(GPIO_PC4_U4RX);
      MAP_GPIOPinTypeUART(GPIO_PORTC_BASE, GPIO_PIN_4);

      //
      // Enable pin PC5 for UART4 U4TX
      //
      MAP_GPIOPinConfigure(GPIO_PC5_U4TX);
      MAP_GPIOPinTypeUART(GPIO_PORTC_BASE, GPIO_PIN_5);

      break;
    }
    case UART5_BASE: {
      //
      // Enable Peripheral Clocks
      //
      MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_UART5);
      MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);

      //
      // Enable pin PE4 for UART5 U5RX
      //
      MAP_GPIOPinConfigure(GPIO_PE4_U5RX);
      MAP_GPIOPinTypeUART(GPIO_PORTE_BASE, GPIO_PIN_4);

      //
      // Enable pin PE5 for UART5 U5TX
      //
      MAP_GPIOPinConfigure(GPIO_PE5_U5TX);
      MAP_GPIOPinTypeUART(GPIO_PORTE_BASE, GPIO_PIN_5);

      break;
    }
    case UART6_BASE: {
      //
      // Enable Peripheral Clocks
      //
      MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_UART6);
      MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);

      //
      // Enable pin PD5 for UART6 U6TX
      //
      MAP_GPIOPinConfigure(GPIO_PD5_U6TX);
      MAP_GPIOPinTypeUART(GPIO_PORTD_BASE, GPIO_PIN_5);

      //
      // Enable pin PD4 for UART6 U6RX
      //
      MAP_GPIOPinConfigure(GPIO_PD4_U6RX);
      MAP_GPIOPinTypeUART(GPIO_PORTD_BASE, GPIO_PIN_4);

      break;
    }
    case UART7_BASE: {
      //
      // Enable Peripheral Clocks
      //
      MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_UART7);
      MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);

      //
      // Enable pin PE0 for UART7 U7RX
      //
      MAP_GPIOPinConfigure(GPIO_PE0_U7RX);
      MAP_GPIOPinTypeUART(GPIO_PORTE_BASE, GPIO_PIN_0);

      //
      // Enable pin PE1 for UART7 U7TX
      //
      MAP_GPIOPinConfigure(GPIO_PE1_U7TX);
      MAP_GPIOPinTypeUART(GPIO_PORTE_BASE, GPIO_PIN_1);
      break;
    }
  }
  MAP_UARTConfigSetExpClk(ui32Base, MAP_SysCtlClockGet(), ui32Baud, ui32Config);
}
void
UARTIntInit(uint32_t ui32Base, uint32_t ui32IntFlags) {
  switch (ui32Base) {
    case UART0_BASE: {
      MAP_IntEnable(INT_UART0);
      break;
    }
    case UART1_BASE: {
      MAP_IntEnable(INT_UART1);
      break;
    }
    case UART2_BASE: {
      MAP_IntEnable(INT_UART2);
      break;
    }
    case UART3_BASE: {
      MAP_IntEnable(INT_UART3);
      break;
    }
    case UART4_BASE: {
      MAP_IntEnable(INT_UART4);
      break;
    }
    case UART5_BASE: {
      MAP_IntEnable(INT_UART5);
      break;
    }
    case UART6_BASE: {
      MAP_IntEnable(INT_UART6);
      break;
    }
    case UART7_BASE: {
      MAP_IntEnable(INT_UART7);
      break;
    }
  }
  MAP_UARTIntEnable(ui32Base, ui32IntFlags);
}
