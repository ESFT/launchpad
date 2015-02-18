/*
 * gps.c
 *
 *  Created on: Feb 17, 2015
 *      Author: Ryan
 */

#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include "inc/hw_memmap.h"
#include "gps.h"

void
gpsInit(void) {
  //
  // Enable the peripherals used by the GPS.
  //
  MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_UART4);
  MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);

  //
  // Set GPIO PC4 as U4RX for GPS int32_terface
  //
  MAP_GPIOPinConfigure(GPIO_PC4_U4RX);
  MAP_GPIOPinTypeUART(GPIO_PORTC_BASE, GPIO_PIN_4);

  //
  // Configure the UART for 9600, 8-N-1 operation.
  //
  MAP_UARTConfigSetExpClk(UART4_BASE, MAP_SysCtlClockGet(), 9600,
                          (UART_CONFIG_WLEN_8 | UART_CONFIG_PAR_NONE |
                          UART_CONFIG_STOP_ONE));
}
bool
gpsReceive(uint8_t* ui8Buffer) {
  uint32_t ui32bIndex = 0;
  uint8_t command[5] = {'G','P','G','G','A'}, newChar, i;
  bool match = true; // If a match was found. Assume match is true until proven otherwise

  if (MAP_UARTCharsAvail(UART4_BASE)) { // Find out if GPS has data available

    newChar = MAP_UARTCharGet(UART4_BASE);

    if ( newChar == '$') { // find start of a string of info
      ui8Buffer[ui32bIndex] = newChar; ui32bIndex++; // Add $ as delimiter
      for (i = 0; i < 5; i++) {
        newChar = MAP_UARTCharGet(UART4_BASE); // collect the next five characters
        if (newChar == command[i]) { // validate match assumption
          ui8Buffer[ui32bIndex] = newChar; ui32bIndex++;
        }
        else {
          match = false; // Assumption was wrong. Break and retry.
          break;
        }
      }

      //if the opening string matched "GPGGA", start processing the data feed
      if (match) {
        while (true) {
          newChar = MAP_UARTCharGet(UART4_BASE); // collect the rest of the GPS log
          if (newChar == '*')  break; // If the character is a star, break the loop
          ui8Buffer[ui32bIndex] = newChar; ui32bIndex++;
        }
        ui8Buffer[ui32bIndex] = ','; ui32bIndex++; // Delimit checksum data
        ui8Buffer[ui32bIndex] = MAP_UARTCharGet(UART4_BASE); ui32bIndex++; // Collect 1st checksum number
        ui8Buffer[ui32bIndex] = MAP_UARTCharGet(UART4_BASE); ui32bIndex++; // Collect 2nd checksum number

        // Add null terminator to end of GPS data
        ui8Buffer[ui32bIndex] = '\0';
        return true;
      }
    } //If char == $
  }
  return false;
}
