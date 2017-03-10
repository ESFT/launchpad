/*
 * gps.c
 *
 * Created on: Feb 17, 2015
 * Author: Ryan
 */

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "inc/hw_memmap.h"
#include "inc/hw_nvic.h"
#include "inc/hw_types.h"

#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"

#include "utils/uartstdio.h"

#include "userlib/gpio.h"
#include "userlib/uart.h"

#include "gps.h"
#include "misc.h"

static volatile uint32_t g_gps_ui32Base;
static volatile bool g_gps_newDataAvailable = false;
static tGPSData *g_gps_data;
static int8_t *g_gps_nmeaGPGGA, *g_gps_nmeaGPRMC;
static volatile uint32_t g_gps_ui32NavLockPort;
static volatile uint8_t g_gps_ui8NavLockPin;
#if GPS_EMULATED_DATA_TEST
char g_gps_emulatedDataRMC[] = GPS_GPRMC_TEST_DATA;
char g_gps_emulatedDataGGA[] = GPS_GPGGA_TEST_DATA;
#endif
void GPSIntHandler(void) {
  bool g_gps_validDataFound = false;
  MAP_UARTIntClear(g_gps_ui32Base, UART_INT_RX | UART_INT_RT);
#if GPS_EMULATED_DATA_TEST
  for (newChar = 0; newChar < strlen(g_gps_emulatedDataRMC); newChar++) {
    g_gps_validDataFound = tinygps_encode((int8_t) g_gps_emulatedDataRMC[newChar]) ? true : g_gps_validDataFound;
  }
  for (newChar = 0; newChar < strlen(g_gps_emulatedDataGGA); newChar++) {
    g_gps_validDataFound = tinygps_encode((int8_t) g_gps_emulatedDataGGA[newChar]) ? true : g_gps_validDataFound;
  }
#else
  do {
    g_gps_validDataFound = tinygps_encode((int8_t) MAP_UARTCharGet(g_gps_ui32Base)) ? true : g_gps_validDataFound;
  } while (MAP_UARTCharsAvail(g_gps_ui32Base));
#endif
  if (g_gps_validDataFound) {
    g_gps_validDataFound = false;
    g_gps_newDataAvailable = true;
    tinygps_crack_datetime(&(g_gps_data->i32Year), &(g_gps_data->ui8Month), &(g_gps_data->ui8Day), &(g_gps_data->ui8Hour), &(g_gps_data->ui8Minute), &(g_gps_data->ui8Second), &(g_gps_data->ui8Hundredths), &(g_gps_data->ui64FixAge));
    tinygps_f_get_position(&(g_gps_data->fLatitude), &(g_gps_data->fLongitude), &(g_gps_data->ui64FixAge));
    if (tinygps_f_altitude() != TINYGPS_INVALID_F_ALTITUDE) g_gps_data->fAltitude = tinygps_f_altitude();
    //if (tinygps_f_course() != TINYGPS_INVALID_F_ANGLE) g_gps_data->fCourse = tinygps_f_course();
    //if (g_gps_data->fCourse != TINYGPS_INVALID_F_ANGLE) g_gps_data->i8Cardinal = *(tinygps_cardinal(g_gps_data->fCourse));
    //if (tinygps_f_speed_mps() != TINYGPS_INVALID_F_SPEED) g_gps_data->fSpeed = tinygps_f_speed_mps();
  }
  g_gps_data->bNavLock = GPSNavLocked();
}
void GPSInit(uint32_t ui32Base, uint32_t ui32Baud, uint32_t ui32Config, tGPSData* gpsData, uint32_t ui32NavLockPort, uint8_t ui8NavLockPin) {
  g_gps_ui32Base = ui32Base;
  g_gps_data = gpsData;
  g_gps_nmeaGPGGA = tinygps_getGPGGA();
  g_gps_nmeaGPRMC = tinygps_getGPRMC();
  tinygps_crack_datetime(&(g_gps_data->i32Year), &(g_gps_data->ui8Month), &(g_gps_data->ui8Day), &(g_gps_data->ui8Hour), &(g_gps_data->ui8Minute), &(g_gps_data->ui8Second), &(g_gps_data->ui8Hundredths), &(g_gps_data->ui64FixAge));
  tinygps_f_get_position(&(g_gps_data->fLatitude), &(g_gps_data->fLongitude), &(g_gps_data->ui64FixAge));
  g_gps_data->fAltitude = tinygps_f_altitude();
  //g_gps_data->fCourse = tinygps_f_course();
  //g_gps_data->i8Cardinal = *(tinygps_cardinal(g_gps_data->fCourse));
  //g_gps_data->fSpeed = tinygps_f_speed_mps();
  UARTInit(ui32Base, ui32Baud, false, ui32Config);
  UARTIntInit(ui32Base, UART_INT_RX | UART_INT_RT, 0x00, GPSIntHandler);
  GPIOInputInit(ui32NavLockPort, ui8NavLockPin, GPIO_PIN_TYPE_STD);
  g_gps_ui32NavLockPort = ui32NavLockPort;
  g_gps_ui8NavLockPin = ui8NavLockPin;
}
inline bool GPSNavLocked(void) {
  return MAP_GPIOPinRead(g_gps_ui32NavLockPort, g_gps_ui8NavLockPin) || ((g_gps_data->ui64FixAge) < 5000);
}
bool GPSAvailable(void) {
  bool temp = g_gps_newDataAvailable;
  g_gps_newDataAvailable = false;
  return temp;
}
inline int8_t* GPSGPGGA(void) {
  return g_gps_nmeaGPGGA;
}
inline int8_t* GPSGPRMC(void) {
  return g_gps_nmeaGPRMC;
}
inline clock_t tinygps_msuptime(void) {
  if (!(HWREG(NVIC_DBG_INT) & 0x01000000) && !(HWREG(DWT_BASE) & 0x01)) {
      HWREG(NVIC_DBG_INT) |= 0x01000000;  /*enable TRCENA bit in NVIC_DBG_INT*/
      HWREG(DWT_BASE + 0x4) = 0;   /* reset the counter */
      HWREG(DWT_BASE) |= 0x01; /* enable the counter */
    }
    return (HWREG(DWT_BASE + 0x4) / (MAP_SysCtlClockGet() / 1000));

}
