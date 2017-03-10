/*
 * misc.c
 *
 *  Created on: Feb 17, 2015
 *      Author: Ryan
 */

#include <stdbool.h>
#include <stdint.h>
#include <time.h>

#include "inc/hw_gpio.h"
#include "inc/hw_memmap.h"
#include "inc/hw_nvic.h"
#include "inc/hw_types.h"

#include "driverlib/fpu.h"
#include "driverlib/gpio.h"
#include "driverlib/hibernate.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"

#include "utils/uartstdio.h"

#include "userlib/gpio.h"
#include "userlib/uart.h"

#include "misc.h"

void COMPortInit(uint32_t ui32Baud,  void (*pfnHandler)(void), uint32_t ui32IntFlags) {
  UARTInit(UART0_BASE, ui32Baud, false, (UART_CONFIG_PAR_NONE | UART_CONFIG_STOP_ONE | UART_CONFIG_WLEN_8));

  //
  // Configure UARTstdio Library
  //
  UARTStdioConfig(0, ui32Baud, MAP_SysCtlClockGet());

  if (pfnHandler) UARTIntInit(UART0_BASE, ui32IntFlags, -1, pfnHandler);
}

void FPUInit(void) {
  //
  // Enable the floating-point unit
  //
  MAP_FPUEnable();

  //
  // Configure the floating-point unit to perform lazy stacking of the floating-point state.
  //
  MAP_FPULazyStackingEnable();
}

void RTCEnable(void) {
  MAP_HibernateEnableExpClk(MAP_SysCtlClockGet());
  MAP_HibernateRTCEnable();
}

void WaitForButtonPress(uint8_t ui8Pins) {
  while (!ButtonsPressed(ui8Pins)) {
  };
  while (ButtonsPressed(ui8Pins)) {
  };
}
