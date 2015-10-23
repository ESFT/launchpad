/*
 * gps.h
 *
 *  Created on: Feb 17, 2015
 *      Author: Ryan
 */

#ifndef __GPS_H__
#define __GPS_H__

#include <stdbool.h>
#include <stdint.h>

#include "driverlib/uart.h"

#include "tinygps/tinygps.h"

//#define GPS_EMULATED_DATA_TEST // Feeds the encoding functions emulated data streams to test code functionality

#define GPS_GPRMC "$GPRMC,173843,A,3349.896,N,11808.521,W,000.0,360.0,230108,013.4,E*69\r\n"
#define GPS_GPGGA "$GPGGA,111609.14,5001.27,N,3613.06,E,3,08,0.0,10.2,M,0.0,M,0.0,0000*70\r\n"

#define GPS_BAUD 115200
#define GPS_UART_CONFIG UART_CONFIG_WLEN_8 | UART_CONFIG_PAR_NONE | UART_CONFIG_STOP_ONE

//*****************************************************************************
//
// If building with a C++ compiler, make all of the definitions in this header
// have a C binding.
//
//*****************************************************************************
#ifdef __cplusplus
extern "C"
{
#endif

typedef struct {
    int32_t  year;
    uint8_t  month;
    uint8_t  day;
    uint8_t  hour;
    uint8_t  minute;
    uint8_t  second;
    uint8_t  hundredths;
    uint64_t fix_age;
    float    latitude;
    float    longitude;
    float    altitude;
    float    course;
    const int8_t* cardinal;
    float    speed;
} GPSData_t;

//*****************************************************************************
//
// Module function prototypes.
//
//*****************************************************************************
extern void gpsInit(uint32_t ui32Base, uint32_t ui32Baud, uint32_t ui32Config, GPSData_t* gpsData, uint32_t ui32NavLockPort, uint8_t ui8NovLockPin);
extern bool gpsAvailable(void);
extern bool gpsNavLocked(void);
extern void gpsInterrupt(void);
extern int8_t* gpsGPGGA(void);
extern int8_t* gpsGPRMC(void);
extern float* gpsMaxAlt(void);

//*****************************************************************************
//
// Mark the end of the C bindings section for C++ compilers.
//
//*****************************************************************************
#ifdef __cplusplus
}
#endif

#endif /* GPS_H_ */
