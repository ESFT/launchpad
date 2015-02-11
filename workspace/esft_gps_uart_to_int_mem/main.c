#define TARGET_IS_TM4C123_RA1

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "driverlib/adc.h"
#include "driverlib/debug.h"
#include "driverlib/fpu.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include "flashstore.h"

//*****************************************************************************
//
// The error routine that is called if the driver library encounters an error.
//
//*****************************************************************************
#ifdef DEBUG
void
__error__(char *pcFilename, uint32_t ui32Line)
{
}
#endif


void
LEDInit(void) {
  //
  // Enable the GPIO port that is used for the on-board LED.
  //
  ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);

  //
  // Enable the GPIO pins for the LED (PF2).
  //
  ROM_GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_2);
}

void
LEDOn(void)
{
    ROM_GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, GPIO_PIN_2);
}

void
LEDOff(void)
{
	ROM_GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, 0);
}

void
LEDBlink(void) {
    LEDOn();
    // Delay for 1 millisecond. Each SysCtlDelay is about 3 clocks.
    ROM_SysCtlDelay(ROM_SysCtlClockGet() / (1000 * 3));
    LEDOff();
}

//*****************************************************************************
//
// Send a string to the UART.
//
//*****************************************************************************
void
UARTSend(const uint8_t *pui8Buffer, uint32_t ui32Count)
{
    //
    // Loop while there are more characters to send.
    //
    while(ui32Count--)
    {
        //
        // Write the next character to the UART.
        //
        ROM_UARTCharPut(UART0_BASE, *pui8Buffer++);
    }
}

int
main(void) {
  //
  // Enable lazy stacking for interrupt handlers.  This allows floating-point
  // instructions to be used within interrupt handlers, but at the expense of
  // extra stack usage.
  //
  ROM_FPUEnable();
  ROM_FPULazyStackingEnable();

  //
  // Set the clocking to run directly from the crystal.
  //
  ROM_SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN |
                 SYSCTL_XTAL_16MHZ);

  // Enable the LED
  LEDInit();

  //
  // Enable the peripherals used by this example.
  //
  ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0); // GPS
  ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA); // Console

  //
  // Enable processor interrupts.
  //
  ROM_IntMasterEnable();

  //
  // CONSOLE
  //

  //
  // Set GPIO A0 and A1 as UART pins for console interface
  //
  ROM_GPIOPinConfigure(GPIO_PA0_U0RX);
  ROM_GPIOPinConfigure(GPIO_PA1_U0TX);
  ROM_GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

  //
  // Configure the UART for 115,200, 8-N-1 operation.
  //
  ROM_UARTConfigSetExpClk(UART0_BASE, ROM_SysCtlClockGet(), 115200,
                          (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
                           UART_CONFIG_PAR_NONE));

  //
  // Enable the UART interrupt
  //
  ROM_IntEnable(INT_UART0);
  ROM_UARTIntEnable(UART0_BASE, UART_INT_RX | UART_INT_RT);


  //
  // GPS
  //

  //
  // Enable the peripherals used by this example.
  //
  ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_UART4);
  ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);

  //
  // Set GPIO PC4 as U4RX for GPS interface
  //
  ROM_GPIOPinConfigure(GPIO_PC4_U4RX);
  ROM_GPIOPinTypeUART(GPIO_PORTC_BASE, GPIO_PIN_4);

  //
  // Configure the UART for 9600, 8-N-1 operation.
  //
  ROM_UARTConfigSetExpClk(UART4_BASE, SysCtlClockGet(), 9600,
                          (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
                           UART_CONFIG_PAR_NONE));

  //
  // Loop, writing GPS data to flash while there is free space in flash.
  //

  ///////////////////Initialize//////////////////////////////

  //
  // Initiate Flash Storage.
  //
  uint32_t FreeSpaceAvailable = FlashStoreInit();

  uint8_t command[5] = {'G','P','G','G','A'}, newChar;
  uint32_t gpsBufferIndex, i;
  uint32_t match, noStar; // booleans

  // GPS throws max of 87 chars (TextCRLF) ( 87 chars = 87 bytes )
  // $GPGGA,HHMMSS.SS,DDMM.MMMMM,K,DDDMM.MMMMM,L,N,QQ,PP.P,AAAA.AA,M,+XX.XX,M,SSS,RRRR*CC<CR><LF>
  // Strip *CC<CR><LF>            ( 87 - 5 = 82 chars = 82 bytes )
  // $GPGGA,HHMMSS.SS,DDMM.MMMMM,K,DDDMM.MMMMM,L,N,QQ,PP.P,AAAA.AA,M,+XX.XX,M,SSS,RRRR
  // Add 6 chars for HAM callsign ( 82 + 6 = 88 chars = 88 bytes )
  // $GPGGA,HHMMSS.SS,DDMM.MMMMM,K,DDDMM.MMMMM,L,N,QQ,PP.P,AAAA.AA,M,+XX.XX,M,SSS,RRRRKR0KCT
  // Round to near 4 bytes for flash (88 Bytes = 22 packed uint32_t)
  // Add some padding just in case (128 bytes)
  uint8_t gpsBuffer[128];

  //////////////////////////////////////////////////////////
  while(FreeSpaceAvailable) {
    if (ROM_UARTCharsAvail(UART4_BASE)) { //find out if GPS has sent data

      newChar = ROM_UARTCharGet(UART4_BASE);
      if ( newChar == '$') { // find start of a string of info

        gpsBufferIndex = 0; //start gpsBuffer[] at zero
        gpsBuffer[gpsBufferIndex] = newChar; gpsBufferIndex++; // Add $ as delimiter

        match = 1; // Assume match is true. Check in next loop.

        for (i = 0; i < 5; i++) {
          newChar = ROM_UARTCharGet(UART4_BASE); // collect the next five characters

          if (newChar == command[i]) { // validate init match assumption
            gpsBuffer[gpsBufferIndex] = newChar; gpsBufferIndex++;
          }
          else {
            match = 0; // Assumption was wrong. Break and retry.
            break;
          }
        }

        //if the opening string matched "GPGGA", start processing the data feed
        if (match) {

          noStar = 1; //while asterisk is not found

          while (noStar) {
        	newChar = ROM_UARTCharGet(UART4_BASE); // collect the next five characters
            if (newChar == '*') {
              noStar = 0; //if asterisk is found
            } else {
              gpsBuffer[gpsBufferIndex] = newChar; gpsBufferIndex++;
            }
          }


          // Add Callsign at end of GPS data log
          gpsBuffer[gpsBufferIndex] = 'K'; gpsBufferIndex++;
          gpsBuffer[gpsBufferIndex] = 'R'; gpsBufferIndex++;
          gpsBuffer[gpsBufferIndex] = '0'; gpsBufferIndex++;
          gpsBuffer[gpsBufferIndex] = 'C'; gpsBufferIndex++;
          gpsBuffer[gpsBufferIndex] = 'K'; gpsBufferIndex++;
          gpsBuffer[gpsBufferIndex] = 'T'; gpsBufferIndex++;

          // Write Buffer to Flash
          LEDBlink();
          FreeSpaceAvailable = FlashStoreWriteRecord(&gpsBuffer[0], gpsBufferIndex);
        }
      } //If char == $
    } // main if (Chars avail)
  } // main while end

  uint32_t StartAddr = FLASH_STORE_START_ADDR;
  uint32_t EndAddr = FLASH_STORE_END_ADDR;
  uint32_t CurrAddr = StartAddr;
  uint32_t Header = FLASH_STORE_RECORD_HEADER;
  uint32_t packed_char, recordSize;

  LEDOn();

  while (1)
  {
    packed_char = FlashStoreGetData(CurrAddr);
    if((packed_char & 0xFFFFFF00) == Header) {
      recordSize = packed_char & 0xFF;
      for (i=0; i < (recordSize - 0x04) / 4; i++) {
        CurrAddr += 0x04;
        packed_char = FlashStoreGetData(CurrAddr);
        ROM_UARTCharPut(UART0_BASE, unpack_c0(packed_char));
        ROM_UARTCharPut(UART0_BASE, unpack_c1(packed_char));
        ROM_UARTCharPut(UART0_BASE, unpack_c2(packed_char));
        ROM_UARTCharPut(UART0_BASE, unpack_c3(packed_char));
      }
      ROM_UARTCharPut(UART0_BASE, '\r');
      ROM_UARTCharPut(UART0_BASE, '\n');
    }
    else {
      CurrAddr += 0x04;
    }

    if (CurrAddr >= EndAddr)
    {
      CurrAddr = StartAddr;
      ROM_UARTCharPut(UART0_BASE, '*');
      ROM_UARTCharPut(UART0_BASE, '\r');
      ROM_UARTCharPut(UART0_BASE, '\n');
    }
  }
}
