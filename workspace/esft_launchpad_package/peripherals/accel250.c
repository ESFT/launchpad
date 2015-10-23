/*
 * accel.c
 *
 *  Created on: Feb 17, 2015
 *      Author: Ryan
 */

#include "accel250.h"
#include "sensor_constants.h"

#include "driverlib/adc.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "driverlib/sysctl.h"

#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"

float* accel250_fForcePtr;
bool   accel250_dataAvailable;

void
accel250Init(float* fForcePtr, StatusCode_t* status) {
  accel250_fForcePtr = fForcePtr;

  //
  // Enable the peripherals used by the accelerometer
  //
  MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);
  MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);

  //
  // Configure GPIO PE3 as ADC0 for accelerometer
  //
  MAP_GPIOPinTypeADC(GPIO_PORTE_BASE, GPIO_PIN_3);

  //
  // Configure ADC0 on sequence 0 as an interrupt trigger on Channel 0
  // with an oversample of 64x
  //
  MAP_ADCSequenceDisable(ADC0_BASE, 0);
  MAP_ADCSequenceConfigure(ADC0_BASE, 0, ADC_TRIGGER_PROCESSOR, 0);
  MAP_ADCSequenceStepConfigure(ADC0_BASE, 0, 0, ADC_CTL_CH0 | ADC_CTL_IE | ADC_CTL_END);
  MAP_ADCHardwareOversampleConfigure(ADC0_BASE, 64);
  MAP_ADCSequenceEnable(ADC0_BASE, 0);

  // Enable ADC interupts
  MAP_IntEnable(INT_ADC0SS0);
  MAP_ADCIntEnable(ADC0_BASE, 0);

  MAP_ADCProcessorTrigger(ADC0_BASE, 0);
}
bool
accel250Receive(void) {
  bool temp = accel250_dataAvailable;
  accel250_dataAvailable = false;
  return temp;
}
void
accel250IntHandler(void) {
  uint32_t ui32ADCData, ui32SampleCount;
  MAP_ADCIntClear(ADC0_BASE, 0);
  ui32SampleCount = MAP_ADCSequenceDataGet(ADC0_BASE, 0, &ui32ADCData);
  if (ui32SampleCount > 0) {
    *accel250_fForcePtr = (((float)ui32ADCData * 3.3 / 4095 - ACCEL_250_ZERO_G_VOUT) / ACCEL_250_G_RESOLUTION) * SENSORS_GRAVITY_STANDARD;
    accel250_dataAvailable = true;
  }
  MAP_ADCProcessorTrigger(ADC0_BASE, 0);
}
