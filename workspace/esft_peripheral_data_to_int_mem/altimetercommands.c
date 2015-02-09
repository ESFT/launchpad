//*****************************************************************************
//
// altimetercommands.c
//
// This file defines commands for the MS5607 Altimeter Module
//
// Information in this file copied from MS5607-02BA03 DataSheet from
// http://www.parallax.com/product/29124
//
//*****************************************************************************


//
// Commands
//
// Each I2C communication message starts with the start condition and it is
// ended with the stop condition. The MS5607-02BA address is 111011Cx, where C
// is the complementary value of the pin CSB. Since the IC does not have a
// microcontroller inside, the commands for I2 C and SPI are quite similar.
//

//
// Address of Altimeter
//
#define ALT_ADDRESS 0xEF

//
// Address Write Mode
// 
#define ALT_ADDRESS_WRITE 0xEE //may be incorrect

//
// Address Read Mode
//
#define ALT_ADDRESS_READ 0xEC //may be incorrect


//
// Reset
//
// The reset sequence shall be sent once after power-on to make sure that
// calibration PROM get loaded into the register. It can also be used to reset
// the device ROM from an unknown condition.
//
#define ALT_RESET 0x1E


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

//
// Convert D1 (OSR=256)
//
#define ALT_D1_256 0x40

//
// Convert D1 (OSR=512)
//
#define ALT_D1_512 0x42

//
// Convert D1 (OSR=1024)
//
#define ALT_D1_1024 0x44

//
// Convert D1 (OSR=2048)
//
#define ALT_D1_2048 0x46

//
// Convert D1 (OSR=4096)
//
#define ALT_D1_4096 0x48

//
// Convert D2 (OSR=256)
//
#define ALT_D2_256 0x40

//
// Convert D2 (OSR=512)
//
#define ALT_D2_512 0x42

//
// Convert D2 (OSR=1024)
//
#define ALT_D2_1024 0x44

//
// Convert D2 (OSR=2048)
//
#define ALT_D2_2048 0x46

//
// Convert D2 (OSR=4096)
//
#define ALT_D2_4096 0x48


//
// ACD Read
//
#define ALT_ADC_READ 0x00


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
// PROM Read Address 0 - factory data and setup
//
#define ALT_PROM_READ_0 0xA0

//
// PROM Read Address 1 - calibration coefficients
//
#define ALT_PROM_READ_1 0xA2

//
// PROM Read Address 2 - calibration coefficients
//
#define ALT_PROM_READ_2 0xA4

//
// PROM Read Address 3 - calibration coefficients
//
#define ALT_PROM_READ_3 0xA6

//
// PROM Read Address 4 - calibration coefficients
//
#define ALT_PROM_READ_4 0xA8

//
// PROM Read Address 5 - calibration coefficients
//
#define ALT_PROM_READ_5 0xAA

//
// PROM Read Address 6 - calibration coefficients
//
#define ALT_PROM_READ_6 0xAC

//
// PROM Read Address 7 - serial code and CRC
//
#define ALT_PROM_READ_7 0xAE