//*****************************************************************************
//
// gyro.h
//
// This file defines commands for the L3G4200D 3-Axis Gyroscope Module
//
// Information in this file was gathered from the parallax product page
// http://www.parallax.com/product/27911
//
//*****************************************************************************

#ifndef __GYRO_H__
#define __GYRO_H__

#include <stdbool.h>
#include <stdint.h>

#include "status.h"

//
// Gyro Addresses
//
#define GYRO_ADDRESS_SDO_LO 0x68
#define GYRO_ADDRESS_SDO_HI 0x69

//
// Gyro Delay
//
#define GYRO_STARTUP_DELAY 5
#define GYRO_CONFIG_DELAY  100

// Gyro Sensitivites
#define GYRO_250_SENS  ((float)0.00875)
#define GYRO_500_SENS  ((float)0.0175)
#define GYRO_2000_SENS ((float)0.07)

// Calibration
#define GYRO_NUM_SAMPLES     50 // Calibration Sample Count
#define GYRO_SIGMA_MULTIPLE  3  // Calibration Sigma Multiple

// Device ID
#define GYRO_ID 0xD3

// Register address map
#define GYRO_AUTO_INCREMENT 0x80
#define GYRO_WHO_AM_I       0x0F
#define GYRO_CTRL_REG1      0x20
#define GYRO_CTRL_REG2      0x21
#define GYRO_CTRL_REG3      0x22
#define GYRO_CTRL_REG4      0x23
#define GYRO_CTRL_REG5      0x24
#define GYRO_REFERENCE      0x25
#define GYRO_OUT_TEMP       0x26
#define GYRO_STATUS_REG     0x27
#define GYRO_OUT_X_L        0x28
#define GYRO_OUT_X_H        0x29
#define GYRO_OUT_Y_L        0x2A
#define GYRO_OUT_Y_H        0x2B
#define GYRO_OUT_Z_L        0x2C
#define GYRO_OUT_Z_H        0x2D
#define GYRO_FIFO_CTRL_REG  0x2E
#define GYRO_FIFO_SRC_REG   0x2F
#define GYRO_INT1_CFG       0x30
#define GYRO_INT1_SRC       0x31
#define GYRO_INT1_TSH_XH    0x32
#define GYRO_INT1_TSH_XL    0x33
#define GYRO_INT1_TSH_YH    0x34
#define GYRO_INT1_TSH_YL    0x35
#define GYRO_INT1_TSH_ZH    0x36
#define GYRO_INT1_TSH_ZL    0x37
#define GYRO_INT1_DURATION  0x38

//Control Register 1
#define GYRO_ODR100FC12P5 0x00<<4
#define GYRO_ODR100FC25   0x01<<4
#define GYRO_ODR200FC12P5 0x04<<4
#define GYRO_ODR200FC25   0x05<<4
#define GYRO_ODR200FC50   0x06<<4
#define GYRO_ODR200FC70   0x07<<4
#define GYRO_ODR400FC20   0x08<<4
#define GYRO_ODR400FC25   0x09<<4
#define GYRO_ODR400FC50   0x10<<4
#define GYRO_ODR400FC110  0x11<<4
#define GYRO_ODR800FC30   0x12<<4
#define GYRO_ODR800FC35   0x13<<4
#define GYRO_ODR800FC50   0x14<<4
#define GYRO_ODR800FC110  0x15<<4

#define GYRO_PD           0x00<<3
#define GYRO_SLEEP        0x08
#define GYRO_NORMAL       0x01<<3

#define GYRO_ZEN          0x01<<2
#define GYRO_YEN          0x01<<1
#define GYRO_XEN          0x01

//Control Register 2
#define GYRO_RESET_FILTER   0x00<<4
#define GYRO_REF_SIG_FILTER 0x01<<4
#define GYRO_NORMAL_MODE    0x02<<4
#define GYRO_AUTORESET      0x03<<4

#define GYRO_ODR100HPFC8    0x00
#define GYRO_ODR100HPFC4    0x01
#define GYRO_ODR100HPFC2    0x02
#define GYRO_ODR100HPFC1    0x03
#define GYRO_ODR100HPFC0P5  0x04
#define GYRO_ODR100HPFC0P2  0x05
#define GYRO_ODR100HPFC0P1  0x06
#define GYRO_ODR100HPFC0P05 0x07
#define GYRO_ODR100HPFC0P02 0x08
#define GYRO_ODR100HPFC0P01 0x09

#define GYRO_ODR200HPFC15   0x00
#define GYRO_ODR200HPFC8    0x01
#define GYRO_ODR200HPFC4    0x02
#define GYRO_ODR200HPFC2    0x03
#define GYRO_ODR200HPFC1    0x04
#define GYRO_ODR200HPFC0P5  0x05
#define GYRO_ODR200HPFC0P2  0x06
#define GYRO_ODR200HPFC0P01 0x07
#define GYRO_ODR200HPFC0P05 0x08
#define GYRO_ODR200HPFC0P02 0x09

#define GYRO_ODR400HPFC30   0x00
#define GYRO_ODR400HPFC15   0x01
#define GYRO_ODR400HPFC8    0x02
#define GYRO_ODR400HPFC4    0x03
#define GYRO_ODR400HPFC2    0x04
#define GYRO_ODR400HPFC1    0x05
#define GYRO_ODR400HPFC0P5  0x06
#define GYRO_ODR400HPFC0P2  0x07
#define GYRO_ODR400HPFC0P01 0x08
#define GYRO_ODR400HPFC0P05 0x09

#define GYRO_ODR800HPFC56   0x00
#define GYRO_ODR800HPFC30   0x01
#define GYRO_ODR800HPFC15   0x02
#define GYRO_ODR800HPFC8    0x03
#define GYRO_ODR800HPFC4    0x04
#define GYRO_ODR800HPFC2    0x05
#define GYRO_ODR800HPFC1    0x06
#define GYRO_ODR800HPFC0P5  0x07
#define GYRO_ODR800HPFC0P2  0x08
#define GYRO_ODR800HPFC0P01 0x09

  //Control Register 3
#define GYRO_I1_INT1        0x01<<7
#define GYRO_I1_BOOT        0x01<<6
#define GYRO_H_LACTIVE      0x01<<5
#define GYRO_PP_OD          0x01<<4
#define GYRO_I2_DRDY        0x01<<3
#define GYRO_I2_WTM         0x01<<2
#define GYRO_I2_ORUN        0x01<<1
#define GYRO_I2_EMPTY       0x01

//Control Register 4
#define GYRO_CONTINUOUS_UPDATE  0x00<<7
#define GYRO_BATCH_UPDATE       0x01<<7
#define GYRO_BIG_ENDIAN         0x00<<6
#define GYRO_LITTLE_ENDIAN      0x01<<6
#define GYRO_DPS250             0x00<<4
#define GYRO_DPS500             0x01<<4
#define GYRO_DPS2000            0x02<<4
#define GYRO_DISABLE_SELF_TEST  0x00<<2
#define GYRO_POSITIVE_SELF_TEST 0x01<<2
#define GYRO_NEUTRAL_SELF_TEST  0x02<<2
#define GYRO_NEGATIVE_SELF_TEST 0x03<<2
#define GYRO_WIRE_INT_4         0x00
#define GYRO_WIRE_INT_3         0x01

//Control register 5
#define GYRO_BOOT_NORMAL       0x00<<7
#define GYRO_BOOT_MEM          0x01<<7
#define GYRO_FIFO_DISABLE      0x00<6
#define GYRO_FIFO_ENABLE       0x01<<6
#define GYRO_HIGH_PASS_DISABLE 0x00<<5
#define GYRO_HIGH_PASS_ENABLE  0x01<<5

#define GYRO_NON_HPF_FILT_INT  0x00<<2
#define GYRO_HPF_FILT_INT      0x01<<2
#define GYRO_LPF_FILT_INT      0x02<<2
#define GYRO_HPF_LPF_INT       0x06<<2

#define GYRO_DATA_NON_HPF      0x00
#define GYRO_DATA_HPF          0x01
#define GYRO_DATA_LPF          0x02
#define GYRO_DATA_HPF_LPF      0x06

//Status register
#define GYRO_ZYXOR 0x01<<7
#define GYRO_ZOR   0x01<<6
#define GYRO_YOR   0x01<<5
#define GYRO_XOR   0x01<<4
#define GYRO_ZYXDA 0x01<<3
#define GYRO_ZDA   0x01<<2
#define GYRO_YDA   0x01<<1
#define GYRO_XDA   0x01

//FIFO control register
#define GYRO_BYPASS_MODE        0x00<<5
#define GYRO_FIFO_MODE          0x01<<5
#define GYRO_STREAM_MODE        0x02<<5
#define GYRO_STREAM_FIFO_MODE   0x03<<5
#define GYRO_BYPASS_STREAM_MODE 0x04<<5

//FIFO_SRC_REG
#define GYRO_WTM_STATUS     0x01<<7
#define GYRO_OVERRUN_STATUS 0x01<<6
#define GYRO_FIFO_EMPTY     0x01<<5

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

/*
 * @brief: Configure boot mode
 * @param[in]: GYRO_BOOT_NORMAL, GYRO_BOOT_MEM
 */
extern bool gyroBoot(uint32_t ui32boot);
/*
 * @brief: Calibrate L3G4200D
 */
extern void gyroCalibrate(uint32_t ui32SampleCount, uint32_t ui32SigmaMultiple);
/*
 * @brief: Configure HPF behavior and cutoff frequency
 *        HPF behavior.
 *          GYRO_RESET_FILTER
 *          GYRO_REF_SIG_FILTER
 *          GYRO_NORMAL_MODE
 *          GYRO_AUTORESET
 *
 *        HPF Cut off Frequency GYRO_ODRxxxHPFCy
 *         Refer Table 27 in the datasheet
 *          x : 100, 200, 400,  800
 *          y : 30, 15, 8, 4, 2, 1, 0P5, 0P2, 0P1, 0P05, 0P02, 0P01
 *
 * @param[out]: none
 */
extern bool gyroConfigHPF(uint32_t ui32bhr, uint32_t ui32hpf);
/*
 * @brief: Configure interrupts on L3G4200D
 * @param[in]: Interrupts to enable. A combination of the following is possible
 *        GYRO_I1_Int1
 *        GYRO_I1_BOOT
 *        GYRO_H_LACTIVE
 *        GYRO_PP_OD
 *        GYRO_I2_DRDY
 *        GYRO_I2_WTM
 *        GYRO_I2_ORUN
 *        GYRO_I2_EMPTY
 */
extern bool gyroConfigInterrupts(uint32_t ui32intr);
/*
 * @brief: Configure data O/P method
 * @param[in]: Data update:
 *          GYRO_CONTINUOUS_UPDATE/GYRO_BATCH_UPDATE
 *             Endian
 *          GYRO_DPSx. x = 250, 500, 2000
 */
extern bool gyroDataConfig(uint32_t ui32upd, uint32_t ui32res);
/*
 * @brief: Test connection for L3G4200D
 */
extern bool gyroDetect(void);
/*
 * @brief: Configure FIFO behaviour
 * @param[in]: GYRO_FIFO_DISABLE, GYRO_FIFO_ENABLE
 */
extern bool gyroFIFO(uint32_t ui32fifo);
/*
 * @brief: Enable HPF
 * @param[in]: GYRO_HIGH_PASS_DISABLE, GYRO_HIGH_PASS_ENABLE
 */
extern bool gyroHPF(uint32_t ui32hpf);
/*
 * @brief: Initialize L3G4200D
 */
extern StatusCode_t gyroInit(uint32_t ui32Base, uint8_t ui8GyroAddr, bool bSpeed, uint32_t ui32SenseBase, uint8_t ui8SensePin, float* fDPS);
/*
 * @brief: Int1 selection configuration
 * @param[in]: GYRO_NON_HPF_FILT_INT, GYRO_HPF_FILT_INT, GYRO_LPF_FILT_INT, GYRO_HPF_LPF_INT
 */
extern bool gyroINT1(uint32_t ui32int1);
/*
 * @brief: Handles gyro data ready interrupt
 */
extern void gyroIntHandler(void);
/*
 * @brief: Out selection configuration
 * @param[in]: GYRO_DATA_NON_HPF, GYRO_DATA_LPF, GYRO_DATA_HPF_LPF
 */
extern bool gyroOutSel(uint32_t ui32out);
/*
 * @brief: Power down L3G4200D
 */
extern bool gyroPowerDown(void);
/*
 * @brief: Configure data rate, bandwidth and enable axis measurement
 *        Acceptable values:
 *           GYRO_ODR100FC12P5
 *           GYRO_ODR100FC25
 *           GYRO_ODR200FC12P5
 *           GYRO_ODR200FC25
 *           GYRO_ODR200FC50
 *           GYRO_ODR200FC70
 *           GYRO_ODR400FC20
 *           GYRO_ODR400FC25
 *           GYRO_ODR400FC50
 *           GYRO_ODR400FC110
 *           GYRO_ODR800FC30
 *           GYRO_ODR800FC35
 *           GYRO_ODR800FC50
 *           GYRO_ODR800FC110
 */
extern bool gyroPowerOn(uint32_t ui32odr);
/*
 * @brief: Read X,Y, Z calibrated angular rates
 */
extern bool gyroReadXYZ(void);
/*
 * @brief: Retrieve pointer to raw XYZ array
 */
extern int16_t* gyroRetrieveXYZRaw(void);
/*
 * @brief: Return if new unread data is available
 */
extern bool gyroReceive(void);
/*
 * @brief: Read temperature
 * @param[in]: ptr to temperature data
 */
extern bool gyroReadTemp(int8_t *ui8Temp);
/*
 * @brief: Configure self test
 * @param[in]: Self test
 *        GYRO_DISABLE, GYRO_POSITIVE_SELF_TEST, GYRO_NEUTRAL_SELF_TEST, GYRO_NEGATIVE_SELF_TEST
 */
extern bool gyroSelfTest(uint32_t ui32test);
/*
 * @brief: Sleep L3G4200D
 */
extern bool gyroSleep(void);
/*
 * @brief: Get status
 */
extern bool gyroStatus(uint8_t* ui8Status);

//*****************************************************************************
//
// Mark the end of the C bindings section for C++ compilers.
//
//*****************************************************************************
#ifdef __cplusplus
}
#endif

#endif // __GYRO_H__
