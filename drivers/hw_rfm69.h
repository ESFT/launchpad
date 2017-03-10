//*****************************************************************************
//
// hw_rfm69.h - Macros used when accessing the HopeRF RFM69W/RFM69H radio
//
//*****************************************************************************

#ifndef __HW_RFM69_H__
#define __HW_RFM69_H__

//*****************************************************************************
//
// The following are defines for the RFM69 register addresses.
//
//*****************************************************************************
#define RFM69_O_FIFO          0x00
#define RFM69_O_OPMODE        0x01
#define RFM69_O_DATAMODUL     0x02
#define RFM69_O_BITRATEMSB    0x03
#define RFM69_O_BITRATELSB    0x04
#define RFM69_O_FDEVMSB       0x05
#define RFM69_O_FDEVLSB       0x06
#define RFM69_O_FRFMSB        0x07
#define RFM69_O_FRFMID        0x08
#define RFM69_O_FRFLSB        0x09
#define RFM69_O_OSC1          0x0A
#define RFM69_O_AFCCTRL       0x0B
#define RFM69_O_LISTEN1       0x0D
#define RFM69_O_LISTEN2       0x0E
#define RFM69_O_LISTEN3       0x0F
#define RFM69_O_VERSION       0x10
#define RFM69_O_PALEVEL       0x11
#define RFM69_O_PARAMP        0x12
#define RFM69_O_OCP           0x13
#define RFM69_O_LNA           0x18
#define RFM69_O_RXBW          0x19
#define RFM69_O_AFCBW         0x1A
#define RFM69_O_OOKPEAK       0x1B
#define RFM69_O_OOKAVG        0x1C
#define RFM69_O_OOKFIX        0x1D
#define RFM69_O_AFCFEI        0x1E
#define RFM69_O_AFCMSB        0x1F
#define RFM69_O_AFCLSB        0x20
#define RFM69_O_FEIMSB        0x21
#define RFM69_O_FEILSB        0x22
#define RFM69_O_RSSICONFIG    0x23
#define RFM69_O_RSSIVALUE     0x24
#define RFM69_O_DIOMAPPING1   0x25
#define RFM69_O_DIOMAPPING2   0x26
#define RFM69_O_IRQFLAGS1     0x27
#define RFM69_O_IRQFLAGS2     0x28
#define RFM69_O_RSSITHRESH    0x29
#define RFM69_O_RXTIMEOUT1    0x2A
#define RFM69_O_RXTIMEOUT2    0x2B
#define RFM69_O_PREAMBLEMSB   0x2C
#define RFM69_O_PREAMBLELSB   0x2D
#define RFM69_O_SYNCCONFIG    0x2E
#define RFM69_O_SYNCVALUE1    0x2F
#define RFM69_O_SYNCVALUE2    0x30
#define RFM69_O_SYNCVALUE3    0x31
#define RFM69_O_SYNCVALUE4    0x32
#define RFM69_O_SYNCVALUE5    0x33
#define RFM69_O_SYNCVALUE6    0x34
#define RFM69_O_SYNCVALUE7    0x35
#define RFM69_O_SYNCVALUE8    0x36
#define RFM69_O_PACKETCONFIG1 0x37
#define RFM69_O_PAYLOADLENGTH 0x38
#define RFM69_O_NODEADRS      0x39
#define RFM69_O_BROADCASTADRS 0x3A
#define RFM69_O_AUTOMODES     0x3B
#define RFM69_O_FIFOTHRESH    0x3C
#define RFM69_O_PACKETCONFIG2 0x3D
#define RFM69_O_AESKEY1       0x3E
#define RFM69_O_AESKEY2       0x3F
#define RFM69_O_AESKEY3       0x40
#define RFM69_O_AESKEY4       0x41
#define RFM69_O_AESKEY5       0x42
#define RFM69_O_AESKEY6       0x43
#define RFM69_O_AESKEY7       0x44
#define RFM69_O_AESKEY8       0x45
#define RFM69_O_AESKEY9       0x46
#define RFM69_O_AESKEY10      0x47
#define RFM69_O_AESKEY11      0x48
#define RFM69_O_AESKEY12      0x49
#define RFM69_O_AESKEY13      0x4A
#define RFM69_O_AESKEY14      0x4B
#define RFM69_O_AESKEY15      0x4C
#define RFM69_O_AESKEY16      0x4D
#define RFM69_O_TEMP1         0x4E
#define RFM69_O_TEMP2         0x4F
#define RFM69_O_TESTLNA       0x58
#define RFM69_O_TESTPA1       0x5A
#define RFM69_O_TESTPA2       0x5C
#define RFM69_O_TESTDAGC      0x6F

//*****************************************************************************
//
// The following are defines for the bit fields in the RFM69_O_AFCMSB register.
//
//*****************************************************************************
#define RFM69_FIFO_M            0xFF        // FIFO data input/output
#define RFM69_FIFO_S            0

//*****************************************************************************
//
// The following are defines for the bit fields in the RFM69_O_OPMODE register.
//
//*****************************************************************************
#define RFM69_OPMODE_SEQUENCER_OFF       0x80
#define RFM69_OPMODE_SEQUENCER_ON        0x00  // Default
#define RFM69_OPMODE_LISTEN_ON           0x40
#define RFM69_OPMODE_LISTEN_OFF          0x00  // Default
#define RFM69_OPMODE_LISTENABORT         0x20
#define RFM69_OPMODE_MODE_M              0x1C
#define RFM69_OPMODE_MODE_S                 2
#define RFM69_OPMODE_MODE_SLEEP          0x00
#define RFM69_OPMODE_MODE_STANDBY        0x04  // Default
#define RFM69_OPMODE_MODE_SYNTHESIZER    0x08
#define RFM69_OPMODE_MODE_TRANSMITTER    0x0C
#define RFM69_OPMODE_MODE_RECEIVER       0x10

//*****************************************************************************
//
// The following are defines for the bit fields in the RFM69_O_DATAMODUL register.
//
//*****************************************************************************
#define RFM69_DATAMODUL_DATAMODE_M                 0x60
#define RFM69_DATAMODUL_DATAMODE_S                    5
#define RFM69_DATAMODUL_DATAMODE_PACKET            0x00  // Default
#define RFM69_DATAMODUL_DATAMODE_CONTINUOUS        0x40
#define RFM69_DATAMODUL_DATAMODE_CONTINUOUSNOBSYNC 0x60

#define RFM69_DATAMODUL_MODULATIONTYPE_M           0x18
#define RFM69_DATAMODUL_MODULATIONTYPE_S              3
#define RFM69_DATAMODUL_MODULATIONTYPE_FSK         0x00  // Default
#define RFM69_DATAMODUL_MODULATIONTYPE_OOK         0x08

#define RFM69_DATAMODUL_MODULATIONSHAPING_M                  0x03
#define RFM69_DATAMODUL_MODULATIONSHAPING_S                     0
#define RFM69_DATAMODUL_MODULATIONSHAPING_NO_SHAPING         0x00  // Default
#define RFM69_DATAMODUL_MODULATIONSHAPING_OOK_FILTER_BR      0x01
#define RFM69_DATAMODUL_MODULATIONSHAPING_OOK_FILTER_2BR     0x02
#define RFM69_DATAMODUL_MODULATIONSHAPING_FSK_GAUSSIAN_BT1_0 0x01
#define RFM69_DATAMODUL_MODULATIONSHAPING_FSK_GAUSSIAN_BT0_5 0x02
#define RFM69_DATAMODUL_MODULATIONSHAPING_FSK_GAUSSIAN_BT0_3 0x03

//*****************************************************************************
//
// The following are defines for the bit fields in the RFM69_O_BITRATEMSB register.
//
//*****************************************************************************
#define RFM69_BITRATEMSB_M               0xFF
#define RFM69_BITRATEMSB_S                  0

#define RFM69_BITRATEMSB_1200            0x68
#define RFM69_BITRATEMSB_1200            0x68
#define RFM69_BITRATEMSB_2400            0x34
#define RFM69_BITRATEMSB_4800            0x1A  // Default
#define RFM69_BITRATEMSB_9600            0x0D
#define RFM69_BITRATEMSB_12500           0x0A
#define RFM69_BITRATEMSB_19200           0x06
#define RFM69_BITRATEMSB_25000           0x05
#define RFM69_BITRATEMSB_32768           0x03
#define RFM69_BITRATEMSB_34482           0x03
#define RFM69_BITRATEMSB_38400           0x03
#define RFM69_BITRATEMSB_38323           0x03
#define RFM69_BITRATEMSB_50000           0x02
#define RFM69_BITRATEMSB_55555           0x02
#define RFM69_BITRATEMSB_57600           0x02
#define RFM69_BITRATEMSB_76800           0x01
#define RFM69_BITRATEMSB_153600          0x00
#define RFM69_BITRATEMSB_115200          0x01
#define RFM69_BITRATEMSB_100000          0x01
#define RFM69_BITRATEMSB_150000          0x00
#define RFM69_BITRATEMSB_200000          0x00
#define RFM69_BITRATEMSB_250000          0x00
#define RFM69_BITRATEMSB_300000          0x00

//*****************************************************************************
//
// The following are defines for the bit fields in the RFM69_O_BITRATELSB register.
//
//*****************************************************************************
#define RFM69_BITRATELSB_M               0xFF
#define RFM69_BITRATELSB_S                  0

#define RFM69_BITRATELSB_1200            0x2B
#define RFM69_BITRATELSB_2400            0x15
#define RFM69_BITRATELSB_4800            0x0B  // Default
#define RFM69_BITRATELSB_9600            0x05
#define RFM69_BITRATELSB_12500           0x00
#define RFM69_BITRATELSB_19200           0x83
#define RFM69_BITRATELSB_32768           0xD1
#define RFM69_BITRATELSB_34482           0xA0
#define RFM69_BITRATELSB_38400           0x41
#define RFM69_BITRATELSB_38323           0x43
#define RFM69_BITRATELSB_25000           0x00
#define RFM69_BITRATELSB_50000           0x80
#define RFM69_BITRATELSB_55555           0x40
#define RFM69_BITRATELSB_57600           0x2C
#define RFM69_BITRATELSB_76800           0xA1
#define RFM69_BITRATELSB_100000          0x40
#define RFM69_BITRATELSB_115200          0x16
#define RFM69_BITRATELSB_150000          0xD5
#define RFM69_BITRATELSB_153600          0xD0
#define RFM69_BITRATELSB_200000          0xA0
#define RFM69_BITRATELSB_250000          0x80
#define RFM69_BITRATELSB_300000          0x6B

//*****************************************************************************
//
// The following are defines for the bit fields in the RFM69_O_FDEVMSB register.
//
//*****************************************************************************
#define RFM69_FDEVMSB_M                0x3F
#define RFM69_FDEVMSB_S                   0

#define RFM69_FDEVMSB_10000            0x00
#define RFM69_FDEVMSB_100000           0x06
#define RFM69_FDEVMSB_110000           0x07
#define RFM69_FDEVMSB_120000           0x07
#define RFM69_FDEVMSB_130000           0x08
#define RFM69_FDEVMSB_140000           0x08
#define RFM69_FDEVMSB_15000            0x00
#define RFM69_FDEVMSB_150000           0x09
#define RFM69_FDEVMSB_160000           0x0A
#define RFM69_FDEVMSB_170000           0x0A
#define RFM69_FDEVMSB_180000           0x0B
#define RFM69_FDEVMSB_190000           0x0C
#define RFM69_FDEVMSB_2000             0x00
#define RFM69_FDEVMSB_20000            0x01
#define RFM69_FDEVMSB_200000           0x0C
#define RFM69_FDEVMSB_210000           0x0D
#define RFM69_FDEVMSB_220000           0x0E
#define RFM69_FDEVMSB_230000           0x0E
#define RFM69_FDEVMSB_240000           0x0F
#define RFM69_FDEVMSB_25000            0x01
#define RFM69_FDEVMSB_250000           0x10
#define RFM69_FDEVMSB_260000           0x10
#define RFM69_FDEVMSB_270000           0x11
#define RFM69_FDEVMSB_280000           0x11
#define RFM69_FDEVMSB_290000           0x12
#define RFM69_FDEVMSB_30000            0x01
#define RFM69_FDEVMSB_300000           0x13
#define RFM69_FDEVMSB_35000            0x02
#define RFM69_FDEVMSB_40000            0x02
#define RFM69_FDEVMSB_45000            0x02
#define RFM69_FDEVMSB_5000             0x00  // Default
#define RFM69_FDEVMSB_50000            0x03
#define RFM69_FDEVMSB_55000            0x03
#define RFM69_FDEVMSB_60000            0x03
#define RFM69_FDEVMSB_65000            0x04
#define RFM69_FDEVMSB_70000            0x04
#define RFM69_FDEVMSB_7500             0x00
#define RFM69_FDEVMSB_75000            0x04
#define RFM69_FDEVMSB_80000            0x05
#define RFM69_FDEVMSB_85000            0x05
#define RFM69_FDEVMSB_90000            0x05
#define RFM69_FDEVMSB_95000            0x06

//*****************************************************************************
//
// The following are defines for the bit fields in the RFM69_O_FDEVLSB register.
//
//*****************************************************************************
#define RFM69_FDEVLSB_M                0xFF
#define RFM69_FDEVLSB_S                   0

#define RFM69_FDEVLSB_10000            0xA4
#define RFM69_FDEVLSB_100000           0x66
#define RFM69_FDEVLSB_110000           0x0A
#define RFM69_FDEVLSB_120000           0xAE
#define RFM69_FDEVLSB_130000           0x52
#define RFM69_FDEVLSB_140000           0xF6
#define RFM69_FDEVLSB_15000            0xF6
#define RFM69_FDEVLSB_150000           0x9A
#define RFM69_FDEVLSB_160000           0x3D
#define RFM69_FDEVLSB_170000           0xE1
#define RFM69_FDEVLSB_180000           0x85
#define RFM69_FDEVLSB_190000           0x29
#define RFM69_FDEVLSB_2000             0x21
#define RFM69_FDEVLSB_20000            0x48
#define RFM69_FDEVLSB_200000           0xCD
#define RFM69_FDEVLSB_210000           0x71
#define RFM69_FDEVLSB_220000           0x14
#define RFM69_FDEVLSB_230000           0xB8
#define RFM69_FDEVLSB_240000           0x5C
#define RFM69_FDEVLSB_25000            0x9A
#define RFM69_FDEVLSB_250000           0x00
#define RFM69_FDEVLSB_260000           0xA4
#define RFM69_FDEVLSB_270000           0x48
#define RFM69_FDEVLSB_280000           0xEC
#define RFM69_FDEVLSB_290000           0x8F
#define RFM69_FDEVLSB_30000            0xEC
#define RFM69_FDEVLSB_300000           0x33
#define RFM69_FDEVLSB_35000            0x3D
#define RFM69_FDEVLSB_40000            0x8F
#define RFM69_FDEVLSB_45000            0xE1
#define RFM69_FDEVLSB_5000             0x52  // Default
#define RFM69_FDEVLSB_50000            0x33
#define RFM69_FDEVLSB_55000            0x85
#define RFM69_FDEVLSB_60000            0xD7
#define RFM69_FDEVLSB_65000            0x29
#define RFM69_FDEVLSB_70000            0x7B
#define RFM69_FDEVLSB_7500             0x7B
#define RFM69_FDEVLSB_75000            0xCD
#define RFM69_FDEVLSB_80000            0x1F
#define RFM69_FDEVLSB_85000            0x71
#define RFM69_FDEVLSB_90000            0xC3
#define RFM69_FDEVLSB_95000            0x14

//*****************************************************************************
//
// The following are defines for the bit fields in the RFM69_O_FRFMSB register.
//
//*****************************************************************************
#define RFM69_FRFMSB_M               0xFF
#define RFM69_FRFMSB_S                  0

#define RFM69_FRFMSB_314             0x4E
#define RFM69_FRFMSB_315             0x4E
#define RFM69_FRFMSB_316             0x4F
#define RFM69_FRFMSB_433             0x6C
#define RFM69_FRFMSB_434             0x6C
#define RFM69_FRFMSB_435             0x6C
#define RFM69_FRFMSB_863             0xD7
#define RFM69_FRFMSB_864             0xD8
#define RFM69_FRFMSB_865             0xD8
#define RFM69_FRFMSB_866             0xD8
#define RFM69_FRFMSB_867             0xD8
#define RFM69_FRFMSB_868             0xD9
#define RFM69_FRFMSB_869             0xD9
#define RFM69_FRFMSB_870             0xD9
#define RFM69_FRFMSB_902             0xE1
#define RFM69_FRFMSB_903             0xE1
#define RFM69_FRFMSB_904             0xE2
#define RFM69_FRFMSB_905             0xE2
#define RFM69_FRFMSB_906             0xE2
#define RFM69_FRFMSB_907             0xE2
#define RFM69_FRFMSB_908             0xE3
#define RFM69_FRFMSB_909             0xE3
#define RFM69_FRFMSB_910             0xE3
#define RFM69_FRFMSB_911             0xE3
#define RFM69_FRFMSB_912             0xE4
#define RFM69_FRFMSB_913             0xE4
#define RFM69_FRFMSB_914             0xE4
#define RFM69_FRFMSB_915             0xE4  // Default
#define RFM69_FRFMSB_916             0xE5
#define RFM69_FRFMSB_917             0xE5
#define RFM69_FRFMSB_918             0xE5
#define RFM69_FRFMSB_919             0xE5
#define RFM69_FRFMSB_920             0xE6
#define RFM69_FRFMSB_921             0xE6
#define RFM69_FRFMSB_922             0xE6
#define RFM69_FRFMSB_923             0xE6
#define RFM69_FRFMSB_924             0xE7
#define RFM69_FRFMSB_925             0xE7
#define RFM69_FRFMSB_926             0xE7
#define RFM69_FRFMSB_927             0xE7
#define RFM69_FRFMSB_928             0xE8

//*****************************************************************************
//
// The following are defines for the bit fields in the RFM69_O_FRFMID register.
//
//*****************************************************************************
#define RFM69_FRFMID_M               0xFF
#define RFM69_FRFMID_S                  0

#define RFM69_FRFMID_314             0x80
#define RFM69_FRFMID_315             0xC0
#define RFM69_FRFMID_316             0x00
#define RFM69_FRFMID_433             0x40
#define RFM69_FRFMID_434             0x80
#define RFM69_FRFMID_435             0xC0
#define RFM69_FRFMID_863             0xC0
#define RFM69_FRFMID_864             0x00
#define RFM69_FRFMID_865             0x40
#define RFM69_FRFMID_866             0x80
#define RFM69_FRFMID_867             0xC0
#define RFM69_FRFMID_868             0x00
#define RFM69_FRFMID_869             0x40
#define RFM69_FRFMID_870             0x80
#define RFM69_FRFMID_902             0x80
#define RFM69_FRFMID_903             0xC0
#define RFM69_FRFMID_904             0x00
#define RFM69_FRFMID_905             0x40
#define RFM69_FRFMID_906             0x80
#define RFM69_FRFMID_907             0xC0
#define RFM69_FRFMID_908             0x00
#define RFM69_FRFMID_909             0x40
#define RFM69_FRFMID_910             0x80
#define RFM69_FRFMID_911             0xC0
#define RFM69_FRFMID_912             0x00
#define RFM69_FRFMID_913             0x40
#define RFM69_FRFMID_914             0x80
#define RFM69_FRFMID_915             0xC0  // Default
#define RFM69_FRFMID_916             0x00
#define RFM69_FRFMID_917             0x40
#define RFM69_FRFMID_918             0x80
#define RFM69_FRFMID_919             0xC0
#define RFM69_FRFMID_920             0x00
#define RFM69_FRFMID_921             0x40
#define RFM69_FRFMID_922             0x80
#define RFM69_FRFMID_923             0xC0
#define RFM69_FRFMID_924             0x00
#define RFM69_FRFMID_925             0x40
#define RFM69_FRFMID_926             0x80
#define RFM69_FRFMID_927             0xC0
#define RFM69_FRFMID_928             0x00

//*****************************************************************************
//
// The following are defines for the bit fields in the RFM69_O_FRFLSB register.
//
//*****************************************************************************
#define RFM69_FRFLSB_M               0xFF
#define RFM69_FRFLSB_S                  0

#define RFM69_FRFLSB_314             0x00
#define RFM69_FRFLSB_315             0x00
#define RFM69_FRFLSB_316             0x00
#define RFM69_FRFLSB_433             0x00
#define RFM69_FRFLSB_434             0x00
#define RFM69_FRFLSB_435             0x00
#define RFM69_FRFLSB_863             0x00
#define RFM69_FRFLSB_864             0x00
#define RFM69_FRFLSB_865             0x00
#define RFM69_FRFLSB_866             0x00
#define RFM69_FRFLSB_867             0x00
#define RFM69_FRFLSB_868             0x00
#define RFM69_FRFLSB_869             0x00
#define RFM69_FRFLSB_870             0x00
#define RFM69_FRFLSB_902             0x00
#define RFM69_FRFLSB_903             0x00
#define RFM69_FRFLSB_904             0x00
#define RFM69_FRFLSB_905             0x00
#define RFM69_FRFLSB_906             0x00
#define RFM69_FRFLSB_907             0x00
#define RFM69_FRFLSB_908             0x00
#define RFM69_FRFLSB_909             0x00
#define RFM69_FRFLSB_910             0x00
#define RFM69_FRFLSB_911             0x00
#define RFM69_FRFLSB_912             0x00
#define RFM69_FRFLSB_913             0x00
#define RFM69_FRFLSB_914             0x00
#define RFM69_FRFLSB_915             0x00  // Default
#define RFM69_FRFLSB_916             0x00
#define RFM69_FRFLSB_917             0x00
#define RFM69_FRFLSB_918             0x00
#define RFM69_FRFLSB_919             0x00
#define RFM69_FRFLSB_920             0x00
#define RFM69_FRFLSB_921             0x00
#define RFM69_FRFLSB_922             0x00
#define RFM69_FRFLSB_923             0x00
#define RFM69_FRFLSB_924             0x00
#define RFM69_FRFLSB_925             0x00
#define RFM69_FRFLSB_926             0x00
#define RFM69_FRFLSB_927             0x00
#define RFM69_FRFLSB_928             0x00

//*****************************************************************************
//
// The following are defines for the bit fields in the RFM69_O_OSC1 register.
//
//*****************************************************************************
#define RFM69_OSC1_RCCAL_START       0x80
#define RFM69_OSC1_RCCAL_DONE        0x40

//*****************************************************************************
//
// The following are defines for the bit fields in the RFM69_O_AFCCTRL register.
//
//*****************************************************************************
#define RFM69_AFCCTRL_LOWBETA_OFF    0x00  // Default
#define RFM69_AFCCTRL_LOWBETA_ON     0x20

//*****************************************************************************
//
// The following are defines for the bit fields in the RFM69_O_LISTEN1 register.
//
//*****************************************************************************
#define RFM69_LISTEN1_RESOL_M           0xF0
#define RFM69_LISTEN1_RESOL_S              4
#define RFM69_LISTEN1_RESOL_64          0x50
#define RFM69_LISTEN1_RESOL_4100        0xA0  // Default
#define RFM69_LISTEN1_RESOL_262000      0xF0

#define RFM69_LISTEN1_RESOL_IDLE_M      0xC0
#define RFM69_LISTEN1_RESOL_IDLE_S         6
#define RFM69_LISTEN1_RESOL_IDLE_64     0x40
#define RFM69_LISTEN1_RESOL_IDLE_4100   0x80  // Default
#define RFM69_LISTEN1_RESOL_IDLE_262000 0xC0

#define RFM69_LISTEN1_RESOL_RX_M        0x30
#define RFM69_LISTEN1_RESOL_RX_S           4
#define RFM69_LISTEN1_RESOL_RX_64       0x10
#define RFM69_LISTEN1_RESOL_RX_4100     0x20  // Default
#define RFM69_LISTEN1_RESOL_RX_262000   0x30

#define RFM69_LISTEN1_CRITERIA_RSSI        0x00  // Default
#define RFM69_LISTEN1_CRITERIA_RSSIANDSYNC 0x08

#define RFM69_LISTEN1_END_M                                           0x06
#define RFM69_LISTEN1_END_S                                              1
#define RFM69_LISTEN1_END_CHIP_STAY_LISTEN_STOP                       0x00
#define RFM69_LISTEN1_END_CHIP_STAY_UNTIL_PAYLOAD_OR_TO_LISTEN_STOP   0x02 // Default
#define RFM69_LISTEN1_END_CHIP_STAY_UNTIL_PAYLOAD_OR_TO_LISTEN_RESUME 0x04 

//*****************************************************************************
//
// The following are defines for the bit fields in the RFM69_O_LISTEN2 register.
//
//*****************************************************************************
#define RFM69_LISTEN2_M                      0xFF
#define RFM69_LISTEN2_S                         0
#define RFM69_LISTEN2_COEFIDLE_VALUE         0xF5 // Default

//*****************************************************************************
//
// The following are defines for the bit fields in the RFM69_O_LISTEN3 register.
//
//*****************************************************************************
#define RFM69_LISTEN3_M                      0xFF
#define RFM69_LISTEN3_S                         0
#define RFM69_LISTEN3_COEFRX_VALUE           0x20 // Default

//*****************************************************************************
//
// The following are defines for the bit fields in the RFM69_O_VERSION register.
//
//*****************************************************************************
#define RFM69_VERSION_M          0xFF
#define RFM69_VERSION_S             0
#define RFM69_VERSION_VER        0x24  // Default

//*****************************************************************************
//
// The following are defines for the bit fields in the RFM69_O_PALEVEL register.
//
//*****************************************************************************
#define RFM69_PALEVEL_PA0_ON     0x80  // Default
#define RFM69_PALEVEL_PA0_OFF    0x00
#define RFM69_PALEVEL_PA1_ON     0x40
#define RFM69_PALEVEL_PA1_OFF    0x00  // Default
#define RFM69_PALEVEL_PA2_ON     0x20
#define RFM69_PALEVEL_PA2_OFF    0x00  // Default

#define RFM69_PALEVEL_OUTPUTPOWER_M          0x1F
#define RFM69_PALEVEL_OUTPUTPOWER_S             0
#define RFM69_PALEVEL_OUTPUTPOWER_0          0x00
#define RFM69_PALEVEL_OUTPUTPOWER_1          0x01
#define RFM69_PALEVEL_OUTPUTPOWER_2          0x02
#define RFM69_PALEVEL_OUTPUTPOWER_3          0x03
#define RFM69_PALEVEL_OUTPUTPOWER_4          0x04
#define RFM69_PALEVEL_OUTPUTPOWER_5          0x05
#define RFM69_PALEVEL_OUTPUTPOWER_6          0x06
#define RFM69_PALEVEL_OUTPUTPOWER_7          0x07
#define RFM69_PALEVEL_OUTPUTPOWER_8          0x08
#define RFM69_PALEVEL_OUTPUTPOWER_9          0x09
#define RFM69_PALEVEL_OUTPUTPOWER_10         0x0A
#define RFM69_PALEVEL_OUTPUTPOWER_11         0x0B
#define RFM69_PALEVEL_OUTPUTPOWER_12         0x0C
#define RFM69_PALEVEL_OUTPUTPOWER_13         0x0D
#define RFM69_PALEVEL_OUTPUTPOWER_14         0x0E
#define RFM69_PALEVEL_OUTPUTPOWER_15         0x0F
#define RFM69_PALEVEL_OUTPUTPOWER_16         0x10
#define RFM69_PALEVEL_OUTPUTPOWER_17         0x11
#define RFM69_PALEVEL_OUTPUTPOWER_18         0x12
#define RFM69_PALEVEL_OUTPUTPOWER_19         0x13
#define RFM69_PALEVEL_OUTPUTPOWER_20         0x14
#define RFM69_PALEVEL_OUTPUTPOWER_21         0x15
#define RFM69_PALEVEL_OUTPUTPOWER_22         0x16
#define RFM69_PALEVEL_OUTPUTPOWER_23         0x17
#define RFM69_PALEVEL_OUTPUTPOWER_24         0x18
#define RFM69_PALEVEL_OUTPUTPOWER_25         0x19
#define RFM69_PALEVEL_OUTPUTPOWER_26         0x1A
#define RFM69_PALEVEL_OUTPUTPOWER_27         0x1B
#define RFM69_PALEVEL_OUTPUTPOWER_28         0x1C
#define RFM69_PALEVEL_OUTPUTPOWER_29         0x1D
#define RFM69_PALEVEL_OUTPUTPOWER_30         0x1E
#define RFM69_PALEVEL_OUTPUTPOWER_31         0x1F  // Default

//*****************************************************************************
//
// The following are defines for the bit fields in the RFM69_O_PARAMP register.
//
//*****************************************************************************
#define RFM69_PARAMP_M               0x0F
#define RFM69_PARAMP_S                  0

#define RFM69_PARAMP_3400            0x00
#define RFM69_PARAMP_2000            0x01
#define RFM69_PARAMP_1000            0x02
#define RFM69_PARAMP_500             0x03
#define RFM69_PARAMP_250             0x04
#define RFM69_PARAMP_125             0x05
#define RFM69_PARAMP_100             0x06
#define RFM69_PARAMP_62              0x07
#define RFM69_PARAMP_50              0x08
#define RFM69_PARAMP_40              0x09  // Default
#define RFM69_PARAMP_31              0x0A
#define RFM69_PARAMP_25              0x0B
#define RFM69_PARAMP_20              0x0C
#define RFM69_PARAMP_15              0x0D
#define RFM69_PARAMP_12              0x0E
#define RFM69_PARAMP_10              0x0F

//*****************************************************************************
//
// The following are defines for the bit fields in the RFM69_O_OCP register.
//
//*****************************************************************************
#define RFM69_OCP_OFF                0x0F
#define RFM69_OCP_ON                 0x1A  // Default

#define RFM69_OCP_TRIM_M             0x0F
#define RFM69_OCP_TRIM_S                0
#define RFM69_OCP_TRIM_45            0x00
#define RFM69_OCP_TRIM_50            0x01
#define RFM69_OCP_TRIM_55            0x02
#define RFM69_OCP_TRIM_60            0x03
#define RFM69_OCP_TRIM_65            0x04
#define RFM69_OCP_TRIM_70            0x05
#define RFM69_OCP_TRIM_75            0x06
#define RFM69_OCP_TRIM_80            0x07
#define RFM69_OCP_TRIM_85            0x08
#define RFM69_OCP_TRIM_90            0x09
#define RFM69_OCP_TRIM_95            0x0A  // Default
#define RFM69_OCP_TRIM_100           0x0B
#define RFM69_OCP_TRIM_105           0x0C
#define RFM69_OCP_TRIM_110           0x0D
#define RFM69_OCP_TRIM_115           0x0E
#define RFM69_OCP_TRIM_120           0x0F

//*****************************************************************************
//
// The following are defines for the bit fields in the RFM69_O_LNA  register.
//
//*****************************************************************************
#define RFM69_LNA_ZIN_50                 0x00  // Reset value
#define RFM69_LNA_ZIN_200                0x80  // Recommended default

#define RFM69_LNA_CURRENTGAIN_M          0x38
#define RFM69_LNA_CURRENTGAIN_S             3

#define RFM69_LNA_GAINSELECT_M           0x07
#define RFM69_LNA_GAINSELECT_S              0
#define RFM69_LNA_GAINSELECT_AUTO        0x00  // Default
#define RFM69_LNA_GAINSELECT_MAX         0x01
#define RFM69_LNA_GAINSELECT_MAXMINUS6   0x02
#define RFM69_LNA_GAINSELECT_MAXMINUS12  0x03
#define RFM69_LNA_GAINSELECT_MAXMINUS24  0x04
#define RFM69_LNA_GAINSELECT_MAXMINUS36  0x05
#define RFM69_LNA_GAINSELECT_MAXMINUS48  0x06

//*****************************************************************************
//
// The following are defines for the bit fields in the RFM69_O_RXBW register.
//
//*****************************************************************************
#define RFM69_RXBW_DCCFREQ_M             0xE0
#define RFM69_RXBW_DCCFREQ_S                5
#define RFM69_RXBW_DCCFREQ_000           0x00
#define RFM69_RXBW_DCCFREQ_000           0x00
#define RFM69_RXBW_DCCFREQ_001           0x20
#define RFM69_RXBW_DCCFREQ_010           0x40  // Recommended default
#define RFM69_RXBW_DCCFREQ_011           0x60
#define RFM69_RXBW_DCCFREQ_100           0x80  // Reset value
#define RFM69_RXBW_DCCFREQ_101           0xA0
#define RFM69_RXBW_DCCFREQ_110           0xC0
#define RFM69_RXBW_DCCFREQ_111           0xE0

#define RFM69_RXBW_MANT_M                0x11
#define RFM69_RXBW_MANT_S                   3
#define RFM69_RXBW_MANT_16               0x00  // Reset value
#define RFM69_RXBW_MANT_20               0x08
#define RFM69_RXBW_MANT_24               0x10  // Recommended default

#define RFM69_RXBW_EXP_M                 0x07
#define RFM69_RXBW_EXP_S                    0
#define RFM69_RXBW_EXP_0                 0x00
#define RFM69_RXBW_EXP_1                 0x01
#define RFM69_RXBW_EXP_2                 0x02
#define RFM69_RXBW_EXP_3                 0x03
#define RFM69_RXBW_EXP_4                 0x04
#define RFM69_RXBW_EXP_5                 0x05  // Recommended default
#define RFM69_RXBW_EXP_6                 0x06  // Reset value
#define RFM69_RXBW_EXP_7                 0x07

//*****************************************************************************
//
// The following are defines for the bit fields in the RFM69_O_AFCBW register.
//
//*****************************************************************************
#define RFM69_AFCBW_DCCFREQAFC_000       0x00
#define RFM69_AFCBW_DCCFREQAFC_001       0x20
#define RFM69_AFCBW_DCCFREQAFC_010       0x40
#define RFM69_AFCBW_DCCFREQAFC_011       0x60
#define RFM69_AFCBW_DCCFREQAFC_100       0x80  // Default
#define RFM69_AFCBW_DCCFREQAFC_101       0xA0
#define RFM69_AFCBW_DCCFREQAFC_110       0xC0
#define RFM69_AFCBW_DCCFREQAFC_111       0xE0

#define RFM69_AFCBW_MANTAFC_16           0x00
#define RFM69_AFCBW_MANTAFC_20           0x08  // Default
#define RFM69_AFCBW_MANTAFC_24           0x10

#define RFM69_AFCBW_EXPAFC_0             0x00
#define RFM69_AFCBW_EXPAFC_1             0x01
#define RFM69_AFCBW_EXPAFC_2             0x02  // Reset value
#define RFM69_AFCBW_EXPAFC_3             0x03  // Recommended default
#define RFM69_AFCBW_EXPAFC_4             0x04
#define RFM69_AFCBW_EXPAFC_5             0x05
#define RFM69_AFCBW_EXPAFC_6             0x06
#define RFM69_AFCBW_EXPAFC_7             0x07

//*****************************************************************************
//
// The following are defines for the bit fields in the RFM69_O_OOKPEAK register.
//
//*****************************************************************************
#define RFM69_OOKPEAK_THRESHTYPE_FIXED       0x00
#define RFM69_OOKPEAK_THRESHTYPE_PEAK        0x40  // Default
#define RFM69_OOKPEAK_THRESHTYPE_AVERAGE     0x80

#define RFM69_OOKPEAK_PEAKTHRESHSTEP_000     0x00  // Default
#define RFM69_OOKPEAK_PEAKTHRESHSTEP_001     0x08
#define RFM69_OOKPEAK_PEAKTHRESHSTEP_010     0x10
#define RFM69_OOKPEAK_PEAKTHRESHSTEP_011     0x18
#define RFM69_OOKPEAK_PEAKTHRESHSTEP_100     0x20
#define RFM69_OOKPEAK_PEAKTHRESHSTEP_101     0x28
#define RFM69_OOKPEAK_PEAKTHRESHSTEP_110     0x30
#define RFM69_OOKPEAK_PEAKTHRESHSTEP_111     0x38

#define RFM69_OOKPEAK_PEAKTHRESHDEC_000      0x00  // Default
#define RFM69_OOKPEAK_PEAKTHRESHDEC_001      0x01
#define RFM69_OOKPEAK_PEAKTHRESHDEC_010      0x02
#define RFM69_OOKPEAK_PEAKTHRESHDEC_011      0x03
#define RFM69_OOKPEAK_PEAKTHRESHDEC_100      0x04
#define RFM69_OOKPEAK_PEAKTHRESHDEC_101      0x05
#define RFM69_OOKPEAK_PEAKTHRESHDEC_110      0x06
#define RFM69_OOKPEAK_PEAKTHRESHDEC_111      0x07

//*****************************************************************************
//
// The following are defines for the bit fields in the RFM69_O_OOKAVG register.
//
//*****************************************************************************
#define RFM69_OOKAVG_AVERAGETHRESHFILT_00    0x00
#define RFM69_OOKAVG_AVERAGETHRESHFILT_01    0x40
#define RFM69_OOKAVG_AVERAGETHRESHFILT_10    0x80  // Default
#define RFM69_OOKAVG_AVERAGETHRESHFILT_11    0xC0

//*****************************************************************************
//
// The following are defines for the bit fields in the RFM69_O_OOKFIX register.
//
//*****************************************************************************
#define RFM69_OOKFIX_FIXEDTHRESH_VALUE       0x06  // Default

//*****************************************************************************
//
// The following are defines for the bit fields in the RFM69_O_AFCFEI register.
//
//*****************************************************************************
#define RFM69_AFCFEI_FEI_DONE                0x40
#define RFM69_AFCFEI_FEI_START               0x20
#define RFM69_AFCFEI_AFC_DONE                0x10
#define RFM69_AFCFEI_AFCAUTOCLEAR_ON         0x08
#define RFM69_AFCFEI_AFCAUTOCLEAR_OFF        0x00  // Default

#define RFM69_AFCFEI_AFCAUTO_ON              0x04
#define RFM69_AFCFEI_AFCAUTO_OFF             0x00  // Default

#define RFM69_AFCFEI_AFC_CLEAR               0x02
#define RFM69_AFCFEI_AFC_START               0x01

//*****************************************************************************
//
// The following are defines for the bit fields in the RFM69_O_RSSICONFIG register.
//
//*****************************************************************************
#define RFM69_RSSICONFIG_FASTRX_OFF                0x00  // Default
#define RFM69_RSSICONFIG_DONE                      0x02
#define RFM69_RSSICONFIG_START                     0x01

//*****************************************************************************
//
// The following are defines for the bit fields in the RFM69_O_DIOMAPPING1 register.
//
//*****************************************************************************
#define RFM69_DIOMAPPING1_DIO0_00            0x00  // Default
#define RFM69_DIOMAPPING1_DIO0_01            0x40
#define RFM69_DIOMAPPING1_DIO0_10            0x80
#define RFM69_DIOMAPPING1_DIO0_11            0xC0

#define RFM69_DIOMAPPING1_DIO1_00            0x00  // Default
#define RFM69_DIOMAPPING1_DIO1_01            0x10
#define RFM69_DIOMAPPING1_DIO1_10            0x20
#define RFM69_DIOMAPPING1_DIO1_11            0x30

#define RFM69_DIOMAPPING1_DIO2_00            0x00  // Default
#define RFM69_DIOMAPPING1_DIO2_01            0x04
#define RFM69_DIOMAPPING1_DIO2_10            0x08
#define RFM69_DIOMAPPING1_DIO2_11            0x0C

#define RFM69_DIOMAPPING1_DIO3_00            0x00  // Default
#define RFM69_DIOMAPPING1_DIO3_01            0x01
#define RFM69_DIOMAPPING1_DIO3_10            0x02
#define RFM69_DIOMAPPING1_DIO3_11            0x03

//*****************************************************************************
//
// The following are defines for the bit fields in the RFM69_O_DIOMAPPING2 register.
//
//*****************************************************************************
#define RFM69_DIOMAPPING2_DIO4_00            0x00  // Default
#define RFM69_DIOMAPPING2_DIO4_01            0x40
#define RFM69_DIOMAPPING2_DIO4_10            0x80
#define RFM69_DIOMAPPING2_DIO4_11            0xC0

#define RFM69_DIOMAPPING2_DIO5_00            0x00  // Default
#define RFM69_DIOMAPPING2_DIO5_01            0x10
#define RFM69_DIOMAPPING2_DIO5_10            0x20
#define RFM69_DIOMAPPING2_DIO5_11            0x30

#define RFM69_DIOMAPPING2_CLKOUT_32          0x00
#define RFM69_DIOMAPPING2_CLKOUT_16          0x01
#define RFM69_DIOMAPPING2_CLKOUT_8           0x02
#define RFM69_DIOMAPPING2_CLKOUT_4           0x03
#define RFM69_DIOMAPPING2_CLKOUT_2           0x04
#define RFM69_DIOMAPPING2_CLKOUT_1           0x05  // Reset value
#define RFM69_DIOMAPPING2_CLKOUT_RC          0x06
#define RFM69_DIOMAPPING2_CLKOUT_OFF         0x07  // Recommended default

//*****************************************************************************
//
// The following are defines for the bit fields in the RFM69_O_IRQFLAGS1 register.
//
//*****************************************************************************
#define RFM69_IRQFLAGS1_MODEREADY            0x80
#define RFM69_IRQFLAGS1_RXREADY              0x40
#define RFM69_IRQFLAGS1_TXREADY              0x20
#define RFM69_IRQFLAGS1_PLLLOCK              0x10
#define RFM69_IRQFLAGS1_RSSI                 0x08
#define RFM69_IRQFLAGS1_TIMEOUT              0x04
#define RFM69_IRQFLAGS1_AUTOMODE             0x02
#define RFM69_IRQFLAGS1_SYNCADDRESSMATCH     0x01

//*****************************************************************************
//
// The following are defines for the bit fields in the RFM69_O_IRQFLAGS2 register.
//
//*****************************************************************************
#define RFM69_IRQFLAGS2_FIFOFULL             0x80
#define RFM69_IRQFLAGS2_FIFONOTEMPTY         0x40
#define RFM69_IRQFLAGS2_FIFOLEVEL            0x20
#define RFM69_IRQFLAGS2_FIFOOVERRUN          0x10
#define RFM69_IRQFLAGS2_PACKETSENT           0x08
#define RFM69_IRQFLAGS2_PAYLOADREADY         0x04
#define RFM69_IRQFLAGS2_CRCOK                0x02
#define RFM69_IRQFLAGS2_LOWBAT               0x01  // not present on RFM69/SX1231

//*****************************************************************************
//
// The following are defines for the bit fields in the RFM69_O_RSSITHRESH register.
//
//*****************************************************************************
#define RFM69_RSSITHRESH_VALUE               0xE4  // Default

//*****************************************************************************
//
// The following are defines for the bit fields in the RFM69_O_RXTIMEOUT1 register.
//
//*****************************************************************************
#define RFM69_RXTIMEOUT1_RXSTART_VALUE       0x00  // Default

//*****************************************************************************
//
// The following are defines for the bit fields in the RFM69_O_RXTIMEOUT2 register.
//
//*****************************************************************************
#define RFM69_RXTIMEOUT2_RSSITHRESH_VALUE    0x00  // Default

//*****************************************************************************
//
// The following are defines for the bit fields in the RFM69_O_PREAMBLEMSB register.
//
//*****************************************************************************
#define RFM69_PREAMBLESIZE_MSB_VALUE         0x00  // Default

//*****************************************************************************
//
// The following are defines for the bit fields in the RFM69_O_PREAMBLELSB register.
//
//*****************************************************************************
#define RFM69_PREAMBLESIZE_LSB_VALUE         0x03  // Default

//*****************************************************************************
//
// The following are defines for the bit fields in the RFM69_O_SYNCCONFIG register.
//
//*****************************************************************************
#define RFM69_SYNC_ON                0x80  // Default
#define RFM69_SYNC_OFF               0x00

#define RFM69_SYNC_FIFOFILL_AUTO     0x00  // Default -- when sync interrupt occurs
#define RFM69_SYNC_FIFOFILL_MANUAL   0x40

#define RFM69_SYNC_SIZE_1            0x00
#define RFM69_SYNC_SIZE_2            0x08
#define RFM69_SYNC_SIZE_3            0x10
#define RFM69_SYNC_SIZE_4            0x18  // Default
#define RFM69_SYNC_SIZE_5            0x20
#define RFM69_SYNC_SIZE_6            0x28
#define RFM69_SYNC_SIZE_7            0x30
#define RFM69_SYNC_SIZE_8            0x38

#define RFM69_SYNC_TOL_0             0x00  // Default
#define RFM69_SYNC_TOL_1             0x01
#define RFM69_SYNC_TOL_2             0x02
#define RFM69_SYNC_TOL_3             0x03
#define RFM69_SYNC_TOL_4             0x04
#define RFM69_SYNC_TOL_5             0x05
#define RFM69_SYNC_TOL_6             0x06
#define RFM69_SYNC_TOL_7             0x07

//*****************************************************************************
//
// The following are defines for the bit fields in the RFM69_O_SYNCVALUE1 register.
//
//*****************************************************************************
#define RFM69_SYNC_BYTE1_VALUE       0x00  // Default

//*****************************************************************************
//
// The following are defines for the bit fields in the RFM69_O_SYNCVALUE2 register.
//
//*****************************************************************************
#define RFM69_SYNC_BYTE2_VALUE       0x00  // Default

//*****************************************************************************
//
// The following are defines for the bit fields in the RFM69_O_SYNCVALUE3 register.
//
//*****************************************************************************
#define RFM69_SYNC_BYTE3_VALUE       0x00  // Default

//*****************************************************************************
//
// The following are defines for the bit fields in the RFM69_O_SYNCVALUE4 register.
//
//*****************************************************************************
#define RFM69_SYNC_BYTE4_VALUE       0x00  // Default

//*****************************************************************************
//
// The following are defines for the bit fields in the RFM69_O_SYNCVALUE5 register.
//
//*****************************************************************************
#define RFM69_SYNC_BYTE5_VALUE       0x00  // Default

//*****************************************************************************
//
// The following are defines for the bit fields in the RFM69_O_SYNCVALUE6 register.
//
//*****************************************************************************
#define RFM69_SYNC_BYTE6_VALUE       0x00  // Default

//*****************************************************************************
//
// The following are defines for the bit fields in the RFM69_O_SYNCVALUE7 register.
//
//*****************************************************************************
#define RFM69_SYNC_BYTE7_VALUE       0x00  // Default

//*****************************************************************************
//
// The following are defines for the bit fields in the RFM69_O_SYNCVALUE8 register.
//
//*****************************************************************************
#define RFM69_SYNC_BYTE8_VALUE       0x00  // Default

//*****************************************************************************
//
// The following are defines for the bit fields in the RFM69_O_PACKETCONFIG1 register.
//
//*****************************************************************************
#define RFM69_PACKET1_FORMAT_FIXED       0x00  // Default
#define RFM69_PACKET1_FORMAT_VARIABLE    0x80

#define RFM69_PACKET1_DCFREE_OFF         0x00  // Default
#define RFM69_PACKET1_DCFREE_MANCHESTER  0x20
#define RFM69_PACKET1_DCFREE_WHITENING   0x40

#define RFM69_PACKET1_CRC_ON             0x10  // Default
#define RFM69_PACKET1_CRC_OFF            0x00

#define RFM69_PACKET1_CRCAUTOCLEAR_ON    0x00  // Default
#define RFM69_PACKET1_CRCAUTOCLEAR_OFF   0x08

#define RFM69_PACKET1_ADRSFILTERING_OFF            0x00  // Default
#define RFM69_PACKET1_ADRSFILTERING_NODE           0x02
#define RFM69_PACKET1_ADRSFILTERING_NODEBROADCAST  0x04

//*****************************************************************************
//
// The following are defines for the bit fields in the RFM69_O_PAYLOADLENGTH register.
//
//*****************************************************************************
#define RFM69_PAYLOADLENGTH_VALUE          0x40  // Default

//*****************************************************************************
//
// The following are defines for the bit fields in the RFM69_O_BROADCASTADRS register.
//
//*****************************************************************************
#define RFM69_BROADCASTADDRESS_VALUE       0x00

//*****************************************************************************
//
// The following are defines for the bit fields in the RFM69_O_AUTOMODES register.
//
//*****************************************************************************
#define RFM69_AUTOMODES_ENTER_OFF                0x00  // Default
#define RFM69_AUTOMODES_ENTER_FIFONOTEMPTY       0x20
#define RFM69_AUTOMODES_ENTER_FIFOLEVEL          0x40
#define RFM69_AUTOMODES_ENTER_CRCOK              0x60
#define RFM69_AUTOMODES_ENTER_PAYLOADREADY       0x80
#define RFM69_AUTOMODES_ENTER_SYNCADRSMATCH      0xA0
#define RFM69_AUTOMODES_ENTER_PACKETSENT         0xC0
#define RFM69_AUTOMODES_ENTER_FIFOEMPTY          0xE0

#define RFM69_AUTOMODES_EXIT_OFF                 0x00  // Default
#define RFM69_AUTOMODES_EXIT_FIFOEMPTY           0x04
#define RFM69_AUTOMODES_EXIT_FIFOLEVEL           0x08
#define RFM69_AUTOMODES_EXIT_CRCOK               0x0C
#define RFM69_AUTOMODES_EXIT_PAYLOADREADY        0x10
#define RFM69_AUTOMODES_EXIT_SYNCADRSMATCH       0x14
#define RFM69_AUTOMODES_EXIT_PACKETSENT          0x18
#define RFM69_AUTOMODES_EXIT_RXTIMEOUT           0x1C

#define RFM69_AUTOMODES_INTERMEDIATE_SLEEP       0x00  // Default
#define RFM69_AUTOMODES_INTERMEDIATE_STANDBY     0x01
#define RFM69_AUTOMODES_INTERMEDIATE_RECEIVER    0x02
#define RFM69_AUTOMODES_INTERMEDIATE_TRANSMITTER 0x03

//*****************************************************************************
//
// The following are defines for the bit fields in the RFM69_O_FIFOTHRESH register.
//
//*****************************************************************************
#define RFM69_FIFOTHRESH_TXSTART_FIFOTHRESH      0x00  // Reset value
#define RFM69_FIFOTHRESH_TXSTART_FIFONOTEMPTY    0x80  // Recommended default

#define RFM69_FIFOTHRESH_VALUE                   0x0F  // Default

//*****************************************************************************
//
// The following are defines for the bit fields in the RFM69_O_PACKETCONFIG2 register.
//
//*****************************************************************************
#define RFM69_PACKET2_RXRESTARTDELAY_1BIT        0x00  // Default
#define RFM69_PACKET2_RXRESTARTDELAY_2BITS       0x10
#define RFM69_PACKET2_RXRESTARTDELAY_4BITS       0x20
#define RFM69_PACKET2_RXRESTARTDELAY_8BITS       0x30
#define RFM69_PACKET2_RXRESTARTDELAY_16BITS      0x40
#define RFM69_PACKET2_RXRESTARTDELAY_32BITS      0x50
#define RFM69_PACKET2_RXRESTARTDELAY_64BITS      0x60
#define RFM69_PACKET2_RXRESTARTDELAY_128BITS     0x70
#define RFM69_PACKET2_RXRESTARTDELAY_256BITS     0x80
#define RFM69_PACKET2_RXRESTARTDELAY_512BITS     0x90
#define RFM69_PACKET2_RXRESTARTDELAY_1024BITS    0xA0
#define RFM69_PACKET2_RXRESTARTDELAY_2048BITS    0xB0
#define RFM69_PACKET2_RXRESTARTDELAY_NONE        0xC0
#define RFM69_PACKET2_RXRESTART                  0x04

#define RFM69_PACKET2_AUTORXRESTART_ON           0x02  // Default
#define RFM69_PACKET2_AUTORXRESTART_OFF          0x00

#define RFM69_PACKET2_AES_ON                     0x01
#define RFM69_PACKET2_AES_OFF                    0x00  // Default

//*****************************************************************************
//
// The following are defines for the bit fields in the RFM69_O_AESKEY1 register.
//
//*****************************************************************************
#define RFM69_AESKEY1_VALUE            0x00  // Default

//*****************************************************************************
//
// The following are defines for the bit fields in the RFM69_O_AESKEY2 register.
//
//*****************************************************************************
#define RFM69_AESKEY2_VALUE            0x00  // Default

//*****************************************************************************
//
// The following are defines for the bit fields in the RFM69_O_AESKEY3 register.
//
//*****************************************************************************
#define RFM69_AESKEY3_VALUE            0x00  // Default

//*****************************************************************************
//
// The following are defines for the bit fields in the RFM69_O_AESKEY4 register.
//
//*****************************************************************************
#define RFM69_AESKEY4_VALUE            0x00  // Default

//*****************************************************************************
//
// The following are defines for the bit fields in the RFM69_O_AESKEY5 register.
//
//*****************************************************************************
#define RFM69_AESKEY5_VALUE            0x00  // Default


//*****************************************************************************
//
// The following are defines for the bit fields in the RFM69_O_AESKEY6 register.
//
//*****************************************************************************
#define RFM69_AESKEY6_VALUE            0x00  // Default


//*****************************************************************************
//
// The following are defines for the bit fields in the RFM69_O_AESKEY7 register.
//
//*****************************************************************************
#define RFM69_AESKEY7_VALUE            0x00  // Default

//*****************************************************************************
//
// The following are defines for the bit fields in the RFM69_O_AESKEY8 register.
//
//*****************************************************************************
#define RFM69_AESKEY8_VALUE            0x00  // Default

//*****************************************************************************
//
// The following are defines for the bit fields in the RFM69_O_AESKEY9 register.
//
//*****************************************************************************
#define RFM69_AESKEY9_VALUE            0x00  // Default

//*****************************************************************************
//
// The following are defines for the bit fields in the RFM69_O_AESKEY10 register.
//
//*****************************************************************************
#define RFM69_AESKEY10_VALUE           0x00  // Default

//*****************************************************************************
//
// The following are defines for the bit fields in the RFM69_O_AESKEY11 register.
//
//*****************************************************************************
#define RFM69_AESKEY11_VALUE           0x00  // Default

//*****************************************************************************
//
// The following are defines for the bit fields in the RFM69_O_AESKEY12 register.
//
//*****************************************************************************
#define RFM69_AESKEY12_VALUE           0x00  // Default

//*****************************************************************************
//
// The following are defines for the bit fields in the RFM69_O_AESKEY13 register.
//
//*****************************************************************************
#define RFM69_AESKEY13_VALUE           0x00  // Default

//*****************************************************************************
//
// The following are defines for the bit fields in the RFM69_O_AESKEY14 register.
//
//*****************************************************************************
#define RFM69_AESKEY14_VALUE           0x00  // Default

//*****************************************************************************
//
// The following are defines for the bit fields in the RFM69_O_AESKEY15 register.
//
//*****************************************************************************
#define RFM69_AESKEY15_VALUE           0x00  // Default

//*****************************************************************************
//
// The following are defines for the bit fields in the RFM69_O_AESKEY16 register.
//
//*****************************************************************************
#define RFM69_AESKEY16_VALUE           0x00  // Default

//*****************************************************************************
//
// The following are defines for the bit fields in the RFM69_O_TEMP1 register.
//
//*****************************************************************************
#define RFM69_TEMP1_MEAS_START         0x08
#define RFM69_TEMP1_MEAS_RUNNING       0x04

//*****************************************************************************
//
// The following are defines for the bit fields in the RFM69_O_TESTLNA register.
//
//*****************************************************************************
#define RFM69_TESTLNA_NORMAL           0x1B
#define RFM69_TESTLNA_HIGH_SENSITIVITY 0x2D

//*****************************************************************************
//
// The following are defines for the bit fields in the RFM69_O_TESTDAGC register.
//
//*****************************************************************************
#define RFM69_DAGC_NORMAL              0x00  // Reset value
#define RFM69_DAGC_IMPROVED_LOWBETA1   0x20
#define RFM69_DAGC_IMPROVED_LOWBETA0   0x30  // Recommended default
