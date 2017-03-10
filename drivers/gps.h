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

#define GPS_EMULATED_DATA_TEST false // Feeds the encoding functions emulated data streams to test code functionality

#define GPS_GPRMC_TEST_DATA "$GPRMC,173843,A,3349.896,N,11808.521,W,000.0,360.0,230108,013.4,E*69\r\n"
#define GPS_GPGGA_TEST_DATA "$GPGGA,162254.00,3723.02837,N,12159.39853,W,1,03,2.36,525.6,M,-25.6,M,,*65\r\n"

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
    int32_t  i32Year;
    uint8_t  ui8Month;
    uint8_t  ui8Day;
    uint8_t  ui8Hour;
    uint8_t  ui8Minute;
    uint8_t  ui8Second;
    uint8_t  ui8Hundredths;
    uint64_t ui64FixAge;
    float    fLatitude;
    float    fLongitude;
    float    fAltitude;
    float    fCourse;
    int8_t   i8Cardinal;
    float    fSpeed;
} tGPSData;

//*****************************************************************************
//
// Module function prototypes.
//
//*****************************************************************************
extern void GPSInit(uint32_t ui32Base, uint32_t ui32Baud, uint32_t ui32Config, tGPSData* gpsData, uint32_t ui32NavLockPort, uint8_t ui8NovLockPin);
extern bool GPSAvailable(void);
extern bool GPSNavLocked(void);
extern void GPSInterrupt(void);
extern int8_t* GPSGPGGA(void);
extern int8_t* GPSGPRMC(void);

//*****************************************************************************
//
// Mark the end of the C bindings section for C++ compilers.
//
//*****************************************************************************
#ifdef __cplusplus
}
#endif

#endif /* GPS_H_ */
