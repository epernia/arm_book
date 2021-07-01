//=====[Libraries]=============================================================

#include "mbed.h"
#include "arm_book_lib.h"

#include "gate.h"
#include "motor.h"

//=====[Declaration of private defines]========================================

//=====[Declaration of private data types]=====================================

//=====[Declaration and initialization of public global objects]===============

InterruptIn gateOpenLimitSwitch(PG_1);
InterruptIn gateCloseLimitSwitch(PF_7);

//=====[Declaration of external public global variables]=======================

//=====[Declaration and initialization of public global variables]=============

//=====[Declaration and initialization of private global variables]============

static bool gateOpenLimitSwitchState;
static bool gateCloseLimitSwitchState;

static gateStatus_t gateStatus;

//=====[Declarations (prototypes) of private functions]========================

static void gateOpenLimitSwitchCallback();
static void gateCloseLimitSwitchCallback();

//=====[Implementations of public functions]===================================

void gateInit()
{
    gateOpenLimitSwitch.mode(PullUp);
    gateCloseLimitSwitch.mode(PullUp);

    gateOpenLimitSwitch.fall(&gateOpenLimitSwitchCallback);
    gateCloseLimitSwitch.fall(&gateCloseLimitSwitchCallback);

    gateOpenLimitSwitchState = OFF;
    gateCloseLimitSwitchState = ON;
    gateStatus = GATE_CLOSED;
}

void gateOpen()
{
    if ( !gateOpenLimitSwitchState ) {
        motorDirectionWrite( DIRECTION_1 );
        gateStatus = GATE_OPENING;
        gateCloseLimitSwitchState = OFF;
    }
}

void gateClose()
{
    if ( !gateCloseLimitSwitchState ) {
        motorDirectionWrite( DIRECTION_2 );
        gateStatus = GATE_CLOSING;
        gateOpenLimitSwitchState = OFF;
    }
}

gateStatus_t gateStatusRead()
{
    return gateStatus;
}

//=====[Implementations of private functions]==================================

static void gateOpenLimitSwitchCallback()
{
    if ( motorDirectionRead() == DIRECTION_1 ) {
        motorDirectionWrite(STOPPED);
        gateStatus = GATE_OPEN;
        gateOpenLimitSwitchState = ON;
    }
}

static void gateCloseLimitSwitchCallback()
{
    if ( motorDirectionRead() == DIRECTION_2 ) {
        motorDirectionWrite(STOPPED);
        gateStatus = GATE_CLOSED;
        gateCloseLimitSwitchState = ON;
    }
}
