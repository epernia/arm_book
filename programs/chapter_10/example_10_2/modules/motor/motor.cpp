//=====[Libraries]=============================================================

#include "mbed.h"
#include "arm_book_lib.h"

#include "motor.h"

//=====[Declaration of private defines]======================================

//=====[Declaration of private data types]=====================================

bool motorDirection1limitSwitchState;
bool motorDirection2limitSwitchState;

//=====[Declaration and initialization of public global objects]===============

InterruptIn motorDirection1Button(PF_9);
InterruptIn motorDirection2Button(PF_8);
InterruptIn motorDirection1limitSwitch(PG_1);
InterruptIn motorDirection2limitSwitch(PF_7);
DigitalOut  motorM1Pin(PF_2);
DigitalOut  motorM2Pin(PE_3);

//=====[Declaration of external public global variables]=======================

//=====[Declaration and initialization of public global variables]=============

//=====[Declaration and initialization of private global variables]============

//=====[Declarations (prototypes) of private functions]========================

void motorDirection1ButtonFall();
void motorDirection2ButtonFall();
void motorDirection1limitSwitchFall();
void motorDirection2limitSwitchFall();

//=====[Implementations of public functions]===================================

void motorControlInit()
{
    motorDirection1Button.mode(PullUp);
    motorDirection2Button.mode(PullUp);
    motorDirection1limitSwitch.mode(PullUp);
    motorDirection2limitSwitch.mode(PullUp);

    motorDirection1Button.fall(&motorDirection1ButtonFall);
    motorDirection2Button.fall(&motorDirection2ButtonFall);
    motorDirection1limitSwitch.fall(&motorDirection1limitSwitchFall);
    motorDirection2limitSwitch.fall(&motorDirection2limitSwitchFall);

    motorDirection1limitSwitchState = OFF;
    motorDirection2limitSwitchState = OFF;

    motorM1Pin = HIGH;
    motorM2Pin = HIGH;
}

//=====[Implementations of private functions]==================================

void motorDirection1ButtonFall()
{
    if ( !motorDirection1limitSwitchState ) {
        motorM2Pin = HIGH;
        motorM1Pin = LOW;
        motorDirection2limitSwitchState = OFF;
    }
}

void motorDirection2ButtonFall()
{
    if ( !motorDirection2limitSwitchState ) {
        motorM1Pin = HIGH;
        motorM2Pin = LOW;
        motorDirection1limitSwitchState = OFF;
    }
}

void motorDirection1limitSwitchFall()
{
    motorDirection1limitSwitchState = ON;
    motorDirection2limitSwitchState = OFF;
    motorM1Pin = HIGH;
}

void motorDirection2limitSwitchFall()
{
    motorDirection1limitSwitchState = OFF;
    motorDirection2limitSwitchState = ON;
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

bool motorDirection1limitSwitchStateRead()
{
    return motorDirection1limitSwitchState;
}

bool motorDirection2limitSwitchStateRead()
{
    return motorDirection2limitSwitchState;
}
