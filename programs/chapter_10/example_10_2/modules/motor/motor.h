//=====[#include guards - begin]===============================================

#ifndef _MOTOR_H_
#define _MOTOR_H_

//=====[Libraries]=============================================================

//=====[Declaration of public defines]=======================================

//=====[Declaration of public data types]======================================

//=====[Declarations (prototypes) of public functions]=========================

void motorControlInit();
bool motorM1PinRead();
bool motorM2PinRead();
bool motorDirection1limitSwitchStateRead();
bool motorDirection2limitSwitchStateRead();

//=====[#include guards - end]=================================================

#endif // _MOTOR_H_
