/*
 * accel.c
 *
 *  Created on: Feb 17, 2015
 *      Author: Ryan
 */

#include "driverlib/adc.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "driverlib/sysctl.h"
#include "inc/hw_memmap.h"
#include "accel.h"

void
accelInit(void) {
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
  MAP_ADCIntEnable(ADC0_BASE, 0);
  MAP_ADCIntClear(ADC0_BASE, 0);
}
void
accelReceive(float* fptrForce) {
  uint32_t ui32ADCData;
  MAP_ADCProcessorTrigger(ADC0_BASE, 0);
  while(!MAP_ADCIntStatus(ADC0_BASE, 0, false)) {} // wait for a2d conversion
  MAP_ADCIntClear(ADC0_BASE, 0);
  MAP_ADCSequenceDataGet(ADC0_BASE, 0, &ui32ADCData);
  *fptrForce = ((float)ui32ADCData * 3.3 / 4095 - ACCEL_ZERO_G_VOUT) / ACCEL_G_RESOLUTION;
}
