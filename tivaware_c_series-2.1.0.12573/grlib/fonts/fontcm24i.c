//*****************************************************************************
//
// fontcm24i.c - Font definition for the 24pt Cm italic font.
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
// This is part of revision 2.1.0.12573 of the Tiva Graphics Library.
//
//*****************************************************************************

//*****************************************************************************
//
// This file is generated by ftrasterize; DO NOT EDIT BY HAND!
//
//*****************************************************************************

#include <stdint.h>
#include <stdbool.h>
#include "grlib/grlib.h"

//*****************************************************************************
//
// Details of this font:
//     Characters: 32 to 126 inclusive
//     Style: cm
//     Size: 24 point
//     Bold: no
//     Italic: yes
//     Memory usage: 2936 bytes
//
//*****************************************************************************

//*****************************************************************************
//
// The compressed data for the 24 point Cm italic font.
// Contains characters 32 to 126 inclusive.
//
//*****************************************************************************
static const uint8_t g_pui8Cm24iData[2735] =
{
      4,  10,   0,  30,  21,   8, 194,  83,  82,  98,  98,  97,
     98,  98,  97, 113, 113,  97, 113, 240, 114,  98,   0,   6,
     96,  18,   9, 240,  66,  34,  50,  34,  65,  49,  49,  49,
     65,  49,  49,  49,   0,  18,  64,  44,  18,   0,   5,  97,
     65, 177,  65, 193,  65, 193,  49, 193,  65, 193,  49, 193,
     65, 193,  49, 127, 129,  49, 193,  65, 193,  49, 143, 113,
     49, 193,  65, 177,  65, 193,  65, 177,  65, 193,  65, 177,
     65,   0,   6,  31,  16, 240, 240, 180, 178,  49, 146,  65,
    145,  66, 130,  66, 130, 225, 213, 209, 226, 225, 241, 241,
    181,  81,  65,  51,  49,  83,  52,   0,  12,  96,  60,  17,
     66, 129,  81,  34,  67,  65,  49,  22,  81,  49,  66,  81,
     65,  51,  81,  65,  50,  97,  49,  50, 113,  49,  34, 129,
     33,  34, 162,  34,  50, 161,  49,  33, 130,  33,  49, 114,
     49,  49,  98,  49,  65,  82,  65,  65,  66,  81,  49,  81,
     97,  49,  65, 113,  33,  81, 130,   0,  11,  48,  51,  18,
    240, 132,  82,  97,  50,  66,  81,  81,  65,  82,  66,  65,
     81,  82,  49,  82, 145,  98,  19,  66,  99,  33,  49,  18,
     82,  49,  34,  33,  81,  19,  50,  33,  66, 114,  33,  65,
    193,  65, 177,  81, 177,  82, 145, 114,  98, 166,   0,  14,
     80,  11,   5, 178,  50,  65,  49,  65,  49,   0,  10,  64,
     27,  10, 113, 129, 129, 145, 129, 129, 145, 129, 145, 130,
    129, 145, 145, 130, 129, 145, 145, 145, 145, 145, 145, 161,
    145, 145, 128,  27,  10, 240,  17, 145, 161, 145, 145, 145,
    145, 145, 145, 145, 130, 129, 145, 145, 129, 145, 129, 145,
    129, 145, 129, 129, 129, 144,  22,  11,  81, 161,  97,  49,
     33,  65,  17,  18,  99, 131,  99,  33,  65,  33,  49,  97,
    161,   0,  20,  16,  22,  16,   0,   9,   1, 241, 241, 225,
    241, 241, 241, 241, 142, 129, 241, 241, 225, 241, 241,   0,
     11,  32,  11,   5,   0,  10,  18,  50,  65,  49,  65,  49,
    224,   8,   8,   0,  13,   6,   0,  10,  32,   8,   4,   0,
      8,   2,  34, 240, 176,  27,  16, 209, 225, 241, 225, 241,
    225, 241, 225, 225, 241, 225, 241, 225, 241, 225, 241, 225,
    225, 241, 225, 241, 225, 241, 225, 240,  35,  12, 240, 240,
    180, 113,  50,  81,  81,  66,  81,  65,  97,  50,  97,  49,
     98,  49,  97,  50,  97,  49,  98,  49,  97,  65,  82,  65,
     81,  82,  49, 116,   0,   9, 112,  22,  10, 240, 240, 113,
    114, 100, 130, 130, 129, 130, 130, 129, 145, 130, 130, 129,
    145, 103,   0,   7, 112,  34,  12, 240, 240, 195, 114,  49,
     82,  65,  81,  33,  33,  65,  49,  33,  65,  33,  49,  67,
     49, 161, 161, 146, 145, 161, 161,  97,  68,  49,  49,  67,
      0,   9,  64,  32,  13, 240, 240, 244, 114,  65,  81,  97,
     65,  49,  49,  65,  49,  49,  83,  49, 178, 131, 209, 193,
    193,  66,  82,  66,  81,  81,  66, 116,   0,  10,  96,  31,
     12, 240, 240, 241, 162, 161, 177, 162, 161, 162, 161, 161,
    177,  18, 113,  33, 113,  49,  97,  65,  69,  18,  65,  69,
    129, 162, 162, 161, 240, 240,  28,  11, 240, 240, 102,  85,
     81, 161, 161, 145,  35,  83,  49,  65,  81, 161, 161,  49,
     82,  49,  81, 146,  65,  50,  99,   0,   9,  16,  35,  11,
    240, 240, 147,  98,  33,  81,  65,  66, 145,  19,  81,  17,
     34,  66,  65,  50,  81,  50,  81,  49,  82,  49,  82,  49,
     81,  65,  66,  66,  34,  99,   0,   9,  16,  29,  13, 240,
    240, 177,  19,  49,  66,  33,  33,  65,  82,  81,  97,  65,
     97, 193, 177, 178, 178, 177, 178, 177, 178, 178, 177,   0,
     10, 112,  33,  13, 240, 240, 244, 114,  50,  81,  97,  81,
     97,  81,  81,  98,  49, 132, 148, 114,  35,  81,  97,  65,
    113,  65, 113,  65,  97,  97,  65, 132,   0,  10,  80,  34,
     12, 240, 240, 195, 114,  49,  82,  65,  81,  81,  66,  81,
     65,  97,  65,  82,  65,  67,  65,  49,  17,  99,  18, 161,
    162,  81,  65,  81,  50, 116,   0,   9, 112,  13,   6,   0,
      6,  34,  66, 240, 240, 130,  66, 240, 240, 160,  16,   7,
      0,   7,  50,  82, 240, 240, 242,  82,  97,  97,  81,  81,
    240,  80,  20,   8,   0,   7,  66,  98, 225,  97, 113, 113,
    113,  98,  98,  97,  98,  98,  98,  83,  82, 224,  11,  17,
      0,  21,  62,   0,   6,  94,   0,  19,  64,  30,  10,   0,
      9,  66, 114, 240,  35,  98,  17,  97,  33, 130, 114, 114,
     98,  50,  49,  66,  33,  97,  33,  81,  49,  81,  50,  34,
     83, 240,  16,  29,  10, 227,  82,  49,  49,  81,  49,  81,
     33,  97,  34,  65,  50,  50,  98, 114, 114, 129,  33,  97,
     18,  99, 240, 194, 114,   0,   8,  64,  57,  17, 240, 240,
    181, 162,  82, 113,  50,  49,  97,  34,  33,  49,  65,  34,
     65,  33,  49,  49,  81,  33,  49,  34,  81,  33,  33,  49,
     82,  33,  33,  49,  82,  33,  33,  49,  81,  33,  49,  49,
     66,  33,  49,  65,  35,  17,  81,  51,  35,  81, 240,  34,
     98, 135,   0,  13,  80,  36,  16, 240, 177, 241, 226, 209,
     17, 209,  17, 193,  33, 177,  50, 161,  50, 145,  66, 145,
     66, 129,  82, 136, 113,  98, 113,  98,  97, 114,  82, 114,
     68,  70,   0,  12,  32,  36,  16, 240, 240, 105, 145,  82,
    114,  97, 114,  97, 113,  98, 113,  98,  98,  82, 119, 145,
     97, 129, 113,  98, 113,  98, 113,  97, 129,  97, 113,  98,
     97,  90,   0,  12,  96,  34,  16, 240, 240, 133,  33, 113,
     81,  17,  82, 114,  66, 129,  81, 145,  65, 161,  65, 225,
    241, 241, 241, 241, 145,  81, 129, 113, 113, 114,  66, 164,
      0,  13,  16,  37,  17, 240, 240, 122, 146,  82, 130,  97,
    129, 129,  98, 129,  98, 129,  98, 129,  97, 145,  82, 130,
     82, 129,  98, 129,  97, 129,  98, 129,  98, 113, 114,  82,
    105,   0,  13,  96,  36,  17, 240, 240, 124, 114,  98, 114,
    113, 113, 129,  98, 129,  98,  65, 162,  49, 182, 162,  65,
    162,  65, 162, 129,  97, 129,  98, 129,  98, 114,  98,  98,
     92,   0,  13,  48,  31,  17, 240, 240, 124, 114,  98, 114,
    113, 113, 113, 114, 113, 114,  65, 162,  49, 178,  49, 167,
    162,  65, 162, 241, 242, 242, 242, 214,   0,  14,  16,  34,
     16, 240, 240, 148, 162,  65,  17,  97, 114,  81, 130,  65,
    145,  66, 145,  65, 161,  65, 225, 241, 241, 102,  49, 146,
     65, 145,  97, 114,  98,  83, 133,   0,  13,  38,  20, 240,
    240, 230,  38, 130,  98, 146, 113, 162,  98, 162,  98, 161,
    114, 146, 113, 170, 162,  98, 161, 114, 146, 113, 162,  98,
    162,  98, 161, 114, 146, 113, 134,  38,   0,  15,  96,  22,
     12, 240, 214, 130, 161, 162, 162, 162, 161, 162, 162, 161,
    162, 162, 162, 161, 162, 134,   0,   9,  96,  26,  14, 240,
    240,  70, 162, 194, 193, 194, 194, 194, 193, 194, 194, 194,
    193, 114,  50, 114,  50, 113,  50, 147,   0,  11,  96,  40,
     20, 240, 240, 214,  69, 114, 113, 162,  97, 177,  97, 178,
     81, 194,  50, 210,  34, 225,  35, 210,  17,  18, 211,  50,
    194,  66, 193,  98, 162,  98, 162,  98, 162, 114, 117,  69,
      0,  15,  96,  27,  14, 240, 240,  23, 146, 194, 193, 194,
    194, 194, 193, 194, 194, 194, 193, 113,  66, 113,  66,  97,
     82,  82,  59,   0,  10, 112,  61,  23,   0,   6,  36, 148,
    115, 131, 147, 113,  17, 145,  18, 113,  17, 145,  18,  97,
     18, 145,  33,  81,  34, 145,  33,  81,  33, 145,  49,  65,
     34, 145,  49,  65,  34, 145,  49,  49,  50, 145,  50,  17,
     65, 145,  66,  17,  50, 145,  67,  66, 145,  67,  66, 145,
     66,  81, 133,  33,  54,   0,  18,  47,  20, 240, 240, 212,
    101, 115,  98, 147,  97, 145,  33,  97, 145,  34,  65, 161,
     34,  65, 161,  49,  65, 145,  66,  49, 145,  66,  33, 161,
     82,  17, 161,  82,  17, 145,  98,  17, 145, 114, 161, 114,
    161, 114, 132, 113,   0,  16,  37,  15, 240, 240, 101, 145,
     66,  98,  97,  97, 129,  65, 145,  49, 161,  49, 161,  34,
    161,  33, 162,  33, 161,  49, 161,  49, 145,  65, 130,  81,
     98,  98,  65, 149,   0,  12,  32,  30,  16, 240, 240,  90,
    130,  82, 114,  97, 114,  97, 113, 113,  98,  98,  98,  97,
    113,  97, 135, 130, 226, 226, 210, 226, 226, 198,   0,  13,
     32,  48,  15, 240, 240, 116, 146,  65, 113,  97,  97, 129,
     65, 145,  50, 145,  49, 161,  34, 161,  33, 177,  33, 161,
     49, 161,  49, 145,  65,  67,  33,  81,  33,  33,  17, 113,
     17,  34, 149,  49, 161,  33, 177,  33, 180, 179, 240, 240,
     80,  38,  16, 240, 240,  89, 146,  81, 130,  97, 113, 113,
     98, 113,  98,  97, 113,  82, 134, 146,  66, 130,  81, 130,
     81, 129,  97, 114,  97, 114,  82, 114,  82,  33,  37,  83,
      0,  12,  48,  35,  14, 240, 240,  68, 145,  49,  17,  97,
     82,  81,  97,  97,  97,  97,  97,  98, 213, 180, 209, 209,
     81, 113,  81,  97,  97,  97,  81,  17,  65,  97,  36,   0,
     11,  48,  31,  17, 240, 240, 109,  50,  66,  65,  65,  82,
     65,  49,  97,  81,  49,  82,  81, 146, 241, 242, 242, 242,
    241, 242, 242, 242, 241, 215,   0,  13, 112,  38,  16, 240,
    240,  38,  53,  66, 113,  97, 129,  97, 113,  98, 113,  98,
    113,  97, 129,  97, 113,  98, 113,  98, 113,  97, 129,  97,
    113, 113, 113, 113,  97, 145,  50, 180,   0,  13,  32,  35,
     17, 240, 240,  69, 100,  65, 145,  97, 129, 114, 113, 114,
     97, 130,  81, 146,  81, 146,  65, 162,  49, 178,  49, 178,
     33, 209,  33, 211, 226, 242, 241,   0,  14,  48,  56,  23,
      0,   5, 101,  53,  68,  50,  98, 113,  82,  98,  97,  98,
     98,  97,  98,  98,  81, 129,  81,  17,  81, 129,  65,  33,
     65, 145,  65,  33,  49, 161,  49,  49,  49, 162,  17,  65,
     33, 178,  17,  66,  17, 179,  83, 195,  83, 194,  98, 210,
     98, 209, 113,   0,  18,  96,  40,  20, 240, 240, 230,  53,
    130,  98, 178,  65, 210,  65, 210,  49, 242,  17, 240,  19,
    240,  49, 240,  51, 240,  17,  18, 241,  50, 209,  66, 193,
     82, 193,  98, 161, 114, 117,  85,   0,  15,  80,  31,  17,
    240, 240,  69, 100,  66, 129,  98, 113, 114,  97, 146,  65,
    162,  49, 178,  33, 210,  17, 211, 226, 242, 241, 240,  17,
    242, 242, 214,   0,  14,  32,  16, 240, 240, 106,  98,  97,
    113,  98,  97,  98, 113,  82, 210, 210, 225, 225, 226, 210,
     81, 114,  81, 114,  97, 113, 113,  97,  99,  90,   0,  12,
     96,  26,  10, 243, 113, 145, 129, 145, 145, 145, 129, 145,
    145, 145, 129, 145, 145, 145, 129, 145, 145, 145, 129, 145,
    145, 147, 112,  20,   9, 240,  81,  49,  49,  49,  49,  49,
     65,  49,  65,  49,  66,  34,  50,  34,   0,  17,  32,  26,
     10, 243, 145, 145, 129, 145, 145, 145, 129, 145, 145, 145,
    129, 145, 145, 145, 129, 145, 145, 145, 129, 145, 145, 115,
    112,  11,   8, 240,  65,  97,  17,  50,  49,   0,  19,  32,
      7,   4, 194,  34,   0,   9,  96,  12,   5, 193,  49,  49,
     65,  65,  66,  50,   0,   9,  96,  29,  11,   0,  11,  51,
     17,  82,  34,  66,  50,  65,  66,  50,  65,  65,  81,  65,
     66,  65,  65,  33,  33,  50,  33,  51,  34,   0,   8,  80,
     30,   9, 240,  67, 114, 114, 113, 129, 114, 117,  66,  49,
     49,  65,  49,  65,  34,  65,  33,  66,  33,  65,  49,  65,
     49,  49,  83,   0,   7,  48,  21,   9,   0,   9,  52,  66,
     33,  50,  49,  49, 114, 113, 129, 129,  81,  49,  49,  68,
      0,   7,  32,  34,  12, 240, 244, 161, 177, 177, 162, 161,
    115,  17,  98,  34,  82,  50,  81,  65,  82,  65,  81,  81,
     81,  66,  81,  65,  33,  49,  50,  33,  67,  34,   0,   9,
     64,  21,  10,   0,  10,  52,  81,  65,  49,  66,  38,  65,
    145, 145, 145,  97,  49,  50,  68,   0,   8,  16,  30,  13,
    240, 240,  67, 146,  17, 145,  33, 130, 178, 178, 135, 130,
    178, 178, 177, 178, 178, 178, 177, 178, 178, 177, 145,  18,
    147, 240, 240,  96,  30,  11,   0,  11,  67,  17,  81,  50,
     66,  50,  65,  66,  50,  65,  65,  81,  65,  66,  65,  65,
     81,  50, 101, 146, 145,  81,  50,  84, 240, 224,  35,  12,
    240, 179, 177, 162, 162, 161, 177, 162,  19,  99,  33,  98,
     49,  97,  65,  82,  65,  82,  65,  81,  65,  33,  34,  65,
     17,  50,  65,  17,  49,  82,   0,   9,  64,  25,   7, 240,
    146,  82, 240, 147,  65,  17,  49,  33,  49,  18,  81,  97,
     82,  17,  49,  33,  49,  17,  67, 240, 240, 240,  28,  11,
    240, 240, 162, 146, 240, 240, 162, 129,  33,  97,  49,  81,
     65, 146, 145, 161, 146, 145, 161, 146, 145,  97,  34,  99,
    240, 240,  35,  11, 240, 147, 161, 146, 145, 161, 161, 146,
     50,  65,  34,  17,  65,  17, 130, 130,  17, 113,  49,  97,
     49,  33,  34,  49,  17,  50,  49,  17,  49,  81,   0,   8,
     96,  24,   6, 211,  81,  81,  66,  65,  81,  81,  66,  65,
     81,  81,  66,  65,  33,  33,  33,  33,  17,  66, 240, 240,
    144,  45,  20,   0,  20,  34,  35,  51,  97,  17,  17,  49,
     17,  49,  65,  34,  66,  65,  65,  33,  81,  81, 113,  66,
     66,  98,  66,  65, 113,  81,  81,  33,  65,  81,  65,  33,
     66,  66,  65,  33,  66,  65,  98,   0,  15,  64,  33,  14,
      0,  14,  19,  35,  97,  17,  17,  49,  65,  34,  65,  65,
     33,  81,  98,  66,  98,  65, 113,  81,  33,  50,  65,  33,
     66,  65,  33,  65,  98,   0,  11,  25,  10,   0,  10,  52,
     82,  34,  50,  65,  49,  81,  34,  81,  33,  97,  33,  81,
     49,  66,  50,  34,  84,   0,   8,  16,  33,  13,   0,  13,
     50,  35,  81,  34,  49,  65,  33,  65,  98,  65,  97,  81,
     97,  66,  82,  65,  98,  50,  98,  49, 113,  19, 114, 177,
    193, 165, 240, 240,  64,  29,  10,   0,  10,  51,  98,  34,
     50,  50,  49,  66,  34,  65,  49,  81,  49,  66,  49,  65,
     65,  50,  83,  17, 130, 129, 145, 102, 240, 128,  20,  10,
      0,  10,  18,  35,  33,  19,  33,  66, 130, 129, 130, 130,
    129, 145, 130,   0,   8,  64,  22,  10,   0,  10,  52,  81,
     65,  49,  66,  50, 148, 146,  49,  81,  49,  81,  49,  65,
     84,   0,   8,  16,  25,   9, 240, 240,  17, 114, 114, 113,
    129,  87,  66, 114, 113, 129, 114, 113,  49,  65,  49,  65,
     33,  98,   0,   7,  48,  32,  12,   0,  12,  34,  65,  65,
     17,  65,  49,  33,  50,  49,  33,  50,  82,  49,  97,  65,
     97,  50,  97,  49,  33,  65,  34,  33,  82,  34,   0,   9,
     48,  28,  11,   0,  11,  34,  65,  49,  17,  65,  33,  33,
     65,  33,  33,  65,  66,  49,  81,  65,  81,  65,  81,  49,
     97,  34, 114,   0,   9,  39,  15,   0,  15,  34,  65,  49,
     49,  17,  66,  33,  33,  33,  65,  49,  33,  33,  65,  49,
     65,  66,  49,  65,  65,  49,  81,  65,  49,  81,  65,  49,
     81,  50,  33, 115,  35,   0,  11,  96,  26,  12,   0,  12,
     36,  19,  49,  50,  33, 114, 146, 162, 162, 161, 113,  34,
     49,  49,  17,  17,  49,  51,  35,   0,   9,  64,  34,  11,
      0,  11,  34,  65,  49,  17,  65,  33,  33,  50,  33,  33,
     50,  66,  49,  81,  65,  81,  50,  81,  50,  81,  34, 116,
    146,  81,  49,  97,  34,  99, 240, 224,  23,  11,   0,  11,
     50,  49,  65,  35, 145, 145, 130, 129, 145,  81,  66,  49,
     65,  33,  33, 115,   0,   8, 112,   8,  13,   0,  17, 123,
      0,  19,  96,   9,  23,   0,  31,  95,   6,   0,  34,  96,
     13,   9, 240,  82,  33,  50,  33,  65,  33,  65,   0,  21,
     32,  10,   9, 240, 211,  33,  33,  35,   0,  21,  96,
};

//*****************************************************************************
//
// The font definition for the 24 point Cm italic font.
//
//*****************************************************************************
const tFont g_sFontCm24i =
{
    //
    // The format of the font.
    //
    FONT_FMT_PIXEL_RLE,

    //
    // The maximum width of the font.
    //
    20,

    //
    // The height of the font.
    //
    24,

    //
    // The baseline of the font.
    //
    18,

    //
    // The offset to each character in the font.
    //
    {
           0,    4,   25,   43,   87,  118,  178,  229,
         240,  267,  294,  316,  338,  349,  357,  365,
         392,  427,  449,  483,  515,  546,  574,  609,
         638,  671,  705,  718,  734,  754,  765,  795,
         824,  881,  917,  953,  987, 1024, 1060, 1091,
        1125, 1163, 1185, 1211, 1251, 1278, 1339, 1386,
        1423, 1453, 1501, 1539, 1574, 1605, 1643, 1678,
        1734, 1774, 1805, 1837, 1863, 1883, 1909, 1920,
        1927, 1939, 1968, 1998, 2019, 2053, 2074, 2104,
        2134, 2169, 2194, 2222, 2257, 2281, 2326, 2359,
        2384, 2417, 2446, 2466, 2488, 2513, 2545, 2573,
        2612, 2638, 2672, 2695, 2703, 2712, 2725,
    },

    //
    // A pointer to the actual font data
    //
    g_pui8Cm24iData
};
