//=====[Libraries]=============================================================

#include "mbed.h"
#include "arm_book_lib.h"

#include "motor.h"
#include "code.h"
#include "siren.h"

//=====[Declaration of private defines]======================================

//=====[Declaration of private data types]=====================================

//=====[Declaration and initialization of public global objects]===============

DigitalInOut motorM1Pin(PF_2);
DigitalInOut motorM2Pin(PE_3);

//=====[Declaration of external public global variables]=======================

//=====[Declaration and initialization of public global variables]=============

MotorDirection_t motorDirection;

//=====[Declaration and initialization of private global variables]============

bool motorBlockedState;

//=====[Declarations (prototypes) of private functions]========================

void motorDirection1LimitSwitchFall();
void motorDirection2LimitSwitchFall();
static void motorUnblockUpdate();
static void motorUnblock();

//=====[Implementations of public functions]===================================

void motorControlInit()
{
    motorBlockedState = OFF;

    motorM1Pin.mode(OpenDrain);
    motorM2Pin.mode(OpenDrain);
    
    motorM1Pin.input();
    motorM2Pin.input();

    motorDirection = STOPPED;
}

MotorDirection_t motorDirectionRead()
{
    return motorDirection;
}
/*
void motorCloseGate()
{
    if ( !motorDirection1LimitSwitchState ) {
        motorM2Pin.input();
        motorM1Pin.output();
        motorM1Pin = LOW;
        motorDirection = DIRECTION_1;
        //motorDirection2LimitSwitchState = OFF;
    }
    motorBlockedState = ON;
    //motorDirection1Button.fall(NULL);
    //motorDirection2Button.fall(NULL);    
}
*/
void motorBlockedStateWrite( bool state )
{
    motorBlockedState = state;
    
    if  ( !state ) {
        //motorDirection1Button.fall(&motorDirection1ButtonFall);
        //motorDirection2Button.fall(&motorDirection2ButtonFall);
    }
}

bool motorBlockedStateRead()
{
    return motorBlockedState;
}

void motorUpdate()
{
    motorUnblockUpdate();
}

void motorDirectionWrite( MotorDirection_t Direction )
{
    switch ( Direction ) {
        case DIRECTION_1:
            motorM2Pin.input();
            motorM1Pin.output();
            motorM1Pin = LOW;
            motorDirection = DIRECTION_1;
        break;

        case DIRECTION_2:
            motorM1Pin.input();
            motorM2Pin.output();
            motorM2Pin = LOW;
            motorDirection = DIRECTION_2;
        break;

        default:
            motorM1Pin.input();
            motorM2Pin.input();
            motorDirection = STOPPED;
        break;
    }
}

//=====[Implementations of private functions]==================================

static void motorUnblockUpdate()
{
    if ( motorBlockedState ) {
        if ( codeMatchFrom(CODE_KEYPAD) ||
             codeMatchFrom(CODE_PC_SERIAL) ) {
            motorUnblock();
        }
    }
}

static void motorUnblock()
{
    motorBlockedStateWrite(OFF);
    sirenStateWrite(OFF);
}