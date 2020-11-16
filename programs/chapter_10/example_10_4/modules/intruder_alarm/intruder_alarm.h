//=====[#include guards - begin]===============================================

#ifndef _INTRUDER_ALARM_H_
#define _INTRUDER_ALARM_H_

//=====[Libraries]=============================================================

//=====[Declaration of public defines]=======================================

//=====[Declaration of public data types]======================================

//=====[Declarations (prototypes) of public functions]=========================

void intruderAlarmInit();
void intruderAlarmUpdate();
bool intruderAlarmStateRead();

bool intruderDetectedRead();
bool intruderDetectorStateRead();

void intruderAlarmDeactivate();

//=====[#include guards - end]=================================================

#endif // _INTRUDER_ALARM_H_