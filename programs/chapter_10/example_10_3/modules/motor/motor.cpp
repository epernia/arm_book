//=====[Libraries]=============================================================

#include "mbed.h"
#include "arm_book_lib.h"

#include "motor.h"

//=====[Declaration of private defines]======================================

//=====[Declaration of private data types]=====================================

bool motorDirection1LimitSwitchState;
bool motorDirection2LimitSwitchState;
bool motorBlockedState;

//=====[Declaration and initialization of public global objects]===============

InterruptIn motorDirection1Button(PF_9);
InterruptIn motorDirection2Button(PF_8);
InterruptIn motorDirection1LimitSwitch(PG_1);
InterruptIn motorDirection2LimitSwitch(PF_7);
DigitalOut motorM1Pin(PF_2);
DigitalOut motorM2Pin(PE_3);

//=====[Declaration of external public global variables]=======================

//=====[Declaration and initialization of public global variables]=============

//=====[Declaration and initialization of private global variables]============

//=====[Declarations (prototypes) of private functions]========================

void motorDirection1ButtonFall();
void motorDirection2ButtonFall();
void motorDirection1LimitSwitchFall();
void motorDirection2LimitSwitchFall();

//=====[Implementations of public functions]===================================

void motorControlInit()
{
    motorDirection1Button.mode(PullUp);
    motorDirection2Button.mode(PullUp);
    motorDirection1LimitSwitch.mode(PullUp);
    motorDirection2LimitSwitch.mode(PullUp);

    motorDirection1Button.fall(&motorDirection1ButtonFall);
    motorDirection2Button.fall(&motorDirection2ButtonFall);
    motorDirection1LimitSwitch.fall(&motorDirection1LimitSwitchFall);
    motorDirection2LimitSwitch.fall(&motorDirection2LimitSwitchFall);

    motorDirection1LimitSwitchState = OFF;
    motorDirection2LimitSwitchState = OFF;
    motorBlockedState = OFF;

    motorM1Pin.mode(OpenDrain);
    motorM2Pin.mode(OpenDrain);
    
    motorM1Pin = HIGH;
    motorM2Pin = HIGH;
}

bool motorM1PinRead()
{
    return motorM1Pin.read();
}

bool motorM2PinRead()
{
    return motorM2Pin.read();
}

bool motorDirection1LimitSwitchStateRead()
{
    return motorDirection1LimitSwitchState;
}

bool motorDirection2LimitSwitchStateRead()
{
    return motorDirection2LimitSwitchState;
}

void motorDirection1Move()
{
    if ( !motorDirection1LimitSwitchState ) {
        motorM2Pin = HIGH;
        motorM1Pin = LOW;
    }
    motorBlockedState = ON;
    motorDirection1Button.fall(NULL);
    motorDirection2Button.fall(NULL);    
}

void gateLockStateWrite( bool state )
{
    motorBlockedState = state;
    
    if  ( !state ) {
        motorDirection1Button.fall(&motorDirection1ButtonFall);
        motorDirection2Button.fall(&motorDirection2ButtonFall);
    }
}

//=====[Implementations of private functions]==================================

void motorDirection1ButtonFall()
{
    if ( !motorDirection1LimitSwitchState ) {
        motorM2Pin = HIGH;
        motorM1Pin = LOW;
        motorDirection2LimitSwitchState = OFF;
    }
}

void motorDirection2ButtonFall()
{
    if ( !motorDirection2LimitSwitchState ) {
        motorM1Pin = HIGH;
        motorM2Pin = LOW;
        motorDirection1LimitSwitchState = OFF;
    }
}

void motorDirection1LimitSwitchFall()
{
    motorDirection1LimitSwitchState = ON;
    motorM1Pin = HIGH;
}

void motorDirection2LimitSwitchFall()
{
    motorDirection2LimitSwitchState = ON;
    motorM2Pin = HIGH;
}