//*****************************************************************************
//
// flashStore.h - Prototypes and definitions for flash storage module.
//
// Copyright (c) 2011-2014 Texas Instruments Incorporated.  All rights reserved.
// Software License Agreement
//
// Texas Instruments (TI) is supplying this software for use solely and
// exclusively on TI's microcontroller products. The software is owned by
// TI and/or its suppliers, and is protected under applicable copyright
// laws. You may not combine this software with "viral" open-source
// software in order to form a larger program.
//
// THIS SOFTWARE IS PROVIDED "AS IS" AND WITH ALL FAULTS.
// NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT
// NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. TI SHALL NOT, UNDER ANY
// CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR CONSEQUENTIAL
// DAMAGES, FOR ANY REASON WHATSOEVER.
//
// This is part of revision 2.1.0.12573 of the DK-TM4C123G Firmware Package.
//
//*****************************************************************************

#ifndef __FLASHSTORE_H__
#define __FLASHSTORE_H__

#include <stdbool.h>
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

//*****************************************************************************
//
// Define the beginning and end of the flash storage area.  You must make sure
// that this area is well clear of any space occupied by the application
// binary, and that this space is not used for any other purpose.
// The start and end addresses must be 1K aligned.  The end address is
// exclusive - it is 1 value greater than the last valid location used for
// storage.
//
//*****************************************************************************
#define FLASH_STORE_START_ADDR  0x30000
#define FLASH_STORE_END_ADDR    0x40000

//*****************************************************************************
//
// Flash block write size
//
//*****************************************************************************
#define FLASH_STORE_BLOCK_WRITE_SIZE 0x04

//*****************************************************************************
//
// Flash block erase size
//
//*****************************************************************************
#define FLASH_STORE_BLOCK_ERASE_SIZE 0x400

//*****************************************************************************
//
// Empty data (all ones)
//
//*****************************************************************************
#define FLASH_STORE_EMPTY_DATA 0xFFFFFFFF

//*****************************************************************************
//
// Define the flash record header, which is a 3-byte signature and an added
// one byte count of bytes in the record.  Saved at the beginning
// of the write buffer.
//
//*****************************************************************************
#define FLASH_STORE_RECORD_HEADER 0x53554100
#define FLASH_STORE_RECORD_HEADER_SIZE 0x04

//*****************************************************************************
//
// Module function prototypes.
//
//*****************************************************************************
extern uint32_t pack(uint8_t, uint8_t, uint8_t, uint8_t);
extern void     unpack(uint32_t, uint8_t*);
extern bool     flashstoreInit(bool);
extern bool     flashstoreNewLogFile(uint32_t, bool);
extern bool     flashstoreWriteRecord(uint8_t *, uint32_t);
extern int32_t  flashstoreSave(void);
extern void     flashstoreFormat(void);
static bool     flashstoreIsBlockFree(uint32_t);
extern bool     flashstoreFree(void);
extern bool     flashstoreUsed(void);
extern uint32_t flashstoreGetData(uint32_t);
extern uint32_t flashstoreGetCurrentAddr(void);
extern void     flashstoreSetCurrentAddr(uint32_t);

//*****************************************************************************
//
// Mark the end of the C bindings section for C++ compilers.
//
//*****************************************************************************
#ifdef __cplusplus
}
#endif

#endif // __flashStore_H__
