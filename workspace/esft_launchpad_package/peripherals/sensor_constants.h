/*
 * sensor_contants.h
 *
 *  Created on: Mar 4, 2015
 *      Author: Ryan
 */

#ifndef SENSOR_CONTANTS_H_
#define SENSOR_CONTANTS_H_

/* Constants */
#define SENSORS_NO_CHANGE                    (1)                    /**< No modification to sensor output */
#define SENSORS_GRAVITY_EARTH                (9.80665)              /**< Earth's gravity in m/s^2 */
#define SENSORS_GRAVITY_MOON                 (1.6)                  /**< The moon's gravity in m/s^2 */
#define SENSORS_GRAVITY_SUN                  (275.0)                /**< The sun's gravity in m/s^2 */
#define SENSORS_GRAVITY_STANDARD             SENSORS_GRAVITY_EARTH
#define SENSORS_GAUSS_TO_MICROTESLA          (100.0)                /**< Gauss to micro-Tesla multiplier */
#define SENSORS_GAUSS_TO_MILLITESLA          (1000.0)               /**< Gauss to milli-Tesla multiplier */
#define SENSORS_GAUSS_TO_TESLA               (10000.0)              /**< Gauss to Tesla multiplier */
#define SENSORS_GAUSS_CONVERSION_STANDARD    SENSORS_GAUSS_TO_MICROTESLA
#define SENSORS_PASCAL_TO_MBAR               (0.01)                 /**< Pascal to millibar multiplier */
#define SENSORS_PRESSURE_CONVERSION_STANDARD SENSORS_NO_CHANGE
#define SENSORS_TEMP_CELSIUS_TO_FARENHEIT    (1.8) + 32             /**< Celsius to Farenheit multiplier */
#define SENSORS_TEMP_CELSIUS_TO_KELVIN       (1)   + 273.15         /**< Celsius to Kelvin multiplier */
#define SENSORS_TEMP_CONVERSION_STANDARD     SENSORS_NO_CHANGE
#define SENSORS_METERS_TO_FEET               (3.280839895)          /**< Meters to Feet multiplier */
#define SENSORS_METERS_TO_YARDS              (1.093613298)          /**< Meters to Yards multiplier */
#define SENSORS_DISTANCE_CONVERSION_STANDARD SENSORS_NO_CHANGE

#endif /* SENSOR_CONTANTS_H_ */
