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

#include "uartstdio.h"

static uint32_t gps_ui32Base;
static uint32_t gps_ui32SenseBase;
static uint8_t gps_ui8SensePin;
static bool gps_dataInitialized;

GPSData_t* gps_data;
bool gps_new;

void
gpsInit(uint32_t ui32Base, uint32_t ui32SenseBase, uint8_t ui8SensePin, uint32_t ui32Baud, uint32_t ui32Config, GPSData_t* gpsData) {
  gps_ui32Base = ui32Base;
  gps_ui32SenseBase = ui32SenseBase;
  gps_ui8SensePin = ui8SensePin;
  gps_data = gpsData;

  // Initialize gpsData
  tinygps_crack_datetime(&(gps_data->year), &(gps_data->month), &(gps_data->day), &(gps_data->hour), &(gps_data->minute), &(gps_data->second), &(gps_data->hundredths), &(gps_data->fix_age));
  tinygps_f_get_position(&(gps_data->latitude), &(gps_data->longitude), &(gps_data->fix_age));
  gps_data->altitude = tinygps_f_altitude();
  gps_data->course = tinygps_f_course();
  gps_data->cardinal = tinygps_cardinal(gps_data->course);
  gps_data->speed = tinygps_f_speed_mps();

  UARTInit(ui32Base);
  gpioInputInit(ui32SenseBase, ui8SensePin, GPIO_PIN_TYPE_STD);
  MAP_UARTConfigSetExpClk(ui32Base, MAP_SysCtlClockGet(), ui32Baud, ui32Config);

  UARTIntInit(ui32Base, UART_INT_RX | UART_INT_RT);
}
void
gpsIntHandler(void) {
  MAP_UARTIntClear(gps_ui32Base, UART_INT_RX | UART_INT_RT);
  uint8_t newChar;
  do {
    newChar = (uint8_t) MAP_UARTCharGet(gps_ui32Base);
    gps_dataInitialized = tinygps_encode(newChar) ? true : gps_dataInitialized;
  } while (MAP_UARTCharsAvail(gps_ui32Base));
  if (gps_dataInitialized) {
    tinygps_crack_datetime(&(gps_data->year), &(gps_data->month), &(gps_data->day), &(gps_data->hour), &(gps_data->minute), &(gps_data->second), &(gps_data->hundredths), &(gps_data->fix_age));
    tinygps_f_get_position(&(gps_data->latitude), &(gps_data->longitude), &(gps_data->fix_age));
    gps_data->altitude = tinygps_f_altitude();
    gps_data->course = tinygps_f_course();
    gps_data->cardinal = tinygps_cardinal(gps_data->course);
    gps_data->speed = tinygps_f_speed_mps();
  }
}
bool
gpsAvailable(void) {
  bool temp = gps_dataInitialized;
  gps_dataInitialized = false;
  return (temp && MAP_GPIOPinRead(gps_ui32SenseBase, gps_ui8SensePin));
}
