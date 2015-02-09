#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_i2c.h"
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/adc.h"
#include "driverlib/debug.h"
#include "driverlib/fpu.h"
#include "driverlib/gpio.h"
#include "driverlib/i2c.h"
#include "driverlib/interrupt.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include "flashstore.h"
#include "altimetercommands.c"

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

//*****************************************************************************
//
// LED functions
//
//*****************************************************************************
void
LEDOn(void)
{
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, GPIO_PIN_2);
}

void
LEDOff(void)
{
	GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, 0);
}

void
LEDBlink(void) {
    LEDOn();
    // Delay for 1 millisecond. Each SysCtlDelay is about 3 clocks.
    SysCtlDelay(SysCtlClockGet() / (1000 * 3));
    LEDOff();
}

//*****************************************************************************
//
// Initialize Peripherals
//
//*****************************************************************************
void
LEDInit(void) {
  //
  // Enable the GPIO port that is used for the on-board LED.
  //
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);

  //
  // Enable the GPIO pins for the LED (PF2).
  //
  GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_2);
}

void
consoleInit(void) {
  //
  // Enable the peripherals used by the console.
  //
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
  
  //
  // Set GPIO A0 and A1 as UART pins for console interface
  //
  GPIOPinConfigure(GPIO_PA0_U0RX);
  GPIOPinConfigure(GPIO_PA1_U0TX);
  GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

  //
  // Configure the UART for 115,200, 8-N-1 operation.
  //
  UARTConfigSetExpClk(UART0_BASE, SysCtlClockGet(), 115200,
                      (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
                       UART_CONFIG_PAR_NONE));
					   
  //
  // Enable the UART interrupt
  //
  IntEnable(INT_UART0);
  UARTIntEnable(UART0_BASE, UART_INT_RX | UART_INT_RT);
  
}

void
GPSInit(void) {
  //
  // Enable the peripherals used by the GPS.
  //
  SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
  
  //
  // Enable the peripherals used by this example.
  //
  SysCtlPeripheralEnable(SYSCTL_PERIPH_UART4);
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);

  //
  // Set GPIO PC4 as U4RX for GPS interface
  //
  GPIOPinConfigure(GPIO_PC4_U4RX);
  GPIOPinTypeUART(GPIO_PORTC_BASE, GPIO_PIN_4);

  //
  // Configure the UART for 9600, 8-N-1 operation.
  //
  UARTConfigSetExpClk(UART4_BASE, SysCtlClockGet(), 9600,
  (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
  UART_CONFIG_PAR_NONE));
  
}

void
accelInit(void) {
  //
  // Configure ADC0 Port E Pin 3 on sequence 3
  //
  SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
  GPIOPinTypeADC(GPIO_PORTE_BASE, GPIO_PIN_3);
  ADCSequenceDisable(ADC0_BASE, 3);
  ADCSequenceConfigure(ADC0_BASE, 3, ADC_TRIGGER_PROCESSOR, 0);
  ADCSequenceStepConfigure(ADC0_BASE, 3, 0, ADC_CTL_CH0 | ADC_CTL_IE | ADC_CTL_END);
  ADCSequenceEnable(ADC0_BASE, 3);
  ADCIntEnable(ADC0_BASE, 3);
  ADCIntClear(ADC0_BASE, 3);

}

void
altInit(void) {
  //
  // Configure I2C0 Port B SCL Pin 2 SDA Pin 3
  //
  SysCtlPeripheralEnable(SYSCTL_PERIPH_I2C0);
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
  
  GPIOPinConfigure(GPIO_PB2_I2C0SCL);
  GPIOPinConfigure(GPIO_PB3_I2C0SDA);
  
  GPIOPinTypeI2CSCL(GPIO_PORTB_BASE, GPIO_PIN_2);
  GPIOPinTypeI2C(GPIO_PORTB_BASE, GPIO_PIN_3);
  
  //false = 100kbps, true = 400kbps
  I2CMasterInitExpClk(I2C0_BASE, SysCtlClockGet(), false);
  
  I2CMasterEnable(I2C0_BASE);
  
  //false = writing to slave, true = reading from slave
  I2CMasterSlaveAddrSet(I2C0_BASE, ALT_ADDRESS, false);
  
  // reset altimeter
 // I2CMasterDataPut(I2C0_BASE, ALT_RESET); //place data to be sent in register
 // I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_SINGLE_SEND); //send command
 // while(I2CMasterBusy(I2C0_BASE)) {} //wait until slave gets command
  
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
        UARTCharPut(UART0_BASE, *pui8Buffer++);
    }
}

//*****************************************************************************
//
// Main function
//
//*****************************************************************************
int
main(void) {
  //
  // Enable lazy stacking for interrupt handlers.
  //
  FPUEnable();
  FPULazyStackingEnable();

  //
  // Set the clocking to run directly from the crystal.
  //
  SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN |
                 SYSCTL_XTAL_16MHZ);

  // Enable the LED
  LEDInit();
  
  // Enable the Console
  consoleInit();

  // Enable the GPS
  GPSInit();
  
  // Enable the accelerometer
  accelInit();
  
  //
  altInit();

  // Enable processor interrupts.
  IntMasterEnable();




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
  
  // accelerometer
  char accelBuffer[16];
  uint32_t accel[1];

  // altimeter
  char altBuffer[16];
  int altData;
  
  //////////////////////////////////////////////////////////
  while(FreeSpaceAvailable) {
    if (UARTCharsAvail(UART4_BASE)) { //find out if GPS has sent data

      newChar = UARTCharGet(UART4_BASE);
      if ( newChar == '$') { // find start of a string of info

        gpsBufferIndex = 0; //start gpsBuffer[] at zero
        gpsBuffer[gpsBufferIndex] = newChar; gpsBufferIndex++; // Add $ as delimiter

        match = 1; // Assume match is true. Check in next loop.

        for (i = 0; i < 5; i++) {
          newChar = UARTCharGet(UART4_BASE); // collect the next five characters

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
        	newChar = UARTCharGet(UART4_BASE); // collect the next five characters
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

    //
    // Get data from accelerometer
    //
    ADCProcessorTrigger(ADC0_BASE, 3);
    while(!ADCIntStatus(ADC0_BASE, 3, false)) {} // wait for a2d conversion
        // if stuck in while() unplug lauchpad for 15s to reset
    ADCIntClear(ADC0_BASE, 3);
    ADCSequenceDataGet(ADC0_BASE, 3, accel);

    // send accelerometer data over UART for debugging
    // TODO: save accel data to flash
    sprintf(accelBuffer, "accel : %d  \n\r", accel[0]);
    UARTSend((uint8_t *)accelBuffer,16);
    
    //
    // Get data from altimeter
    // TODO: fix code to read from altimeter
    //
    I2CMasterSlaveAddrSet(I2C0_BASE, ALT_ADDRESS, false);
    I2CMasterDataPut(I2C0_BASE, ALT_D1_256);
    I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_SINGLE_SEND);
    while(!( ROM_I2CMasterBusy(I2C0_BASE) )){}
    while( ROM_I2CMasterBusy(I2C0_BASE) ){}
    
    I2CMasterDataPut(I2C0_BASE, ALT_ADC_READ);
    I2CMasterSlaveAddrSet(I2C0_BASE, ALT_ADDRESS, true);
    I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_SINGLE_RECEIVE);
    while(!( ROM_I2CMasterBusy(I2C0_BASE) )){}
    while(I2CMasterBusy(I2C0_BASE)) {}
    altData = I2CSlaveDataGet(I2C0_BASE); // TODO: returns 0, not retrieving data from altimeter
    
    // send altimeter data over UART for debugging
    // TODO: save alt data to flash
    sprintf(altBuffer, "alt : %d  \n\r", altData);
    UARTSend((uint8_t *)altBuffer,16);   
    
    LEDBlink(); // blink led to verify code is running for debugging without flash write
    

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
        UARTCharPut(UART0_BASE, unpack_c0(packed_char));
        UARTCharPut(UART0_BASE, unpack_c1(packed_char));
        UARTCharPut(UART0_BASE, unpack_c2(packed_char));
        UARTCharPut(UART0_BASE, unpack_c3(packed_char));
      }
      UARTCharPut(UART0_BASE, '\r');
      UARTCharPut(UART0_BASE, '\n');
    }
    else {
      CurrAddr += 0x04;
    }

    if (CurrAddr >= EndAddr)
    {
      CurrAddr = StartAddr;
      UARTCharPut(UART0_BASE, '*');
      UARTCharPut(UART0_BASE, '\r');
      UARTCharPut(UART0_BASE, '\n');
    }
  }
}
