/*
 * status.c
 *
 *  Created on: Feb 17, 2015
 *      Author: Ryan
 */

#include "status.h"

#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/timer.h"

#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"

#include "misc.h"

static StatusCode_t statusCode = INITIALIZING;  // Current Status Code
static StatusCode_t statusCodeDefault = INITIALIZING;  // Default state if none are set
static bool statusBusy = false;        // Status code system is busy

uint32_t misc_ui32TimerBase;
uint32_t misc_ui32TimerInt;

bool
setStatus(StatusCode_t scStatus) {  // Set status code
  if (!statusBusy) {
    statusCode = scStatus;
    return true;
  }
  return false;
}
void
setStatusDefault(StatusCode_t scStatus) {  // Set default status code
  statusCodeDefault = scStatus;
}
void
statusCodeInterruptInit(uint32_t ui32Base) {

  LEDInit();

  uint32_t ui32Peripheral;
  misc_ui32TimerBase = ui32Base;

  switch (ui32Base) {
    case TIMER0_BASE: {
      ui32Peripheral = SYSCTL_PERIPH_TIMER0;
      misc_ui32TimerInt = INT_TIMER0A;
      break;
    }
    case TIMER1_BASE: {
      ui32Peripheral = SYSCTL_PERIPH_TIMER1;
      misc_ui32TimerInt = INT_TIMER1A;
      break;
    }
    case TIMER2_BASE: {
      ui32Peripheral = SYSCTL_PERIPH_TIMER2;
      misc_ui32TimerInt = INT_TIMER2A;
      break;
    }
    case TIMER3_BASE: {
      ui32Peripheral = SYSCTL_PERIPH_TIMER3;
      misc_ui32TimerInt = INT_TIMER3A;
      break;
    }
    case TIMER4_BASE: {
      ui32Peripheral = SYSCTL_PERIPH_TIMER4;
      misc_ui32TimerInt = INT_TIMER4A;
      break;
    }
    case TIMER5_BASE: {
      ui32Peripheral = SYSCTL_PERIPH_TIMER5;
      misc_ui32TimerInt = INT_TIMER5A;
      break;
    }
  }
  //
  // Enable the perifpherals used by the timer interrupts
  //
  MAP_SysCtlPeripheralEnable(ui32Peripheral);

  //
  // Enable processor interrupts.
  //
  MAP_IntMasterEnable();

  //
  // Configure the 32-bit periodic timer for the status code length define.
  //
  MAP_TimerConfigure(misc_ui32TimerBase, TIMER_CFG_PERIODIC);
  MAP_TimerLoadSet(misc_ui32TimerBase, TIMER_A, MAP_SysCtlClockGet() / (1000 / STATUS_CODE_LENGTH));

  //
  // Setup the interrupts for the timer timeouts.
  //
  MAP_IntEnable(misc_ui32TimerInt);
  MAP_TimerIntEnable(misc_ui32TimerBase, TIMER_TIMA_TIMEOUT);

  //
  // Enable the timer.
  //
  MAP_TimerEnable(misc_ui32TimerBase, TIMER_A);
}
void
statusIntHandler(void) {  // Timer interrupt to handle status codes
  static uint8_t statusColor[3];        // Status Code Color
  static uint32_t statusBlinkDelay[3];  // Status Code Delays
  static uint8_t statusBeepIndex = 0;   // Index of beep LED
  static uint32_t statusDelayIndex = 0; // Index of beep length
  static bool statusBeepOn = false;     // Is "beep" still active

  //
  // Disable interrupts to prevent loops
  //
  MAP_IntDisable(misc_ui32TimerInt);

  //
  // Clear the timer interrupt
  //
  ROM_TimerIntClear(misc_ui32TimerBase, TIMER_TIMA_TIMEOUT);

  if (!statusBusy) {  // If a status code has already been initiated, do not overwrite
    statusBusy = true;  // Set the busy state for status codes

    //
    // set color and delay
    //
    switch (statusCode) {
      case INITIALIZING: {  // device is initializing
        statusColor[0] = WHITE_LED;
        statusColor[1] = WHITE_LED;
        statusColor[2] = WHITE_LED;
        statusBlinkDelay[0] = STATUS_DOT;
        statusBlinkDelay[1] = STATUS_DOT;
        statusBlinkDelay[2] = STATUS_DOT;
        break;
      }
      case GPS_NAV_LOCK_HOLD: {  // GPS Nav lock hold
        statusColor[0] = WHITE_LED;
        statusColor[1] = WHITE_LED;
        statusColor[2] = WHITE_LED;
        statusBlinkDelay[0] = STATUS_DOT;
        statusBlinkDelay[1] = STATUS_DOT;
        statusBlinkDelay[2] = STATUS_DASH;
        break;
      }
      case INITIALIZING_HOLD: {  // device is initializing hold
	    statusColor[0] = WHITE_LED;
    	statusColor[1] = WHITE_LED;
    	statusColor[2] = WHITE_LED;
        statusBlinkDelay[0] = STATUS_DASH;
        statusBlinkDelay[1] = STATUS_DASH;
        statusBlinkDelay[2] = STATUS_DASH;
        break;
      }
      case RUNNING: {  // code is running
        statusColor[0] = GREEN_LED;
        statusColor[1] = GREEN_LED;
        statusColor[2] = GREEN_LED;
        statusBlinkDelay[0] = STATUS_DOT;
        statusBlinkDelay[1] = STATUS_DOT;
        statusBlinkDelay[2] = STATUS_DOT;
        break;
      }
      case MMC_MOUNT_ERR: {
        statusColor[0] = BLUE_LED;
        statusColor[1] = BLUE_LED;
        statusColor[2] = BLUE_LED;
        statusBlinkDelay[0] = STATUS_DASH;
        statusBlinkDelay[1] = STATUS_DASH;
        statusBlinkDelay[2] = STATUS_DASH;
        break;
      }
      case MMC_OPEN_ERR: {
        statusColor[0] = BLUE_LED;
        statusColor[1] = BLUE_LED;
        statusColor[2] = BLUE_LED;
        statusBlinkDelay[0] = STATUS_DASH;
        statusBlinkDelay[1] = STATUS_DOT;
        statusBlinkDelay[2] = STATUS_DASH;
        break;
      }
      case OUT_OF_FLASH: {  // out of flash memory
        statusColor[0] = BLUE_LED;
        statusColor[1] = BLUE_LED;
        statusColor[2] = BLUE_LED;
        statusBlinkDelay[0] = STATUS_DOT;
        statusBlinkDelay[1] = STATUS_DOT;
        statusBlinkDelay[2] = STATUS_DOT;
        break;
      }
      case DRL_ERR: {  // driver library encountered an error
        statusColor[0] = RED_LED;
        statusColor[1] = RED_LED;
        statusColor[2] = RED_LED;
        statusBlinkDelay[0] = STATUS_DOT;
        statusBlinkDelay[1] = STATUS_DOT;
        statusBlinkDelay[2] = STATUS_DOT;
        break;
      }
      case BUS_FAULT: {  // Bus fault
        statusColor[0] = RED_LED;
        statusColor[1] = RED_LED;
        statusColor[2] = RED_LED;
        statusBlinkDelay[0] = STATUS_DOT;
        statusBlinkDelay[1] = STATUS_DOT;
        statusBlinkDelay[2] = STATUS_DASH;
        break;
      }
      case USAGE_FAULT: {  // Usage fault
        statusColor[0] = RED_LED;
        statusColor[1] = RED_LED;
        statusColor[2] = RED_LED;
        statusBlinkDelay[0] = STATUS_DOT;
        statusBlinkDelay[1] = STATUS_DASH;
        statusBlinkDelay[2] = STATUS_DOT;
        break;
      }
      case ACC250_ADC_CONV_ERR: {  // 250G Accelerometer ADC conversion error
        statusColor[0] = RED_LED;
        statusColor[1] = RED_LED;
        statusColor[2] = RED_LED;
        statusBlinkDelay[0] = STATUS_DASH;
        statusBlinkDelay[1] = STATUS_DOT;
        statusBlinkDelay[2] = STATUS_DOT;
        break;
      }
      case ALT_RESET_ERR: {  // Altimeter failed to reset
        statusColor[0] = RED_LED;
        statusColor[1] = RED_LED;
        statusColor[2] = RED_LED;
        statusBlinkDelay[0] = STATUS_DOT;
        statusBlinkDelay[1] = STATUS_DASH;
        statusBlinkDelay[2] = STATUS_DASH;
        break;
      }
      case ALT_PROM_ERR: {  // Altimeter PROM read failed
        statusColor[0] = RED_LED;
        statusColor[1] = RED_LED;
        statusColor[2] = RED_LED;
        statusBlinkDelay[0] = STATUS_DASH;
        statusBlinkDelay[1] = STATUS_DOT;
        statusBlinkDelay[2] = STATUS_DOT;
        break;
      }
      case ALT_CRC_ERR: {  // Altimeter CRC check failed
        statusColor[0] = RED_LED;
        statusColor[1] = RED_LED;
        statusColor[2] = RED_LED;
        statusBlinkDelay[0] = STATUS_DASH;
        statusBlinkDelay[1] = STATUS_DOT;
        statusBlinkDelay[2] = STATUS_DASH;
        break;
      }
      case ALT_GPS_NOT_FOUND: { // Can't track altitude. UNSAFE!
        statusColor[0] = RED_LED;
        statusColor[1] = RED_LED;
        statusColor[2] = RED_LED;
        statusBlinkDelay[0] = STATUS_DASH;
        statusBlinkDelay[1] = STATUS_DASH;
        statusBlinkDelay[2] = STATUS_DOT;
        break;
      }
      case ALT_ADC_CONV_ERR: {  // Altimeter data retrieve failed
        statusColor[0] = YELLOW_LED;
        statusColor[1] = YELLOW_LED;
        statusColor[2] = YELLOW_LED;
        statusBlinkDelay[0] = STATUS_DOT;
        statusBlinkDelay[1] = STATUS_DOT;
        statusBlinkDelay[2] = STATUS_DASH;
        break;
      }
      case COMPASS_ACCEL_STARTUP: {  // Compass accelerometer failed to initialize
        statusColor[0] = YELLOW_LED;
        statusColor[1] = YELLOW_LED;
        statusColor[2] = YELLOW_LED;
        statusBlinkDelay[0] = STATUS_DASH;
        statusBlinkDelay[1] = STATUS_DOT;
        statusBlinkDelay[2] = STATUS_DOT;
        break;
      }
      case COMPASS_MAG_STARTUP: {  // Compass magnetometer failed to initialize
        statusColor[0] = YELLOW_LED;
        statusColor[1] = YELLOW_LED;
        statusColor[2] = YELLOW_LED;
        statusBlinkDelay[0] = STATUS_DASH;
        statusBlinkDelay[1] = STATUS_DOT;
        statusBlinkDelay[2] = STATUS_DASH;
        break;
      }
      case GYRO_STARTUP_ERR: {  // Gyro failed to initialize
        statusColor[0] = YELLOW_LED;
        statusColor[1] = YELLOW_LED;
        statusColor[2] = YELLOW_LED;
        statusBlinkDelay[0] = STATUS_DOT;
        statusBlinkDelay[1] = STATUS_DASH;
        statusBlinkDelay[2] = STATUS_DASH;
        break;
      }
      case GYRO_READ_ERR: {  // Gyro failed to initialize
		  statusColor[0] = YELLOW_LED;
		  statusColor[1] = YELLOW_LED;
		  statusColor[2] = YELLOW_LED;
		  statusBlinkDelay[0] = STATUS_DASH;
		  statusBlinkDelay[1] = STATUS_DASH;
		  statusBlinkDelay[2] = STATUS_DASH;
		  break;
      }
      default: {
        statusColor[0] = RED_LED;
        statusColor[1] = RED_LED;
        statusColor[2] = RED_LED;
        statusBlinkDelay[0] = STATUS_DASH;
        statusBlinkDelay[1] = STATUS_DASH;
        statusBlinkDelay[2] = STATUS_DASH;
        break;
      }
    }  //switch(code)
  }

  //
  // Configure LED
  //
  if (!statusBeepOn) {
    LEDClear();  // Turn all LEDS off
    statusBeepOn = true;
  } else if (statusBeepIndex < 3) {
    LEDOn(statusColor[statusBeepIndex]);
    statusDelayIndex++;
    if (statusBlinkDelay[statusBeepIndex] == statusDelayIndex) {
      statusDelayIndex = 0;
      statusBeepIndex++;
      statusBeepOn = false;
    }
  } else if (statusBeepIndex < 3 + STATUS_DELIMTER) {
    statusBeepIndex++;
  } else {
    statusBeepIndex = 0;
    statusBusy = false;
    statusCode = statusCodeDefault;
  }

  MAP_IntEnable(misc_ui32TimerInt);
}  //Timer0IntHandler()
