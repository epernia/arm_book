//=====[#include guards - begin]===============================================

#ifndef _MOTOR_H_
#define _MOTOR_H_

//=====[Libraries]=============================================================

//=====[Declaration of public defines]=======================================

//=====[Declaration of public data types]======================================

//=====[Declarations (prototypes) of public functions]=========================

void motorControlInit();
int motorDirectionRead();
void motorCloseGate();
void motorBlockedStateWrite( bool state );
bool motorDirection1LimitSwitchStateRead();
bool motorDirection2LimitSwitchStateRead();
bool motorBlockedStateRead();
void motorUpdate();

//=====[#include guards - end]=================================================

#endif // _MOTOR_H_
