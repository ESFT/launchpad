//*****************************************************************************
//
// altimeter.h
//
// This file defines commands for the MS5607 Altimeter Module
//
// Information in this file copied from MS5607-02BA03 DataSheet from
// http://www.parallax.com/product/29124
//
//*****************************************************************************

#ifndef __ALTIMETER_H__
#define __ALTIMETER_H__

#include <stdint.h>

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

//
// Commands
//
// Each I2C communication message starts with the start condition and it is
// ended with the stop condition. The MS5607-02BA address is 111011Cx, where C
// is the complementary value of the pin CSB. Since the IC does not have a
// microcontroller inside, the commands for I2 C and SPI are quite similar.
//
// I2C address for Tiva are a 7 bit address with the 8th bit being the RS bit.
// As such, the address for the MS5607-02BA should be right shifted by 1,
// making the address 111011C. Depending on C, the final address is 0x77 when
// CSB is pulled low and 0x76 when CSB is pulled high.
//

//
// Address of Altimeter
//
#define ALT_ADDRESS_CSB_LO 0x77
#define ALT_ADDRESS_CSB_HI 0x76

//
// Reset
//
// The reset sequence shall be sent once after power-on to make sure that
// calibration PROM get loaded into the register. It can also be used to reset
// the device ROM from an unknown condition.
//
#define ALT_RESET 0x1E
#define ALT_RESET_DELAY 3

//
// ACD Read
//
#define ALT_ADC_READ 0x00

//
// Conversion Sequence
//
// The conversion command is used to initiate uncompensated pressure (D1) or
// uncompensated temperature (D2) conversion. The chip select can be disabled
// during this time to communicate with other devices. After the conversion, using
// ADC read command the result is clocked out with the MSB first. If the conversion
// is not executed before the ADC read command, or the ADC read command is repeated,
// it will give 0 as the output result. If the ADC read command is sent during
// conversion the result will be 0, the conversion will not stop and the final result
// will be wrong. Conversion sequence sent during the already started conversion process
// will yield incorrect result as well.
//

#define ALT_ADC_CONV 0x40

//
// Conversion type
//
#define ALT_ADC_D1 0x00
#define ALT_ADC_D2 0x10

//
// ADC Conversion Resolutions
//
#define ALT_ADC_256  0x00
#define ALT_ADC_512  0x02
#define ALT_ADC_1024 0x04
#define ALT_ADC_2048 0x06
#define ALT_ADC_4096 0x08

// Conversion DELAYs
#define ALT_256_DELAY 1
#define ALT_512_DELAY 3
#define ALT_1024_DELAY 4
#define ALT_2048_DELAY 6
#define ALT_4096_DELAY 10

//
// PROM Read Sequence
//
// The read command for PROM shall be executed once after reset by the user to
// read the content of the calibration PROM and to calculate the calibration
// coefficients. There are in total 8 addresses resulting in a total memory of
// 128 bit. Address 0 contains factory data and the setup, addresses 1-6 calibration
// coefficients and address 7 contains the serial code and CRC. The command sequence
// is 8 bits long with a 16 bit result which is clocked with the MSB first.
//

//
// PROM Read - factory data and setup
//
#define ALT_PROM_READ 0xA0

//*****************************************************************************
//
// Mark the end of the C bindings section for C++ compilers.
//
//*****************************************************************************
#ifdef __cplusplus
}
#endif

#endif // __flashStore_H__
