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
 License aint64_t with this library; if not, write to the Free Software
 Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include <math.h>
#include <stdlib.h>
#include <time.h>

#include "tinygps.h"

// properties
uint64_t _time = TINYGPS_INVALID_TIME, _new_time;
uint64_t _date, _new_date;
int64_t _latitude = TINYGPS_INVALID_ANGLE, _new_latitude;
int64_t _longtitude = TINYGPS_INVALID_ANGLE, _new_longtitude;
int64_t _altitude = TINYGPS_INVALID_ALTITUDE, _new_altitude;
uint64_t _speed = TINYGPS_INVALID_SPEED, _new_speed;
uint64_t _course = TINYGPS_INVALID_ANGLE, _new_course;
uint64_t _hdop = TINYGPS_INVALID_HDOP, _new_hdop;
uint16_t _numsats = TINYGPS_INVALID_SATELLITES, _new_numsats;

uint64_t _last_time_fix = TINYGPS_INVALID_FIX_TIME, _new_time_fix;
uint64_t _last_position_fix = TINYGPS_INVALID_FIX_TIME, _new_position_fix;

// parsing state variables
uint8_t _parity;
bool _is_checksum_term;
int8_t _term[15];
uint8_t _sentence_type;
uint8_t _term_number = 0;
uint8_t _term_offset = 0;
bool _is_tinygps_data_good;

#ifndef TINYGPS_NO_STATS
// statistics
uint64_t _encoded_characters;
uint16_t _good_sentences;
uint16_t _failed_checksum;
uint16_t _passed_checksum;
#endif

/*
 * internal utilities
 */

// internal utilities prototypes
clock_t uptime();
float radians(float deg);
float degrees(float rad);
int32_t from_hex(int8_t a);
uint64_t tinygps_parse_decimal();
uint64_t tinygps_parse_degrees();
bool tinygps_term_complete();
bool gpsisdigit(int8_t c);
int64_t gpsatol(const int8_t *str);
int32_t gpsstrcmp(const int8_t *str1, const int8_t *str2);

clock_t
uptime() {
  return clock() / (CLOCKS_PER_SEC / 1000);
}

float
radians(float deg) {
  return deg * (PI / 180);
}

float
degrees(float rad) {
  return rad * (180 / PI);
}

// verify is character is a digit
bool
tinygps_isdigit(int8_t c) {
  return c >= '0' && c <= '9';
}

int32_t
from_hex(int8_t a) {
  if (a >= 'A' && a <= 'F')
    return a - 'A' + 10;
  else if (a >= 'a' && a <= 'f')
    return a - 'a' + 10;
  else
    return a - '0';
}

uint64_t
tinygps_parse_decimal() {
  int8_t *p;
  bool isneg;
  uint64_t ret;

  p = _term;
  isneg = (*p == '-');
  if (isneg) ++p;

  ret = 100UL * gpsatol(p);

  while (tinygps_isdigit(*p))
    ++p;

  if (*p == '.') {
    if (tinygps_isdigit(p[1])) {
      ret += 10 * (p[1] - '0');
      if (tinygps_isdigit(p[2])) ret += p[2] - '0';
    }
  }
  return isneg ? -ret : ret;
}

uint64_t
tinygps_parse_degrees() {
  int8_t *p;
  uint64_t left;
  uint64_t tenk_minutes;

  left = gpsatol(_term);
  tenk_minutes = (left % 100UL) * 10000UL;

  for (p = _term; tinygps_isdigit(*p); ++p)
    ;

  if (*p == '.') {
    uint64_t mult = 1000;
    while (tinygps_isdigit(*++p)) {
      tenk_minutes += mult * (*p - '0');
      mult /= 10;
    }
  }
  return (left / 100) * 100000 + tenk_minutes / 6;
}

#define COMBINE(sentence_type, term_number) (((unsigned)(sentence_type) << 5) | term_number)

/* Processes a just-completed term
 * Returns true if new sentence has just passed checksum test and is validated
 */
bool
tinygps_term_complete() {
  if (_is_checksum_term) {
    uint8_t checksum;
    checksum = 16 * from_hex(_term[0]) + from_hex(_term[1]);
    if (checksum == _parity) {
      if (_is_tinygps_data_good) {
#ifndef TINYGPS_NO_STATS
        ++_good_sentences;
#endif
        _last_time_fix = _new_time_fix;
        _last_position_fix = _new_position_fix;

        switch (_sentence_type) {
          case TINYGPS_SENTENCE_GPRMC:
            _time = _new_time;
            _date = _new_date;
            _latitude = _new_latitude;
            _longtitude = _new_longtitude;
            _speed = _new_speed;
            _course = _new_course;
            break;
          case TINYGPS_SENTENCE_GPGGA:
            _altitude = _new_altitude;
            _time = _new_time;
            _latitude = _new_latitude;
            _longtitude = _new_longtitude;
            _numsats = _new_numsats;
            _hdop = _new_hdop;
            break;
        }

        return true;
      }
    }

#ifndef TINYGPS_NO_STATS
    else
      ++_failed_checksum;
#endif
    return false;
  }

  // the first term determines the sentence type
  if (_term_number == 0) {
    if (!gpsstrcmp(_term, GPRMC_TERM))
      _sentence_type = TINYGPS_SENTENCE_GPRMC;
    else if (!gpsstrcmp(_term, GPGGA_TERM))
      _sentence_type = TINYGPS_SENTENCE_GPGGA;
    else
      _sentence_type = TINYGPS_SENTENCE_OTHER;
    return false;
  }

  if (_sentence_type != TINYGPS_SENTENCE_OTHER && _term[0]) switch (COMBINE(_sentence_type, _term_number)) {
    case COMBINE(TINYGPS_SENTENCE_GPRMC, 1):  // Time in both sentences
    case COMBINE(TINYGPS_SENTENCE_GPGGA, 1):
      _new_time = tinygps_parse_decimal();
      _new_time_fix = uptime();
      break;
    case COMBINE(TINYGPS_SENTENCE_GPRMC, 2):  // GPRMC validity
      _is_tinygps_data_good = (_term[0] == 'A');
      break;
    case COMBINE(TINYGPS_SENTENCE_GPRMC, 3):  // Latitude
    case COMBINE(TINYGPS_SENTENCE_GPGGA, 2):
      _new_latitude = tinygps_parse_degrees();
      _new_position_fix = uptime();
      break;
    case COMBINE(TINYGPS_SENTENCE_GPRMC, 4):  // N/S
    case COMBINE(TINYGPS_SENTENCE_GPGGA, 3):
      if (_term[0] == 'S') _new_latitude = -_new_latitude;
      break;
    case COMBINE(TINYGPS_SENTENCE_GPRMC, 5):  // Longitude
    case COMBINE(TINYGPS_SENTENCE_GPGGA, 4):
      _new_longtitude = tinygps_parse_degrees();
      break;
    case COMBINE(TINYGPS_SENTENCE_GPRMC, 6):  // E/W
    case COMBINE(TINYGPS_SENTENCE_GPGGA, 5):
      if (_term[0] == 'W') _new_longtitude = -_new_longtitude;
      break;
    case COMBINE(TINYGPS_SENTENCE_GPRMC, 7):  // Speed (GPRMC)
      _new_speed = tinygps_parse_decimal();
      break;
    case COMBINE(TINYGPS_SENTENCE_GPRMC, 8):  // Course (GPRMC)
      _new_course = tinygps_parse_decimal();
      break;
    case COMBINE(TINYGPS_SENTENCE_GPRMC, 9):  // Date (GPRMC)
      _new_date = gpsatol(_term);
      break;
    case COMBINE(TINYGPS_SENTENCE_GPGGA, 6):  // Fix data (GPGGA)
      _is_tinygps_data_good = (_term[0] > '0');
      break;
    case COMBINE(TINYGPS_SENTENCE_GPGGA, 7):  // Satellites used (GPGGA)
      _new_numsats = (uint8_t) atoi((char*) _term);
      break;
    case COMBINE(TINYGPS_SENTENCE_GPGGA, 8):  // HDOP
      _new_hdop = tinygps_parse_decimal();
      break;
    case COMBINE(TINYGPS_SENTENCE_GPGGA, 9):  // Altitude (GPGGA)
      _new_altitude = tinygps_parse_decimal();
      break;
  }

  return false;
}

int64_t
gpsatol(const int8_t *str) {
  int64_t ret = 0;
  while (tinygps_isdigit(*str))
    ret = 10 * ret + *str++ - '0';
  return ret;
}

int32_t
gpsstrcmp(const int8_t *str1, const int8_t *str2) {
  while (*str1 && *str1 == *str2)
    ++str1, ++str2;
  return *str1;
}

//
// public methods
//

// signed altitude in centimeters (from GPGGA sentence)
int64_t
altitude() {
  return _altitude;
}

// course in last full GPRMC sentence in 100th of a degree
uint64_t
course() {
  return _course;
}

// speed in last full GPRMC sentence in 100ths of a knot
uint64_t
speed() {
  return _speed;
}

// satellites used in last full GPGGA sentence
inline uint16_t
tinygps_satellites() {
  return _numsats;
}

// horizontal dilution of precision in 100ths
inline uint64_t
tinygps_hdop() {
  return _hdop;
}

bool
tinygps_encode(int8_t c) {
  bool valid_sentence = false;

#ifndef TINYGPS_NO_STATS
  _encoded_characters++;
#endif
  switch (c) {
    case ',':  // term terminators
      _parity ^= c;
    case '\r':
    case '\n':
    case '*':
      if (_term_offset < sizeof(_term)) {
        _term[_term_offset] = 0;
        valid_sentence = tinygps_term_complete();
      }
      ++_term_number;
      _term_offset = 0;
      _is_checksum_term = c == '*';
      return valid_sentence;

    case '$':  // sentence begin
      _term_number = 0;
      _term_offset = 0;
      _parity = 0;
      _sentence_type = TINYGPS_SENTENCE_OTHER;
      _is_checksum_term = false;
      _is_tinygps_data_good = false;
      return valid_sentence;
  }

  // ordinary characters
  if (_term_offset < sizeof(_term) - 1) _term[_term_offset++] = c;
  if (!_is_checksum_term) _parity ^= c;

  return valid_sentence;
}

#ifndef TINYGPS_NO_STATS
void
tinygps_stats(uint64_t *chars, uint16_t *sentences, uint16_t *failed_cs) {
  if (chars) *chars = _encoded_characters;
  if (sentences) *sentences = _good_sentences;
  if (failed_cs) *failed_cs = _failed_checksum;
}
#endif

float
tinygps_distance_between(float lat1, float long1, float lat2, float int64_t2) {
  // returns distance in meters between two positions, both specified
  // as signed decimal-degrees latitude and longtitude. Uses great-circle
  // distance computation for hypothetical sphere of radius 6372795 meters.
  // Because Earth is no exact sphere, rounding errors may be up to 0.5%.
  // Courtesy of Maarten Lamers
  float delta = radians(long1 - int64_t2);
  float sdint64_t = (float) sin(delta);
  float cdint64_t = (float) cos(delta);
  lat1 = radians(lat1);
  lat2 = radians(lat2);
  float slat1 = sin(lat1);
  float clat1 = cos(lat1);
  float slat2 = sin(lat2);
  float clat2 = cos(lat2);
  delta = (clat1 * slat2) - (slat1 * clat2 * cdint64_t);
  delta = sq(delta);
  delta += sq(clat2 * sdint64_t);
  delta = sqrt(delta);
  float denom = (slat1 * slat2) + (clat1 * clat2 * cdint64_t);
  delta = atan2(delta, denom);
  return delta * 6372795;
}

float
tinygps_course_to(float lat1, float long1, float lat2, float int64_t2) {
  // returns course in degrees (North=0, West=270) from position 1 to position 2,
  // both specified as signed decimal-degrees latitude and longtitude.
  // Because Earth is no exact sphere, calculated course may be off by a tiny fraction.
  // Courtesy of Maarten Lamers
  float dlon = radians(int64_t2 - long1);
  lat1 = radians(lat1);
  lat2 = radians(lat2);
  float a1 = sin(dlon) * cos(lat2);
  float a2 = sin(lat1) * cos(lat2) * cos(dlon);
  a2 = cos(lat1) * sin(lat2) - a2;
  a2 = atan2(a1, a2);
  if (a2 < 0.0) {
    a2 += TWO_PI;
  }
  return degrees(a2);
}

const int8_t *
tinygps_cardinal(float course) {
  static const int8_t* directions[] = { "N", "NNE", "NE", "ENE", "E", "ESE", "SE", "SSE", "S", "SSW", "SW", "WSW", "W", "WNW", "NW", "NNW" };

  int32_t direction = (int32_t) ((course + 11.25f) / 22.5f);
  return directions[direction % 16];
}

// lat/long in hundred thousandths of a degree and age of fix in milliseconds
void
tinygps_get_position(int64_t *latitude, int64_t *longtitude, uint64_t *fix_age) {
  if (latitude) *latitude = _latitude;
  if (longtitude) *longtitude = _longtitude;
  if (fix_age) *fix_age = (_last_position_fix == TINYGPS_INVALID_FIX_TIME) ? TINYGPS_INVALID_AGE : uptime() - _last_position_fix;
}

// date as ddmmyy, time as hhmmsscc, and age in milliseconds
void
tinygps_get_datetime(uint64_t *date, uint64_t *time, uint64_t *age) {
  if (date) *date = _date;
  if (time) *time = _time;
  if (age) *age = _last_time_fix == TINYGPS_INVALID_FIX_TIME ? TINYGPS_INVALID_AGE : uptime() - _last_time_fix;
}

void
tinygps_f_get_position(float *latitude, float *longtitude, uint64_t *fix_age) {
  int64_t lat, lon;
  tinygps_get_position(&lat, &lon, fix_age);
  *latitude = lat == TINYGPS_INVALID_ANGLE ? TINYGPS_INVALID_F_ANGLE : (lat / 100000.0);
  *longtitude = lat == TINYGPS_INVALID_ANGLE ? TINYGPS_INVALID_F_ANGLE : (lon / 100000.0);
}

void
tinygps_crack_datetime(int32_t *year, uint8_t *month, uint8_t *day, uint8_t *hour, uint8_t *minute, uint8_t *second, uint8_t *hundredths, uint64_t *age) {
  uint64_t date, time;
  tinygps_get_datetime(&date, &time, age);
  if (year) {
    *year = date % 100;
    *year += *year > 80 ? 1900 : 2000;
  }
  if (month) *month = (date / 100) % 100;
  if (day) *day = date / 10000;
  if (hour) *hour = time / 1000000;
  if (minute) *minute = (time / 10000) % 100;
  if (second) *second = (time / 100) % 100;
  if (hundredths) *hundredths = time % 100;
}

float
tinygps_f_altitude() {
  return _altitude == TINYGPS_INVALID_ALTITUDE ? TINYGPS_INVALID_F_ALTITUDE : _altitude / 100.0;
}

float
tinygps_f_course() {
  return _course == TINYGPS_INVALID_ANGLE ? TINYGPS_INVALID_F_ANGLE : _course / 100.0;
}

float
tinygps_f_speed_knots() {
  return _speed == TINYGPS_INVALID_SPEED ? TINYGPS_INVALID_F_SPEED : _speed / 100.0;
}

float
tinygps_f_speed_mph() {
  float sk = tinygps_f_speed_knots();
  return sk == TINYGPS_INVALID_F_SPEED ? TINYGPS_INVALID_F_SPEED : TINYGPS_MPH_PER_KNOT * tinygps_f_speed_knots();
}

float
tinygps_f_speed_mps() {
  float sk = tinygps_f_speed_knots();
  return sk == TINYGPS_INVALID_F_SPEED ? TINYGPS_INVALID_F_SPEED : TINYGPS_MPS_PER_KNOT * tinygps_f_speed_knots();
}

float
tinygps_f_speed_kmph() {
  float sk = tinygps_f_speed_knots();
  return sk == TINYGPS_INVALID_F_SPEED ? TINYGPS_INVALID_F_SPEED : TINYGPS_KMPH_PER_KNOT * tinygps_f_speed_knots();
}
