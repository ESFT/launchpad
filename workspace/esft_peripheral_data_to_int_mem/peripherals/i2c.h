/*
 * i2c.h
 *
 *  Created on: Feb 17, 2015
 *      Author: Ryan
 */

#ifndef I2C_H_
#define I2C_H_

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

#include <stdbool.h>
#include <stdint.h>

#define I2C_MODE_WRITE false
#define I2C_MODE_READ  true
#define I2C_SPEED_100  false
#define I2C_SPEED_400  true

//*****************************************************************************
//
// Module function prototypes.
//
//*****************************************************************************
void I2CInit(uint32_t ui32Base, bool bSpeed);
bool I2CRead(uint32_t ui32Base, uint8_t ui8SlaveAddr, uint32_t* ui32ptr32Data);
bool I2CWrite(uint32_t ui32Base, uint8_t ui8SlaveAddr, uint8_t ui8Data);
bool I2CBurstRead(uint32_t ui32Base, uint8_t ui8SlaveAddr, uint32_t* ui32ptrReadData, uint32_t ui32Size);
bool I2CBurstWrite(uint32_t ui32Base, uint8_t ui8SlaveAddr, uint8_t ui8SendData[], uint32_t ui32Size);

//*****************************************************************************
//
// Mark the end of the C bindings section for C++ compilers.
//
//*****************************************************************************
#ifdef __cplusplus
}
#endif

#endif /* I2C_H_ */
