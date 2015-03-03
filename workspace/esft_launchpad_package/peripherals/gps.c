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
#include "gpio.h"
#include "gps.h"
#include "uart.h"

uint32_t gps_ui32Base;
uint32_t gps_ui32SenseBase;
uint8_t gps_ui8SensePin;

void
gpsInit(uint32_t ui32Base, uint32_t ui32Baud, uint32_t ui32Config, uint32_t ui32SenseBase, uint8_t ui8SensePin) {
  gps_ui32Base = ui32Base;
  gps_ui32SenseBase = ui32SenseBase;
  gps_ui8SensePin = ui8SensePin;

  UARTInit(gps_ui32Base);
  gpioInputInit(gps_ui32SenseBase, gps_ui8SensePin, GPIO_STRENGTH_4MA, GPIO_PIN_TYPE_STD);
  MAP_UARTConfigSetExpClk(ui32Base, MAP_SysCtlClockGet(), ui32Baud, ui32Config);
}
bool
gpsReceive(uint8_t* ui8Buffer) {
  uint32_t ui32bIndex = 0;
  uint8_t command[5] = {'G','P','G','G','A'}, newChar, i;
  bool match = true; // If a match was found. Assume match is true until proven otherwise

  if (MAP_GPIOPinRead(gps_ui32SenseBase, gps_ui8SensePin) != gps_ui8SensePin) return false;  // No Nav Lock
  if (MAP_UARTCharsAvail(gps_ui32Base)) { // Find out if GPS has data available

    newChar = MAP_UARTCharGet(gps_ui32Base);

    if ( newChar == '$') { // find start of a string of info
      ui8Buffer[ui32bIndex] = newChar; ui32bIndex++; // Add $ as delimiter
      for (i = 0; i < 5; i++) {
        newChar = MAP_UARTCharGet(gps_ui32Base); // collect the next five characters
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
          newChar = MAP_UARTCharGet(gps_ui32Base); // collect the rest of the GPS log
          if (newChar == '*')  break; // If the character is a star, break the loop
          ui8Buffer[ui32bIndex] = newChar; ui32bIndex++;
        }
        ui8Buffer[ui32bIndex] = ','; ui32bIndex++; // Delimit checksum data
        ui8Buffer[ui32bIndex] = MAP_UARTCharGet(gps_ui32Base); ui32bIndex++; // Collect 1st checksum number
        ui8Buffer[ui32bIndex] = MAP_UARTCharGet(gps_ui32Base); ui32bIndex++; // Collect 2nd checksum number

        // Add null terminator to end of GPS data
        ui8Buffer[ui32bIndex] = '\0';
        return true;
      }
    } //If char == $
  }
  return false;
}
