/*
 * status.c
 *
 *  Created on: Feb 17, 2015
 *      Author: Ryan
 */

#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/timer.h"

#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"

#include "misc.h"
#include "status.h"

static StatusCode_t statusCode = INITIALIZING; // Current Status Code
static StatusCode_t statusCodeDefault = INITIALIZING; // Default state if none are set
static bool         statusBusy = false;        // Status code system is busy

bool
setStatus(StatusCode_t scStatus) { // Set status code
  if (!statusBusy) {
    statusCode = scStatus;
    return true;
  }
  return false;
}
void
setStatusDefault(StatusCode_t scStatus) { // Set default status code
  statusCodeDefault = scStatus;
}
void
statusCodeInterruptInit(void) {

  LEDInit();

  //
  // Enable the perifpherals used by the timer interrupts
  //
  MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);

  //
  // Enable processor interrupts.
  //
  MAP_IntMasterEnable();

  //
  // Configure the 32-bit periodic timer for the status code length define.
  //
  MAP_TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);
  MAP_TimerLoadSet(TIMER0_BASE, TIMER_A, MAP_SysCtlClockGet() / (1000 / STATUS_CODE_LENGTH));

  //
  // Setup the interrupts for the timer timeouts.
  //
  MAP_IntEnable(INT_TIMER0A);
  MAP_TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);

  //
  // Enable the timer.
  //
  MAP_TimerEnable(TIMER0_BASE, TIMER_A);
}
void
statusIntHandler(void) { // Timer interrupt to handle status codes
  static uint8_t  statusColor;          // Status Code Color
  static uint32_t statusBlinkDelay[3];  // Status Code Delays
  static uint8_t  statusBeepIndex = 0;  // Index of beep LED
  static uint32_t statusDelayIndex = 0; // Index of beep length
  static bool     statusLEDOn = false;  // Status of the LED

  //
  // Disable interrupts to prevent loops
  //
  MAP_IntDisable(INT_TIMER0A);

  //
  // Clear the timer interrupt
  //
  ROM_TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);

  if (!statusBusy) { // If a status code has already been initiated, do not overwrite
    statusBusy = true; // Set the busy state for status codes

    //
    // set color and delay
    //
    switch (statusCode) {
      case INITIALIZING: { // device is initializing
        statusColor = WHITE_LED;
        statusBlinkDelay[0] = STATUS_DOT;
        statusBlinkDelay[1] = STATUS_DOT;
        statusBlinkDelay[2] = STATUS_DOT;
        break;
      }
      case INITIALIZING_HOLD: { // device is initializing hold
        statusColor = WHITE_LED;
        statusBlinkDelay[0] = STATUS_DASH;
        statusBlinkDelay[1] = STATUS_DASH;
        statusBlinkDelay[2] = STATUS_DASH;
        break;
      }
      case RUNNING: { // code is running
        statusColor = GREEN_LED;
        statusBlinkDelay[0] = STATUS_DOT;
        statusBlinkDelay[1] = STATUS_DOT;
        statusBlinkDelay[2] = STATUS_DOT;
        break;
      }
      case OUT_OF_FLASH_HOLD: { // Out of flash memory hold
        statusColor = BLUE_LED;
        statusBlinkDelay[0] = STATUS_DASH;
        statusBlinkDelay[1] = STATUS_DASH;
        statusBlinkDelay[2] = STATUS_DASH;
        break;
      }
      case OUT_OF_FLASH: { // out of flash memory
        statusColor = BLUE_LED;
        statusBlinkDelay[0] = STATUS_DOT;
        statusBlinkDelay[1] = STATUS_DOT;
        statusBlinkDelay[2] = STATUS_DOT;
        break;
      }
      case DRL_ERR: { // driver library encountered an error
        statusColor = RED_LED;
        statusBlinkDelay[0] = STATUS_DOT;
        statusBlinkDelay[1] = STATUS_DOT;
        statusBlinkDelay[2] = STATUS_DOT;
        break;
      }
      case BUS_FAULT: { // Bus fault
        statusColor = RED_LED;
        statusBlinkDelay[0] = STATUS_DOT;
        statusBlinkDelay[1] = STATUS_DOT;
        statusBlinkDelay[2] = STATUS_DASH;
        break;
      }
      case USAGE_FAULT: { // Usage fault
        statusColor = RED_LED;
        statusBlinkDelay[0] = STATUS_DOT;
        statusBlinkDelay[1] = STATUS_DASH;
        statusBlinkDelay[2] = STATUS_DOT;
        break;
      }
      case ACC250_ADC_CONV_ERR: { // 250G Accelerometer ADC conversion error
        statusColor = RED_LED;
        statusBlinkDelay[0] = STATUS_DOT;
        statusBlinkDelay[1] = STATUS_DOT;
        statusBlinkDelay[2] = STATUS_DOT;
        break;
      }
      case ALT_RESET_ERR: { // Altimeter failed to reset
        statusColor = RED_LED;
        statusBlinkDelay[0] = STATUS_DOT;
        statusBlinkDelay[1] = STATUS_DASH;
        statusBlinkDelay[2] = STATUS_DASH;
        break;
      }
      case ALT_PROM_ERR: { // Altimeter PROM read failed
        statusColor = RED_LED;
        statusBlinkDelay[0] = STATUS_DASH;
        statusBlinkDelay[1] = STATUS_DOT;
        statusBlinkDelay[2] = STATUS_DOT;
        break;
      }
      case ALT_CRC_ERR: { // Altimeter CRC check failed
        statusColor = RED_LED;
        statusBlinkDelay[0] = STATUS_DASH;
        statusBlinkDelay[1] = STATUS_DOT;
        statusBlinkDelay[2] = STATUS_DASH;
        break;
      }
      case ALT_ADC_CONV_ERR: { // Altimeter data retrieve failed
        statusColor = YELLOW_LED;
        statusBlinkDelay[0] = STATUS_DOT;
        statusBlinkDelay[1] = STATUS_DOT;
        statusBlinkDelay[2] = STATUS_DASH;
        break;
      }
      case GYRO_STARTUP_ERR: { // Gyro failed to initialize
        statusColor = YELLOW_LED;
        statusBlinkDelay[0] = STATUS_DOT;
        statusBlinkDelay[1] = STATUS_DASH;
        statusBlinkDelay[2] = STATUS_DASH;
        break;
      }
    } //switch(code)
  }

  //
  // Configure LED
  //
  statusLEDOn = !statusLEDOn;
  if (statusLEDOn) {
    LEDOff(WHITE_LED); // Turn all LEDS off
  } else if (statusBeepIndex < 3) {
    LEDOn(statusColor);
    statusDelayIndex++;
    if (statusBlinkDelay[statusBeepIndex]) {
        statusDelayIndex = 0;
        statusBeepIndex++;
    }
  } else if (statusBeepIndex < 3+STATUS_DELIMTER) {
    statusBeepIndex++;
  } else {
    statusBeepIndex = 0;
    statusBusy = false;
    statusCode = statusCodeDefault;
  }

  MAP_IntEnable(INT_TIMER0A);
} //Timer0IntHandler()
