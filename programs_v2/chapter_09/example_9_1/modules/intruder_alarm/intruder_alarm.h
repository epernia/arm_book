//=====[#include guards - begin]===============================================

#ifndef _INTRUDER_ALARM_H_
#define _INTRUDER_ALARM_H_

//=====[Declaration of public defines]=========================================

//=====[Declaration of public data types]======================================

//=====[Declarations (prototypes) of public functions]=========================

void intruderAlarmInit();
void intruderAlarmUpdate();
void intruderAlarmDeactivate();

bool intruderDetectorStateRead();
bool intruderDetectedRead();

//=====[#include guards - end]=================================================

#endif // _INTRUDER_ALARM_H_