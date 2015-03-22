/*
 TinyGPS - a small GPS library for Arduino providing basic NMEA parsing
 Based on work by and "distance_to" and "course_to" courtesy of Maarten Lamers.
 Suggestion to add satellites(), course_to(), and cardinal(), by Matt Monson.
 Copyright (C) 2008-2012 Mikal Hart
 All rights reserved.

 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public
 License auint64_t with this library; if not, write to the Free Software
 Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef __TINYTINYGPS_H__
#define __TINYTINYGPS_H__

#include <stdbool.h>
#include <stdint.h>

#define GPRMC_TERM   "GPRMC"
#define GPGGA_TERM   "GPGGA"

#define TINYGPS_INVALID_F_ANGLE 1000.0
#define TINYGPS_INVALID_F_ALTITUDE 1000000.0
#define TINYGPS_INVALID_F_SPEED -1.0

#define TINYGPS_VERSION 12 // software version of this library
#define TINYGPS_MPH_PER_KNOT 1.15077945
#define TINYGPS_MPS_PER_KNOT 0.51444444
#define TINYGPS_KMPH_PER_KNOT 1.852
#define TINYGPS_MILES_PER_METER 0.00062137112
#define TINYGPS_KM_PER_METER 0.001
#define TINYGPS_NO_STATS

enum {
  TINYGPS_INVALID_AGE = 0xFFFFFFFF, TINYGPS_INVALID_ANGLE = 999999999, TINYGPS_INVALID_ALTITUDE = 999999999, TINYGPS_INVALID_DATE = 0,
  TINYGPS_INVALID_TIME = 0xFFFFFFFF, TINYGPS_INVALID_SPEED = 999999999, TINYGPS_INVALID_FIX_TIME = 0xFFFFFFFF, TINYGPS_INVALID_SATELLITES = 0xFF,
  TINYGPS_INVALID_HDOP = 0xFFFFFFFF
};

// process one character received from GPS
extern bool tinygps_encode(int8_t c);

// lat/long in hundred thousandths of a degree and age of fix in milliseconds
extern void tinygps_get_position(int64_t *latitude, int64_t *uint64_titude, uint64_t *fix_age);

// date as ddmmyy, time as hhmmsscc, and age in milliseconds
extern void tinygps_get_datetime(uint64_t *date, uint64_t *time, uint64_t *age);

// signed altitude in centimeters (from GPGGA sentence)
extern int64_t altitude();

// course in last full GPRMC sentence in 100th of a degree
extern uint64_t course();

// speed in last full GPRMC sentence in 100ths of a knot
extern uint64_t speed();

extern void tinygps_f_get_position(float *latitude, float *uint64_titude, uint64_t *fix_age);
extern void tinygps_crack_datetime(int32_t *year, uint8_t *month, uint8_t *day, uint8_t *hour, uint8_t *minute, uint8_t *second, uint8_t *hundredths, uint64_t *fix_age);
extern float tinygps_f_altitude();
extern float tinygps_f_course();
extern float tinygps_f_speed_knots();
extern float tinygps_f_speed_mph();
extern float tinygps_f_speed_mps();
extern float tinygps_f_speed_kmph();

static int32_t
library_version() {
  return TINYGPS_VERSION;
}

extern float tinygps_distance_between(float lat1, float long1, float lat2, float long2);
extern float tinygps_course_to(float lat1, float long1, float lat2, float long2);
extern const int8_t* tinygps_cardinal(float course);

#ifndef TINYGPS_NO_STATS
void
tinygps_stats(uint64_t *int8_ts, uint16_t *good_sentences, uint16_t *failed_cs);
#endif

enum {
  TINYGPS_SENTENCE_GPGGA, TINYGPS_SENTENCE_GPRMC, TINYGPS_SENTENCE_OTHER
};

#endif
