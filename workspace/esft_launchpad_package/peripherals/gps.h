/*
 * gps.h
 *
 *  Created on: Feb 17, 2015
 *      Author: Ryan
 */

#ifndef GPS_H_
#define GPS_H_

#include <stdbool.h>
#include <stdint.h>

#include "driverlib/uart.h"

#include "tinygps/tinygps.h"

#define GPS_BAUD 9600
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
extern void gpsInit(uint32_t ui32Base, uint32_t ui32SenseBase, uint8_t ui8SensePin, uint32_t ui32Baud, uint32_t ui32Config, GPSData_t* gpsData);
extern bool gpsAvailable();
extern void gpsInterrupt();

//*****************************************************************************
//
// Mark the end of the C bindings section for C++ compilers.
//
//*****************************************************************************
#ifdef __cplusplus
}
#endif

#endif /* GPS_H_ */
