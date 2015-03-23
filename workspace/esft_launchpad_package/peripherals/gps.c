/*
 * gps.c
 *
 *  Created on: Feb 17, 2015
 *      Author: Ryan
 */

#include <string.h>

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
static bool gps_dataInitialized;

static GPSData_t* gps_data;
static int8_t* gps_nmeaGPGGA;
static int8_t* gps_nmeaGPRMC;

static float gps_maxAlt;

#ifdef GPS_EMULATED_DATA_TEST
char emulatedDataRMC[] = GPS_GPRMC;
char emulatedDataGGA[] = GPS_GPGGA;
#endif

void
gpsInit(uint32_t ui32Base, uint32_t ui32Baud, uint32_t ui32Config, GPSData_t* gpsData) {
  gps_ui32Base = ui32Base;
  gps_data = gpsData;

  // Initialize gpsData
  gps_nmeaGPGGA = tinygps_getGPGGA();
  gps_nmeaGPRMC = tinygps_getGPRMC();
  tinygps_crack_datetime(&(gps_data->year), &(gps_data->month), &(gps_data->day), &(gps_data->hour), &(gps_data->minute), &(gps_data->second), &(gps_data->hundredths), &(gps_data->fix_age));
  tinygps_f_get_position(&(gps_data->latitude), &(gps_data->longitude), &(gps_data->fix_age));
  gps_data->altitude = tinygps_f_altitude();
  gps_data->course = tinygps_f_course();
  gps_data->cardinal = tinygps_cardinal(gps_data->course);
  gps_data->speed = tinygps_f_speed_mps();

  // Initialize the GPIO
  UARTInit(ui32Base, ui32Baud, ui32Config);
  UARTIntInit(ui32Base, UART_INT_RX | UART_INT_RT);
}
void
gpsIntHandler(void) {
  MAP_UARTIntClear(gps_ui32Base, UART_INT_RX | UART_INT_RT);
#ifdef GPS_EMULATED_DATA_TEST
  int32_t charIndex;
  while (!gps_dataInitialized) {
    for (charIndex = 0; charIndex < strlen(emulatedDataRMC); charIndex++) {
      gps_dataInitialized = tinygps_encode((int8_t)emulatedDataRMC[charIndex]) ? true : gps_dataInitialized;
    }
    for (charIndex = 0; charIndex < strlen(emulatedDataGGA); charIndex++) {
      gps_dataInitialized = tinygps_encode((int8_t)emulatedDataGGA[charIndex]) ? true : gps_dataInitialized;
    }
  }
#else
  uint8_t newChar;
  do {
    newChar = (uint8_t) MAP_UARTCharGet(gps_ui32Base);
    gps_dataInitialized = tinygps_encode(newChar) ? true : gps_dataInitialized;
  } while (MAP_UARTCharsAvail(gps_ui32Base));
#endif
  if (gps_dataInitialized) {
    tinygps_crack_datetime(&(gps_data->year), &(gps_data->month), &(gps_data->day), &(gps_data->hour), &(gps_data->minute), &(gps_data->second), &(gps_data->hundredths), &(gps_data->fix_age));
    tinygps_f_get_position(&(gps_data->latitude), &(gps_data->longitude), &(gps_data->fix_age));
    gps_data->altitude = tinygps_f_altitude();
    gps_data->course = tinygps_f_course();
    gps_data->cardinal = tinygps_cardinal(gps_data->course);
    gps_data->speed = tinygps_f_speed_mps();
    if (gps_data->altitude > gps_maxAlt) gps_maxAlt = gps_data->altitude;
  }
}
bool
gpsAvailable(void) {
  bool temp = gps_dataInitialized;
  gps_dataInitialized = false;
  return temp;
}
int8_t*
gpsGPGGA(void) {
  return gps_nmeaGPGGA;
}
int8_t*
gpsGPRMC(void) {
  return gps_nmeaGPRMC;
}
float*
gpsMaxAlt(void) {
  return &gps_maxAlt;
}
