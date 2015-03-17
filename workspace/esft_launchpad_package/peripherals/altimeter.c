/*
 * altimeter.c
 *
 *  Created on: Feb 17, 2015
 *      Author: Ryan
 */

#include <math.h>

#include "driverlib/rom.h"
#include "driverlib/rom_map.h"

#include "altimeter.h"
#include "i2c.h"
#include "sensor_constants.h"
#include "misc.h"
#include "uartstdio.h"

static uint32_t alt_ui32Base;
static uint8_t  alt_ui8AltAddr;
static uint16_t alt_ui16Prom[8];

uint8_t
__altCRC4(void) {
  int32_t cnt; // simple counter
  uint16_t n_rem = 0x00; // crc reminder
  uint16_t crc_read = alt_ui16Prom[7]; // original value of the crc
  uint8_t n_bit;
  alt_ui16Prom[7]=(0xFF00 & (alt_ui16Prom[7])); //CRC byte is replaced by 0
  for (cnt = 0; cnt < 16; cnt++) // operation is performed on bytes
  { // choose LSB or MSB
    if (cnt%2==1) {
      n_rem ^= (alt_ui16Prom[cnt>>1]) & 0x00FF;
    }
    else {
      n_rem ^= alt_ui16Prom[cnt>>1]>>8;
    }
    for (n_bit = 8; n_bit > 0; n_bit--)
    {
      if (n_rem & (0x8000))
      {
        n_rem = (n_rem << 1) ^ 0x3000;
      }
      else
      {
        n_rem = (n_rem << 1);
      }
    }
  }
  n_rem= (0x000F & (n_rem >> 12)); // final 4-bit reminder is CRC code
  alt_ui16Prom[7]=crc_read; // restore the crc_read to its original place
  return (n_rem ^ 0x0);
}
bool
altADCConversion(uint8_t ui8Cmd, uint32_t* ui32ptrData) {
  uint8_t ret[3];

  if (!I2CWrite(alt_ui32Base, alt_ui8AltAddr, ALT_ADC_CONV+ui8Cmd)) {
    if (consoleIsEnabled()) UARTprintf("ALT_ADC_CONV write error\n\r");
    return false;
  }
  switch(ui8Cmd & 0x0F) {
    case ALT_ADC_256:  delay(ALT_256_DELAY);  break;
    case ALT_ADC_512:  delay(ALT_512_DELAY);  break;
    case ALT_ADC_1024: delay(ALT_1024_DELAY); break;
    case ALT_ADC_2048: delay(ALT_2048_DELAY); break;
    case ALT_ADC_4096: delay(ALT_4096_DELAY); break;
  }

  //
  // Tell altimeter to send data to launchpad
  //
  if (!I2CWrite(alt_ui32Base, alt_ui8AltAddr, ALT_ADC_READ)) {
    if (consoleIsEnabled()) UARTprintf("ALT_ADC_READ write error\n\r");
    return false;
  }

  //
  // Start receiving data from altimeter
  //
  if (!I2CBurstRead(alt_ui32Base, alt_ui8AltAddr, &ret[0], 3)) {
    if (consoleIsEnabled()) UARTprintf("ALT_ADC_READ read error\n\r");
    return false;
  }

  *ui32ptrData = (65536*ret[0]) + (256 * ret[1]) + ret[2];
  return true;
}
StatusCode_t
altInit(uint32_t ui32Base, uint8_t ui8AltAddr, bool bSpeed) {
  uint8_t altCRC;
  alt_ui32Base = ui32Base;
  alt_ui8AltAddr = ui8AltAddr;

  //
  // Enable the I2C module used by the altimeter
  //
  I2CInit(alt_ui32Base, bSpeed);

  // Reset altimeter
  if (!altReset()) return ALT_RESET_ERR;

  if (!altProm()) return ALT_PROM_ERR; // read coefficients
  altCRC = __altCRC4(); // calculate the CRC.

  // If prom CRC (Last byte of coefficient 7) and calculated CRC do not match, something went wrong!
  if (altCRC != (alt_ui16Prom[7] & 0x000F)) return ALT_CRC_ERR;

  return INITIALIZING;
}
bool
altProm(void) {
  uint32_t i;
  uint8_t ret[2];
  for (i = 0; i < 8; i++) {
    if (!I2CWrite(alt_ui32Base, alt_ui8AltAddr, ALT_PROM_READ+(i*2))) {
      if (consoleIsEnabled()) UARTprintf("ALT_PROM_READ write error\n\r");
      return false;
    }
    if (!I2CBurstRead(alt_ui32Base, alt_ui8AltAddr, &ret[0], 2)) {
      if (consoleIsEnabled()) UARTprintf("ALT_PROM_READ read error\n\r");
      return false;
    }
    alt_ui16Prom[i] = ((256 * ret[0]) + ret[1]);
  }
  return true;
}
bool
altReceive(uint8_t ui8OSR, AltData_t* altData) {
  // Digital pressure and temp
  uint32_t adcPressure = 0;   // ADC value of the pressure conversion
  uint32_t adcTemperature = 0;   // ADC value of the temperature conversion

  // Temp difference, offsets, and sensitivities
  int32_t  dT    = 0; // difference between actual and measured temperature
  int32_t  T2    = 0; // second order temperature offset
  int64_t  OFF   = 0; // offset at actual temperature
  int64_t  OFF2  = 0; // second order offset at actual temperature
  int64_t  SENS  = 0; // sensitivity at actual temperature
  int64_t  SENS2 = 0; // second order sensitivity at actual temperature

  if (altADCConversion(ALT_ADC_D1+ui8OSR, &adcPressure) && altADCConversion(ALT_ADC_D2+ui8OSR, &adcTemperature)) {
    // Calculate 1st order temp difference, offset, and sensitivity (MS5607 1st order algorithm)
    dT=adcTemperature-alt_ui16Prom[5]*pow(2,8);
    OFF=alt_ui16Prom[2]*pow(2,17)+dT*alt_ui16Prom[4]/pow(2,6);
    SENS=alt_ui16Prom[1]*pow(2,16)+dT*alt_ui16Prom[3]/pow(2,7);

    // Calculate 1st order temperature (celsius)
    altData->temperature=(2000+(dT*alt_ui16Prom[6])/pow(2,23))/100;

    // Calculate 2nd order temp difference, offset, and sensitivity (MS5607 2nd order algorithm)
    if (altData->temperature < 20) {
      T2 = pow(dT, 2)/pow(2,31);
      OFF2 = 61*pow((altData->temperature-2000),2)/pow(2,4);
      SENS2 = 2*pow((altData->temperature-2000),2);
      if (altData->temperature < -15) {
        OFF2 = OFF2+15*pow((altData->temperature+1500),2);
        SENS2 = SENS2+8*pow((altData->temperature+1500),2);
      }
    }
    OFF = OFF - OFF2;
    SENS = SENS - SENS2;

    // Calculate 2nd order temperature (celsius)
    altData->temperature -= T2;

    // Calculate 2nd order pressure (pascal)
    altData->pressure = ((adcPressure*SENS)/pow(2,21)-OFF)/pow(2,15);

    // Calculate 2nd order altitude (meters)
    altData->altitude = 44331.5 - (4946.62*pow(altData->pressure,0.190263));

    // Convert units as per sensor_constants.h
    altData->temperature *= SENSORS_TEMP_CONVERSION_STANDARD;
    altData->pressure    *= SENSORS_PRESSURE_CONVERSION_STANDARD;
    altData->altitude    *= SENSORS_DISTANCE_CONVERSION_STANDARD;
    return true;
  }
  return false;
}
bool
altReset(void) {
  if (!I2CWrite(alt_ui32Base, alt_ui8AltAddr, ALT_RESET)) {
    if (consoleIsEnabled()) UARTprintf("ALT_RESET write error\n\r");
    return false;
  }
  delay(ALT_RESET_DELAY);
  return true;
}
