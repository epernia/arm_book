//=====[#include guards - begin]===============================================

#ifndef _FIRE_ALARM_H_
#define _FIRE_ALARM_H_

//=====[Declaration of public defines]=========================================

//=====[Declaration of public data types]======================================

//=====[Declarations (prototypes) of public functions]=========================

void fireAlarmInit();
void fireAlarmUpdate();
void fireAlarmDeactivate();

bool gasDetectorStateRead();
bool gasDetectedRead();

bool overTemperatureDetectorStateRead();
bool overTemperatureDetectedRead();

//=====[#include guards - end]=================================================

#endif // _FIRE_ALARM_H_
