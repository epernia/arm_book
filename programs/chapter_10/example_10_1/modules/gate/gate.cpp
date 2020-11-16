//=====[Libraries]=============================================================

#include "mbed.h"
#include "arm_book_lib.h"

#include "motor.h"

//=====[Declaration of private defines]======================================

//=====[Declaration of private data types]=====================================

//=====[Declaration and initialization of public global objects]===============

InterruptIn gateOpenLimitSwitch(PG_1);
InterruptIn gateCloseLimitSwitch(PF_7);

//=====[Declaration of external public global variables]=======================

//=====[Declaration and initialization of public global variables]=============

//=====[Declaration and initialization of private global variables]============

static bool gateOpenLimitSwitchState;
static bool gateCloseLimitSwitchState;

//=====[Declarations (prototypes) of private functions]========================

static bool gateOpenLimitSwitchRead();
static bool gateCloseLimitSwitchRead();

static bool gateOpenLimitSwitchCallback();
static bool gateCloseLimitSwitcCallback();

//=====[Implementations of public functions]===================================

void gateInit()
{

    gateOpenLimitSwitch.mode(PullUp);
    gateCloseLimitSwitch.mode(PullUp);

    gateOpenLimitSwitch.fall(&gateOpenLimitSwitchCallback);
    gateCloseLimitSwitch.fall(&gateCloseLimitSwitchCallback);

    gateOpenLimitSwitchState = OFF;
    gateCloseLimitSwitchState = OFF;
}

//=====[Implementations of private functions]==================================

static bool gateOpenLimitSwitchCallback()
{
    if ( motorDirectionRead() == DIRECTION_1 ) {
        motorDirectionWrite(STOPPED);
        gateOpenLimitSwitchState = ON;
    }
}

static bool gateCloseLimitSwitchCallback()
{
    if ( motorDirectionRead() == DIRECTION_2 ) {
        motorDirectionWrite(STOPPED);
        gateCloseLimitSwitchState = ON;
    }
}