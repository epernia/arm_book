//=====[#include guards - begin]===============================================

#ifndef _MOTOR_H_
#define _MOTOR_H_

//=====[Libraries]=============================================================

//=====[Declaration of public defines]=======================================

//=====[Declaration of public data types]======================================

//=====[Declarations (prototypes) of public functions]=========================

void motorControlInit();
int motorDirectionRead();
bool motorDirection1LimitSwitchStateRead();
bool motorDirection2LimitSwitchStateRead();

//=====[#include guards - end]=================================================

#endif // _MOTOR_H_
