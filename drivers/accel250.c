/*
 * accel.c
 *
 *  Created on: Feb 17, 2015
 *      Author: Ryan
 */

#include "accel250.h"

#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"

#include "driverlib/adc.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "driverlib/sysctl.h"

int32_t Accel250DataGetFloat(float *pfData) {
  uint32_t ui32Data;
  int32_t i32SampleCount;
  i32SampleCount = Accel250DataGetRaw(&ui32Data);
  *pfData = (ui32Data * ACCEL_250_MAX_VOUT / 4095.0F - ACCEL_250_ZERO_G_VOUT) / ACCEL_250_G_RESOLUTION;
  return i32SampleCount;
}

int32_t Accel250DataGetRaw(uint32_t *pui32Data) {
  MAP_ADCIntClear(ADC0_BASE, 0);
  return MAP_ADCSequenceDataGet(ADC0_BASE, 0, pui32Data);
}

void Accel250Init(void) {
  //
  // Enable the GPIO peripheral used by the accelerometer
  //
  MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
  while (!MAP_SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOE));

  //
  // Configure GPIO PE3 as ADC0 for accelerometer
  //
  MAP_GPIOPinTypeADC(GPIO_PORTE_BASE, GPIO_PIN_3);

  //
  // Enable the accelerometer peripheral
  //
  MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);
  while (!MAP_SysCtlPeripheralReady(SYSCTL_PERIPH_ADC0));
  MAP_SysCtlPeripheralReset(SYSCTL_PERIPH_ADC0);
  while (!MAP_SysCtlPeripheralReady(SYSCTL_PERIPH_ADC0));

  //
  // Configure ADC0 on sequence 0 as a low priority continuous trigger on Channel 0
  // with an oversample of 64x
  //
  MAP_ADCSequenceDisable(ADC0_BASE, 0);
  MAP_ADCSequenceConfigure(ADC0_BASE, 0, ADC_TRIGGER_ALWAYS, 0);
  MAP_ADCSequenceStepConfigure(ADC0_BASE, 0, 0, ADC_CTL_CH0 | ADC_CTL_IE | ADC_CTL_END);
  MAP_ADCHardwareOversampleConfigure(ADC0_BASE, 64);
  MAP_ADCSequenceEnable(ADC0_BASE, 0);
  MAP_ADCIntClear(ADC0_BASE, 0);

  while (!Accel250IsDataAvailable());
}

extern bool Accel250IsDataAvailable(void);
