// RFM12B driver definitions
// http://opensource.org/licenses/mit-license.php
// 2012-12-12 (C) felix@lowpowerlab.com
// Based on the RFM12 driver from jeelabs.com (2009-02-09 <jc@wippler.nl>)

#include <string.h>

#include "driverlib/gpio.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "driverlib/ssi.h"
#include "driverlib/sysctl.h"

#include "inc/hw_memmap.h"
#include "inc/hw_types.h"

#include "../gpio.h"
#include "rfm12b.h"

static uint8_t rxfill;           // number of data uint8_ts in rf12_buf
static int8_t rxstate;           // current transceiver state
static uint16_t rf12_crc;        // running crc value
static uint32_t seqNum;                   // encrypted send sequence number
static uint32_t cryptKey[4];              // encryption key to use
static long rf12_seq;                     // seq number of encrypted packet (or -1)

static uint32_t rfm_irq_int_flags;

// transceiver states, these determine what to do with each interrupt
enum {
  TXCRC1, TXCRC2, TXTAIL, TXDONE, TXIDLE, TXRECV, TXPRE1, TXPRE2, TXPRE3, TXSYN1, TXSYN2,
};

static uint8_t networkID;  // network group
static uint8_t nodeID;    // address of this node
uint8_t* Data;
uint8_t* DataLen;

static uint8_t rf12_buf[RF_MAX];          // recv/xmit buf, including hdr & crc uint8_ts

//
// Prototypes
//
static uint16_t
_crc16_update(uint16_t crc, uint8_t a);
static uint8_t
RFM12BByte(uint8_t out);
static uint16_t
RFM12BXFERSlow(uint16_t cmd);
static void
RFM12BXFER(uint16_t cmd);
static void
SELECT(void);
static void
DESELECT(void);
static void
RFM12BSetSpeedSlow(void);
static void
RFM12BSetSpeedFast(void);

static uint16_t
_crc16_update(uint16_t crc, uint8_t a) {
  int i;
  crc ^= a;
  for (i = 0; i < 8; ++i) {
    if (crc & 1)
      crc = (crc >> 1) ^ 0xA001;
    else
      crc = (crc >> 1);
  }
  return crc;
}

// asserts the CS pin to the card
static void
SELECT(void) {
  MAP_GPIOPinWrite(RF_GPIO_PORT_BASE, RF_SSI_FSS, 0);
}

// de-asserts the CS pin to the card
static void
DESELECT(void) {
  MAP_GPIOPinWrite(RF_GPIO_PORT_BASE, RF_SSI_FSS, RF_SSI_FSS);
}

static void
RFM12BSetSpeedSlow(void) {
  unsigned long i;

  /* Disable the SSI */
  MAP_SSIDisable(RF_SSI_BASE);

  /* Set the maximum speed as half the system clock, with a max of 2.5 MHz. */
  i = MAP_SysCtlClockGet() / 2;
  if (i > 2500000) {
    i = 2500000;
  }

  /* Configure the SSI0 port to run at 2.5MHz */
  MAP_SSIConfigSetExpClk(RF_SSI_BASE, MAP_SysCtlClockGet(), SSI_FRF_MOTO_MODE_0, SSI_MODE_MASTER, i, 8);

  /* Enable the SSI */
  MAP_SSIEnable(RF_SSI_BASE);
}

static void
RFM12BSetSpeedFast(void) {
  unsigned long i;

  /* Disable the SSI */
  MAP_SSIDisable(RF_SSI_BASE);

  /* Set the maximum speed as the system clock, with a max of 8 MHz. */
  i = MAP_SysCtlClockGet();
  if (i > 8000000) {
    i = 8000000;
  }

  /* Configure the SSI0 port to run at 8MHz */
  MAP_SSIConfigSetExpClk(RF_SSI_BASE, MAP_SysCtlClockGet(), SSI_FRF_MOTO_MODE_0, SSI_MODE_MASTER, i, 8);

  /* Enable the SSI */
  MAP_SSIEnable(RF_SSI_BASE);
}

void
RFM12BGPIOInit(void) {
  /* Enable the peripherals used to drive the SDC on SSI */
  MAP_SysCtlPeripheralEnable(RF_SSI_SYSCTL_PERIPH);
  MAP_SysCtlPeripheralEnable(RF_GPIO_SYSCTL_PERIPH);

  /*
   * Configure the appropriate pins to be SSI instead of GPIO. The FSS (CS)
   * signal is directly driven to ensure that we can hold it low through a
   * complete transaction with the SD card.
   */
  MAP_GPIOPinTypeSSI(RF_GPIO_PORT_BASE, RF_SSI_TX | RF_SSI_RX | RF_SSI_CLK);

  /*
   * Set the SSI output pins to 4MA drive strength and engage the
   * pull-up on the receive line.
   */
  gpioOutputInit(RF_GPIO_PORT_BASE, RF_SSI_RX, GPIO_STRENGTH_4MA, GPIO_PIN_TYPE_STD_WPU);
  gpioOutputInit(RF_GPIO_PORT_BASE, RF_SSI_CLK | RF_SSI_TX | RF_SSI_FSS, GPIO_STRENGTH_4MA, GPIO_PIN_TYPE_STD);

  /* Configure the SSI0 port */
  RFM12BSetSpeedFast();
  MAP_SSIEnable(RF_SSI_BASE);

  gpioInputInit(RF_GPIO_INT_PORT_BASE, RF_RFM_IRQ, GPIO_PIN_TYPE_STD_WPU);
  rfm_irq_int_flags = gpioIntInit(RF_GPIO_INT_PORT_BASE, RF_RFM_IRQ, GPIO_RISING_EDGE);

  gpioOutputInit(RF_TX_EN_BASE, GPIO_PIN_6, RF_TX_DRIVE_STRENGTH, GPIO_PIN_TYPE_STD);
  gpioOutputInit(RF_RX_EN_BASE, GPIO_PIN_7, RF_RX_DRIVE_STRENGTH, GPIO_PIN_TYPE_STD);

  MAP_GPIOPinWrite(RF_TX_EN_BASE, GPIO_PIN_6, GPIO_PIN_6);
  MAP_GPIOPinWrite(RF_RX_EN_BASE, GPIO_PIN_7, GPIO_PIN_7);
}

uint8_t
RFM12BByte(uint8_t out) {
  uint32_t ui32RcvDat;

  MAP_SSIDataPut(RF_SSI_BASE, out); /* Write the data to the tx fifo */

  MAP_SSIDataGet(RF_SSI_BASE, &ui32RcvDat); /* flush data read during the write */

  return out;
}

static uint16_t
RFM12BXFERSlow(uint16_t cmd) {
  // slow down to under 2.5 MHz
  RFM12BSetSpeedSlow();
  SELECT();
  uint16_t reply = RFM12BByte(cmd >> 8) << 8;
  reply |= RFM12BByte(cmd);
  DESELECT();
  RFM12BSetSpeedFast();
  return reply;
}

void
RFM12BXFER(uint16_t cmd) {
#if OPTIMIZE_SPI
  // writing can take place at full speed, even 8 MHz works
  SELECT();
  RFM12BByte(cmd >> 8) << 8;
  RFM12BByte(cmd);
  DESELECT();
#else
  XFERSlow(cmd);
#endif
}

// Call this once with params:
// - node ID (0-31)
// - frequency band (RF12_433MHZ, RF12_868MHZ, RF12_915MHZ)
// - networkid [optional - default = 170] (0-255 for RF12B, only 212 allowed for RF12)
// - txPower [optional - default = 0 (max)] (7 is min value)
// - AirKbps [optional - default = 38.31Kbps]
// - lowVoltageThreshold [optional - default = RF12_2v75]
void
RFM12BInitialize(uint8_t ID, uint8_t freqBand, uint8_t networkid, uint8_t txPower, uint8_t airKbps, uint8_t lowVoltageThreshold) {
  //while(millis()<60);
  nodeID = ID;
  networkID = networkid;
  RFM12BGPIOInit();
  RFM12BXFER(0x0000);  // intitial SPI transfer added to avoid power-up problem
  RFM12BXFER(RF_SLEEP_MODE);  // DC (disable clk pin), enable lbd

  // wait until RFM12B is out of power-up reset, this takes several *seconds*
  RFM12BXFER(RF_TXREG_WRITE);  // in case we're still in OOK mode
  while (!MAP_GPIOPinRead(RF_GPIO_INT_PORT_BASE, RF_RFM_IRQ))
    RFM12BXFER(0x0000);

  RFM12BXFER(0x80C7 | (freqBand << 4));  // EL (ena TX), EF (ena RX FIFO), 12.0pF
  RFM12BXFER(0xA640);  // Frequency is exactly 434/868/915MHz (whatever freqBand is)
  RFM12BXFER(0xC600 + airKbps);   //Air transmission baud rate: 0x08= ~38.31Kbps
  RFM12BXFER(0x94A2);             // VDI,FAST,134kHz,0dBm,-91dBm
  RFM12BXFER(0xC2AC);             // AL,!ml,DIG,DQD4
  if (networkID != 0) {
    RFM12BXFER(0xCA83);           // FIFO8,2-SYNC,!ff,DR
    RFM12BXFER(0xCE00 | networkID);  // SYNC=2DXX;
  } else {
    RFM12BXFER(0xCA8B);  // FIFO8,1-SYNC,!ff,DR
    RFM12BXFER(0xCE2D);  // SYNC=2D;
  }
  RFM12BXFER(0xC483);  // @PWR,NO RSTRIC,!st,!fi,OE,EN
  RFM12BXFER(0x9850 | (txPower > 7 ? 7 : txPower));  // !mp,90kHz,MAX OUT               //last uint8_t=power level: 0=highest, 7=lowest
  RFM12BXFER(0xCC77);  // OB1,OB0, LPX,!ddy,DDIT,BW0
  RFM12BXFER(0xE000);  // NOT USE
  RFM12BXFER(0xC800);  // NOT USE
  RFM12BXFER(0xC043);  // Clock output (1.66MHz), Low Voltage threshold (2.55V)

  rxstate = TXIDLE;
}

// access to the RFM12B internal registers with interrupts disabled
uint16_t
RFM12BControl(uint16_t cmd) {
  MAP_IntMasterDisable();
  uint16_t r = RFM12BXFERSlow(cmd);
  MAP_IntMasterEnable();
  return r;
}

void
RFM12BIntHandler(void) {
  MAP_GPIOIntClear(RF_GPIO_PORT_BASE, rfm_irq_int_flags);

  // a transfer of 2x 16 bits @ 2 MHz over SPI takes 2x 8 us inside this ISR
  // correction: now takes 2 + 8 µs, since sending can be done at 8 MHz
  RFM12BXFER(0x0000);

  if (rxstate == TXRECV) {
    uint8_t in = RFM12BXFERSlow(RF_RX_FIFO_READ);

    if (rxfill == 0 && networkID != 0) rf12_buf[rxfill++] = networkID;

    //Serial.print(out, HEX); Serial.print(' ');
    rf12_buf[rxfill++] = in;
    rf12_crc = _crc16_update(rf12_crc, in);

    if (rxfill >= rf12_len+ 6 || rxfill >= RF_MAX) RFM12BXFER(RF_IDLE_MODE);
  } else {
    uint8_t out;
    if (rxstate < 0) {
      uint8_t pos = 4 + rf12_len + rxstate++;
      out = rf12_buf[pos];
      rf12_crc = _crc16_update(rf12_crc, out);
    } else {
      switch (rxstate++) {
        case TXSYN1: {
          out = 0x2D;
          break;
        }
        case TXSYN2: {
          out = networkID;
          rxstate = -(3 + rf12_len);
          break;
        }
        case TXCRC1: {
          out = rf12_crc;
          break;
        }
        case TXCRC2: {
          out = rf12_crc >> 8;
          break;
        }
        case TXDONE: {
          RFM12BXFER(RF_IDLE_MODE);  // fall through
        }
        default: {
          out = 0xAA;
        }
      }
    }

    //Serial.print(out, HEX); Serial.print(' ');
    RFM12BXFER(RF_TXREG_WRITE + out);
  }
}

void
RFM12BReceiveStart(void) {
  rxfill = rf12_len= 0;
  rf12_crc = ~0;
  if (networkID != 0)
  rf12_crc = _crc16_update(~0, networkID);
  rxstate = TXRECV;
  RFM12BXFER(RF_RECEIVER_ON);
}

bool
RFM12BReceiveComplete(void) {
  if (rxstate == TXRECV && (rxfill >= rf12_len+ 6 || rxfill >= RF_MAX)) {
    rxstate = TXIDLE;
    if (rf12_len > RF12_MAXDATA)
    rf12_crc = 1;  // force bad crc if packet length is invalid
    if (RF12_DESTID == 0 || RF12_DESTID == nodeID) {  //if (!(rf12_hdr & RF12_HDR_DST) || (nodeID & NODE_ID) == 31 || (rf12_hdr & RF12_HDR_MASK) == (nodeID & NODE_ID)) {
      if (rf12_crc == 0 && crypter != 0)
      crypter(false);
      else
      rf12_seq = -1;
      return true;// it's a broadcast packet or it's addressed to this node
    }
  }
  if (rxstate == TXIDLE) RFM12BReceiveStart();
  return false;
}

bool
RFM12BCanSend(void) {
  // no need to test with interrupts disabled: state TXRECV is only reached
  // outside of ISR and we don't care if rxfill jumps from 0 to 1 here
  if (rxstate == TXRECV && rxfill == 0 && (RFM12BByte(0x00) & (RF_RSSI_BIT >> 8)) == 0) {
    RFM12BXFER(RF_IDLE_MODE);  // stop receiver
    //XXX just in case, don't know whether these RF12 reads are needed!
    // rf12_XFER(0x0000); // status register
    // rf12_XFER(RF_RX_FIFO_READ); // fifo read
    rxstate = TXIDLE;
    return true;
  }
  return false;
}

void
RFM12BSendStart(uint8_t toNodeID, const void* sendBuf, uint8_t sendLen, bool requestACK, bool sendACK) {
  rf12_len= sendLen;
  memcpy((void*) rf12_data, sendBuf, sendLen);
  RFM12BSendStart2(toNodeID, requestACK, sendACK);
  RFM12BSendWait();
}

void
RFM12BSendStart2(uint8_t toNodeID, bool requestACK, bool sendACK) {
  rf12_hdr1= toNodeID | (sendACK ? RF12_HDR_ACKCTLMASK : 0);
  rf12_hdr2 = nodeID | (requestACK ? RF12_HDR_ACKCTLMASK : 0);
  if (crypter != 0) crypter(true);
  rf12_crc = ~0;
  rf12_crc = _crc16_update(rf12_crc, networkID);
  rxstate = TXPRE1;
  RFM12BXFER(RF_XMITTER_ON);  // bytes will be fed via interrupts
}

/// Should be called immediately after reception in case sender wants ACK
void
RFM12BSendACK(const void* sendBuf, uint8_t sendLen) {
  while (!RFM12BCanSend()) {
    RFM12BReceiveComplete();
  }
  RFM12BSendStart(RF12_SOURCEID, sendBuf, sendLen, false, true);
}

void
RFM12BSend(uint8_t toNodeID, const void* sendBuf, uint8_t sendLen, bool requestACK) {
  while (!RFM12BCanSend()) {
    RFM12BReceiveComplete();
  }
  RFM12BSendStart(toNodeID, sendBuf, sendLen, requestACK, false);
}

void
RFM12BSendWait(void) {
  // wait for packet to actually finish sending
  // go into low power mode, as interrupts are going to come in very soon
  while (rxstate != TXIDLE) {
  };
}

void
RFM12BOnOff(uint8_t value) {
  RFM12BXFER(value ? RF_XMITTER_ON : RF_IDLE_MODE);
}

void
RFM12BSleep(int8_t n) {
  if (n < 0)
    RFM12BControl(RF_IDLE_MODE);
  else {
    RFM12BControl(RF_WAKEUP_TIMER | 0x0500 | n);
    RFM12BControl(RF_SLEEP_MODE);
    if (n > 0) RFM12BControl(RF_WAKEUP_MODE);
  }
  rxstate = TXIDLE;
}
void
RFM12BWakeup(void) {
  RFM12BSleep(-1);
}

bool
RFM12BLowBattery(void) {
  return (RFM12BControl(0x0000) & RF_LBD_BIT) != 0;
}

uint8_t
RFM12BGetSender(void) {
  return RF12_SOURCEID;
}

uint8_t *
RFM12BGetData(void) {
  return rf12_data;
}
uint8_t
RFM12BGetDataLen(void) {
  return *DataLen;
}
bool
RFM12BACKRequested(void) {
  return RF12_WANTS_ACK;
}

/// Should be polled immediately after sending a packet with ACK request
bool
RFM12BACKReceived(uint8_t fromNodeID) {
  if (RFM12BReceiveComplete()) return RFM12BCRCPass() &&
  RF12_DESTID == nodeID && (RF12_SOURCEID == fromNodeID || fromNodeID == 0) && (rf12_hdr1& RF12_HDR_ACKCTLMASK) &&
  !(rf12_hdr2 & RF12_HDR_ACKCTLMASK);
  return false;
}

// XXTEA by David Wheeler, adapted from http://en.wikipedia.org/wiki/XXTEA
#define DELTA 0x9E3779B9
#define MX (((z>>5^y<<2) + (y>>3^z<<4)) ^ ((sum^y) + (cryptKey[(uint8_t)((p&3)^e)] ^ z)))
void
RFM12BCryptFunction(bool sending) {
  uint32_t y, z, sum, *v = (uint32_t*) rf12_data;
  uint8_t p, e, rounds = 6;

  if (sending) {
    // pad with 1..4-uint8_t sequence number
    *(uint32_t*) (rf12_data + rf12_len) = ++seqNum;
    uint8_t pad = 3 - (rf12_len & 3);
    rf12_len += pad;
    rf12_data[rf12_len] &= 0x3F;
    rf12_data[rf12_len] |= pad << 6;
    ++rf12_len;
    // actual encoding
    int8_t n = rf12_len / 4;
    if (n > 1) {
      sum = 0;
      z = v[n-1];
      do {
        sum += DELTA;
        e = (sum >> 2) & 3;
        for (p=0; p<n-1; p++)
        y = v[p+1], z = v[p] += MX;
        y = v[0];
        z = v[n-1] += MX;
      }while (--rounds);
    }
  } else if (rf12_crc == 0) {
    // actual decoding
    int8_t n = rf12_len / 4;
    if (n > 1) {
      sum = rounds*DELTA;
      y = v[0];
      do {
        e = (sum >> 2) & 3;
        for (p=n-1; p>0; p--)
        z = v[p-1], y = v[p] -= MX;
        z = v[n-1];
        y = v[0] -= MX;
      }while ((sum -= DELTA) != 0);
    }
    // strip sequence number from the end again
    if (n > 0) {
      uint8_t pad = rf12_data[--rf12_len] >> 6;
      rf12_seq = rf12_data[rf12_len] & 0x3F;
      while (pad-- > 0)
      rf12_seq = (rf12_seq << 8) | rf12_data[--rf12_len];
    }
  }
}

void
RFM12BEncrypt(const uint8_t* key, uint8_t keyLen) {
  uint8_t i;
  // by using a pointer to CryptFunction, we only link it in when actually used
  if (key != 0) {
    for (i = 0; i < keyLen; ++i)
      ((uint8_t*) cryptKey)[i] = key[i];
    crypter = RFM12BCryptFunction;
  } else
    crypter = 0;
}

bool
RFM12BCRCPass(void) {
  return rf12_crc == 0;
}
