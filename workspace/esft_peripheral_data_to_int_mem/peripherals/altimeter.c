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
#include "misc.h"
#include "status.h"
#include "uartstdio.h"

bool
altADCConversion(uint32_t ui32Base, uint8_t ui8AltAddr, uint8_t ui8Cmd, uint32_t* ui32ptrData) {
  uint32_t ret[3];

  if (!I2CWrite(ui32Base, ui8AltAddr, ALT_ADC_CONV+ui8Cmd)) {
    setStatus(ALT_ADC_CONV_ERR);
    UARTprintf("ALT_ADC_CONV write error\n\r");
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
  if (!I2CWrite(ui32Base, ui8AltAddr, ALT_ADC_READ)) {
    setStatus(ALT_ADC_R_WRITE_ERR);
    UARTprintf("ALT_ADC_READ write error\n\r");
    return false;
  }

  //
  // Start receiving data from altimeter
  //
  if (!I2CBurstRead(ui32Base, ui8AltAddr, &ret[0], 3)) {
    setStatus(ALT_ADC_R_READ_ERR);
    UARTprintf("ALT_ADC_READ read error\n\r");
    return false;
  }

  *ui32ptrData = (65536*ret[0]) + (256 * ret[1]) + ret[2];
  return true;
}
uint8_t
altCRC4(uint16_t ui16nProm[8]) {
  int32_t cnt; // simple counter
  uint16_t n_rem = 0x00; // crc reminder
  uint16_t crc_read = ui16nProm[7]; // original value of the crc
  uint8_t n_bit;
  ui16nProm[7]=(0xFF00 & (ui16nProm[7])); //CRC byte is replaced by 0
  for (cnt = 0; cnt < 16; cnt++) // operation is performed on bytes
  { // choose LSB or MSB
    if (cnt%2==1) {
      n_rem ^= (ui16nProm[cnt>>1]) & 0x00FF;
    }
    else {
      n_rem ^= ui16nProm[cnt>>1]>>8;
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
  ui16nProm[7]=crc_read; // restore the crc_read to its original place
  return (n_rem ^ 0x0);
}
void
altInit(uint32_t ui32Base, uint8_t ui8AltAddr, bool bSpeed) {
  //
  // Enable the I2C module used by the altimeter
  //
  I2CInit(ui32Base, bSpeed);

  // Reset altimeter
  while (!altReset(ui32Base, ui8AltAddr)) {}
}
bool
altProm(uint32_t ui32Base, uint8_t ui8AltAddr, uint16_t ui16nProm[8]) {
  uint32_t i;
  uint32_t ret[2];
  for (i = 0; i < 8; i++) {
    if (!I2CWrite(ui32Base, ui8AltAddr, ALT_PROM_READ+(i*2))) {
      setStatus(ALT_PROM_R_WRITE_ERR);
      UARTprintf("ALT_PROM_READ write error\n\r");
      return false;
    }
    if (!I2CBurstRead(ui32Base, ui8AltAddr, &ret[0], 2)) {
      while (!setStatus(ALT_PROM_R_READ_ERR)) {}
      UARTprintf("ALT_PROM_READ read error\n\r");
      return false;
    }
    ui16nProm[i] = ((256 * ret[0]) + ret[1]);
  }
  return true;
}
bool
altReceive(uint32_t ui32Base, uint8_t ui8AltAddr, uint8_t ui8OSR, uint16_t ui16Calibration[8], float* fptrTemp, float* fptrPressure, float* fptrAltitude)
{
  // Digital pressure and temp
  uint32_t D1 = 0;   // ADC value of the pressure conversion
  uint32_t D2 = 0;   // ADC value of the temperature conversion

  // Temp difference, offsets, and sensitivities
  int32_t  dT    = 0; // difference between actual and measured temperature
  int32_t  T2    = 0; // second order temperature offset
  int64_t  OFF   = 0; // offset at actual temperature
  int64_t  OFF2  = 0; // second order offset at actual temperature
  int64_t  SENS  = 0; // sensitivity at actual temperature
  int64_t  SENS2 = 0; // second order sensitivity at actual temperature

  if (altADCConversion(ui32Base, ui8AltAddr, ALT_ADC_D1+ui8OSR, &D1) && altADCConversion(ui32Base, ui8AltAddr, ALT_ADC_D2+ui8OSR, &D2)) {
    // Calculate 1st order pressure and temperature

    // Calculate 1st order temp difference, offset, and sensitivity (MS5607 1st order algorithm)
    dT=D2-ui16Calibration[5]*pow(2,8);
    OFF=ui16Calibration[2]*pow(2,17)+dT*ui16Calibration[4]/pow(2,6);
    SENS=ui16Calibration[1]*pow(2,16)+dT*ui16Calibration[3]/pow(2,7);

    // Calculate temperature
    *fptrTemp=(2000+(dT*ui16Calibration[6])/pow(2,23))/100;

    // Calculate 2nd order temp difference, offset, and sensitivity (MS5607 2nd order algorithm)
    if (*fptrTemp < 20) {
      T2 = pow(dT, 2)/pow(2,31);
      OFF2 = 61*pow((*fptrTemp-2000),2)/pow(2,4);
      SENS2 = 2*pow((*fptrTemp-2000),2);
      if (*fptrTemp < -15)
      {
        OFF2 = OFF2+15*pow((*fptrTemp+1500),2);
        SENS2 = SENS2+8*pow((*fptrTemp+1500),2);
      }
    }
    *fptrTemp = *fptrTemp - T2;
    OFF = OFF - OFF2;
    SENS = SENS - SENS2;

    // Calculate pressure
    *fptrPressure=((D1*SENS)/pow(2,21)-OFF)/pow(2,15)/100;

    // Calculate altitude (in feet)
    // TODO: Convert to meters
    *fptrAltitude = (1-pow((*fptrPressure/1013.25),.190284))*145366.45;
    return true;
  }
  return false;
}
bool
altReset(uint32_t ui32Base, uint8_t ui8AltAddr) {
  if (!I2CWrite(ui32Base, ui8AltAddr, ALT_RESET)) {
    while (!setStatus(ALT_RESET_ERR)) {}
    UARTprintf("ALT_RESET write error\n\r");
    return false;
  }
  delay(ALT_RESET_DELAY);
  return true;
}
