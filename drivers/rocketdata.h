/*..
 * rocketdata.h
 *
 *  Created on: Mar 8, 2017
 *      Author: clausr
 */

#ifndef DRIVERS_ROCKETDATA_H_
#define DRIVERS_ROCKETDATA_H_

typedef struct {
  struct {
      bool bLaunchArmed : 1;
      bool bAltimeterArmed : 1;
      uint8_t vPad : 6;
  } ui8.ArmedStatus;

  uint32_t ui32ElapsedTime;

  uint32_t ui32LaunchTime;
  uint32_t ui32BackDrogueTime;
  uint32_t ui32BackMainTime;
  uint32_t ui32SelfDroguePrimTime;
  uint32_t ui32SelfDrogueBackTime;
  uint32_t ui32SelfMainPrimTime;
  uint32_t ui32SelfMainBackTime;

  union {
    uint_fast8_t ui8ParachuteDeployment;
    struct {
      bool bSelfPrimaryMainFired :1;
      bool bSelfBackupMainFired :1;
      bool bSelfPrimaryDrogueFired :1;
      bool bSelfBackupDrogueFired :1;
      bool bBackupMainFired :1;
      bool bBackupDrogueFired :1;
      bool vPad : 2;
    } sParachuteDeployment;
  };

  float fAltitudeInitial;

  float fAltitudeMax_Alt;
  float fAltitude_Alt;

  tGPSData sGPSData;
  float fAltitudeMax_GPS;

  float fTemp;
  float fPres;
  float fAccel[3];
  float fGyro[3];
  float fMag[3];
  float fQuaternion[4];
  float fRoll;
  float fPitch;
  float fYaw;
} tRocketData;

typedef struct {
    uint64_t ui64Magic;
    uint32_t ui32DataSize;
    tRocketData sData;
} tRocketPacket;

#endif /* DRIVERS_ROCKETDATA_H_ */
.

.
