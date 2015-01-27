//*****************************************************************************
//
// flashstore.h - Prototypes and definitions for flash storage module.
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
// Module function prototypes.
//
//*****************************************************************************
extern uint32_t PACK(uint8_t, uint8_t, uint8_t, uint8_t);
extern uint8_t UNPACK_C0(uint32_t);
extern uint8_t UNPACK_C1(uint32_t);
extern uint8_t UNPACK_C2(uint32_t);
extern uint8_t UNPACK_C3(uint32_t);

extern int FlashStoreInit(void);
extern int32_t FlashStoreNewLogFile(uint32_t ui32StartAddr, int);
extern int32_t FlashStoreWriteRecord(uint8_t *, int);
extern int32_t FlashStoreSave(void);
extern void FlashStoreErase(void);
extern int32_t FlashStoreFree(void);
extern int32_t FlashStoreUsed(void);
extern uint32_t FlashStoreGetStartAddr(void);
extern uint32_t FlashStoreGetEndAddr(void);
extern uint32_t FlashStoreGetAddr(uint32_t);
extern uint32_t FlashStoreGetCurrentAddr(void);
extern void     FlashStoreSetCurrentAddr(uint32_t);
extern uint32_t FlashStoreGetHeader(void);

//*****************************************************************************
//
// Mark the end of the C bindings section for C++ compilers.
//
//*****************************************************************************
#ifdef __cplusplus
}
#endif

#endif // __FLASHSTORE_H__
