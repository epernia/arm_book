//=====[Libraries]=============================================================

#include "mbed.h"
#include "arm_book_lib.h"

#include "motor.h"
#include "code.h"
#include "siren.h"

//=====[Declaration of private defines]======================================

//=====[Declaration of private data types]=====================================

bool motorDirection1LimitSwitchState;
bool motorDirection2LimitSwitchState;
int motorDirection;
bool motorBlockedState;

//=====[Declaration and initialization of public global objects]===============

InterruptIn motorDirection1Button(PF_9);
InterruptIn motorDirection2Button(PF_8);
InterruptIn motorDirection1LimitSwitch(PG_1);
InterruptIn motorDirection2LimitSwitch(PF_7);
DigitalInOut motorM1Pin(PF_2);
DigitalInOut motorM2Pin(PE_3);

//=====[Declaration of external public global variables]=======================

//=====[Declaration and initialization of public global variables]=============

//=====[Declaration and initialization of private global variables]============

//=====[Declarations (prototypes) of private functions]========================

void motorDirection1ButtonFall();
void motorDirection2ButtonFall();
void motorDirection1LimitSwitchFall();
void motorDirection2LimitSwitchFall();
static void motorUnblockUpdate();
static void motorUnblock();

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
    
    motorM1Pin.input();
    motorM2Pin.input();

    motorDirection = 0;
}

int motorDirectionRead()
{
    return motorDirection;
}

bool motorDirection1LimitSwitchStateRead()
{
    return motorDirection1LimitSwitchState;
}

bool motorDirection2LimitSwitchStateRead()
{
    return motorDirection2LimitSwitchState;
}

void motorCloseGate()
{
    if ( !motorDirection1LimitSwitchState ) {
        motorM2Pin.input();
        motorM1Pin.output();
        motorM1Pin = LOW;
        motorDirection = 1;
        motorDirection2LimitSwitchState = OFF;
    }
    motorBlockedState = ON;
    motorDirection1Button.fall(NULL);
    motorDirection2Button.fall(NULL);    
}

void motorBlockedStateWrite( bool state )
{
    motorBlockedState = state;
    
    if  ( !state ) {
        motorDirection1Button.fall(&motorDirection1ButtonFall);
        motorDirection2Button.fall(&motorDirection2ButtonFall);
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

//=====[Implementations of private functions]==================================

void motorDirection1ButtonFall()
{
    if ( !motorDirection1LimitSwitchState ) {
        motorM2Pin.input();
        motorM1Pin.output();
        motorM1Pin = LOW;
        motorDirection = 1;
        motorDirection2LimitSwitchState = OFF;
    }
}

void motorDirection2ButtonFall()
{
    if ( !motorDirection2LimitSwitchState ) {
        motorM1Pin.input();
        motorM2Pin.output();
        motorM2Pin = LOW;
        motorDirection = 2;
        motorDirection1LimitSwitchState = OFF;
    }
}

void motorDirection1LimitSwitchFall()
{
    if ( motorDirection == 1 ) {
        motorDirection = 0;
        motorDirection1LimitSwitchState = ON;
        motorM1Pin.input();
    }
}

void motorDirection2LimitSwitchFall()
{
    if ( motorDirection == 2 ) {
        motorDirection = 0;
        motorDirection2LimitSwitchState = ON;
        motorM2Pin.input();
    }
}

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