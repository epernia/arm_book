//=====[#include guards - begin]===============================================

#ifndef _MOTOR_H_
#define _MOTOR_H_

//=====[Libraries]=============================================================

//=====[Declaration of public defines]=======================================

//=====[Declaration of public data types]======================================

typedef enum {
    DIRECTION_1,
    DIRECTION_2,
    STOPPED
} MotorDirection_t;

//=====[Declarations (prototypes) of public functions]=========================

void motorControlInit();
void motorDirectionWrite( MotorDirection_t Direction );

MotorDirection_t motorDirectionRead();

void motorUpdate();

//=====[#include guards - end]=================================================

#endif // _MOTOR_H_
