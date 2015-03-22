// RFM12B driver definitions
// http://opensource.org/licenses/mit-license.php
// 2012-12-12 (C) felix@lowpowerlab.com
// Based on the RFM12 driver from jeelabs.com (2009-02-09 <jc@wippler.nl>)

#ifndef __RFM12B_H__
#define __RFM12B_H__

#include <stdbool.h>
#include <stdint.h>

///RF12 Driver version
#define OPTIMIZE_SPI       1  // uncomment this to write to the RFM12B @ 8 Mhz

/// RF12 CTL bit mask.
//#define RF12_HDR_CTL    0x80
/// RF12 DST bit mask.
//#define RF12_HDR_DST    0x40
/// RF12 ACK bit mask.
//#define RF12_HDR_ACK    0x20
/// RF12 HDR bit mask.
//#define RF12_HDR_MASK   0x1F
/// RF12 SENDER extracted from last packet
//#define RF12_SOURCEID   rf12_hdr & RF12_HDR_MASK

/// RF12 Maximum message size in uint8_ts.
#define RF12_MAXDATA    128
/// Max transmit/receive buffer: 4 header + data + 2 crc uint8_ts
#define RF_MAX          (RF12_MAXDATA + 6)

//frequency bands
#define RF12_315MHZ     0
#define RF12_433MHZ     1
#define RF12_868MHZ     2
#define RF12_915MHZ     3

//Low batteery threshold (eg 2v25 = 2.25V)
#define RF12_2v25       0
#define RF12_2v55       3
#define RF12_2v65       4
#define RF12_2v75       5
#define RF12_3v05       8
#define RF12_3v15       9
#define RF12_3v25       10

#define RF12_HDR_IDMASK      0x7F
#define RF12_HDR_ACKCTLMASK  0x80
#define RF12_DESTID   (rf12_hdr1 & RF12_HDR_IDMASK)
#define RF12_SOURCEID (rf12_hdr2 & RF12_HDR_IDMASK)

// shorthands to simplify sending out the proper ACK when requested
#define RF12_WANTS_ACK ((rf12_hdr2 & RF12_HDR_ACKCTLMASK) && !(rf12_hdr1 & RF12_HDR_ACKCTLMASK))

// options for RF12_sleep(void)
#define RF12_SLEEP   0
#define RF12_WAKEUP -1

/// Shorthand for RF12 group uint8_t in rf12_buf.
#define rf12_grp        rf12_buf[0]
/// pointer to 1st header uint8_t in rf12_buf (CTL + DESTINATIONID)
#define rf12_hdr1        rf12_buf[1]
/// pointer to 2nd header uint8_t in rf12_buf (ACK + SOURCEID)
#define rf12_hdr2        rf12_buf[2]

/// Shorthand for RF12 length uint8_t in rf12_buf.
#define rf12_len        rf12_buf[3]
/// Shorthand for first RF12 data uint8_t in rf12_buf.
#define rf12_data       (rf12_buf + 4)

/* Peripheral definitions for DK-TM4C123G board */
// SSI port
#define RF_SSI_BASE            SSI3_BASE
#define RF_SSI_SYSCTL_PERIPH   SYSCTL_PERIPH_SSI3

// GPIO for SSI pins
#define RF_GPIO_PORT_BASE      GPIO_PORTD_BASE
#define RF_GPIO_SYSCTL_PERIPH  SYSCTL_PERIPH_GPIOD
#define RF_SSI_CLK             GPIO_PIN_0
#define RF_SSI_TX              GPIO_PIN_3
#define RF_SSI_RX              GPIO_PIN_2
#define RF_SSI_FSS             GPIO_PIN_1
#define RF_SSI_PINS            (RF_SSI_TX | RF_SSI_RX | RF_SSI_CLK | RF_SSI_FSS)

// GPIO For interrupts
#define RF_GPIO_INT_PORT_BASE      GPIO_PORTA_BASE
#define RF_GPIO_INT_SYSCTL_PERIPH  SYSCTL_PERIPH_GPIOA
#define RF_RFM_IRQ                 GPIO_PIN_7

// RF12 command codes
#define RF_RECEIVER_ON  0x82DD
#define RF_XMITTER_ON   0x823D
#define RF_IDLE_MODE    0x820D
#define RF_SLEEP_MODE   0x8205
#define RF_WAKEUP_MODE  0x8207
#define RF_TXREG_WRITE  0xB800
#define RF_RX_FIFO_READ 0xB000
#define RF_WAKEUP_TIMER 0xE000

//RF12 status bits
#define RF_LBD_BIT      0x0400
#define RF_RSSI_BIT     0x0100

extern void SPIInit(void);

extern void RFM12BIntHandler(void);

extern void RFM12BInitialize(uint8_t nodeid, uint8_t freqBand, uint8_t groupid, uint8_t txPower, uint8_t airKbps, uint8_t lowVoltageThreshold);
extern void RFM12BReceiveStart(void);
extern bool RFM12BReceiveComplete(void);
extern bool RFM12BCanSend(void);
extern uint16_t RFM12BControl(uint16_t cmd);

extern void RFM12BSendStart(uint8_t toNodeId, const void* sendBuf, uint8_t sendLen, bool requestACK, bool sendACK);
extern void RFM12BSendStart2(uint8_t toNodeID, bool requestACK, bool sendACK);
extern void RFM12BSendACK(const void* sendBuf, uint8_t sendLen);
extern void RFM12BSend(uint8_t toNodeId, const void* sendBuf, uint8_t sendLen, bool requestACK);
extern void RFM12BSendWait(void);

extern void RFM12BOnOff(uint8_t value);
extern void RFM12BSleep(int8_t n);
extern void RFM12BSleep(int8_t n);
extern void RFM12BWakeup(void);

extern  uint8_t* RFM12BGetData(void);
extern uint8_t RFM12BGetDataLen(void);  //how many uint8_ts were received
extern uint8_t RFM12BGetSender(void);
extern bool RFM12BLowBattery(void);
extern bool RFM12BACKRequested(void);
extern bool RFM12BACKReceived(uint8_t fromNodeID);
static void RFM12BCryptFunction(bool sending);
extern void RFM12BEncrypt(const uint8_t* key, uint8_t keyLen);
extern bool RFM12BCRCPass(void);

void (*crypter)(bool);                    // does en-/decryption (null if disabled)

#endif
