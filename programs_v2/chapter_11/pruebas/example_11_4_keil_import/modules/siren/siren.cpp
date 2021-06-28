//=====[Libraries]=============================================================

#include "mbed.h"
#include "arm_book_lib.h"

#include "siren.h"

#include "smart_home_system.h"

//=====[Declaration of private defines]========================================

//=====[Declaration of private data types]=====================================

//=====[Declaration and initialization of public global objects]===============

PwmOut sirenPin(PC_9_ALT0);

//=====[Declaration of external public global variables]=======================

//=====[Declaration and initialization of public global variables]=============

//=====[Declaration and initialization of private global variables]============

static bool sirenState = OFF;
static int currentStrobeTime = 0;

//=====[Declarations (prototypes) of private functions]========================

//=====[Implementations of public functions]===================================

void sirenInit()
{
    sirenPin.period(1.0f);
    sirenPin.write(1.0f);
}

bool sirenStateRead()
{
    return sirenState;
}

void sirenStateWrite( bool state )
{
    sirenState = state;
}

void sirenUpdate( int strobeTime )
{
    if( sirenState ) {
        if (currentStrobeTime != strobeTime) {
            sirenPin.period( (float) strobeTime * 2 / 1000 );
            sirenPin.write(0.5f);
            currentStrobeTime = strobeTime;
        }
    } else {
        sirenPin.write(1.0f);
        currentStrobeTime = 0;
    }
}

//=====[Implementations of private functions]==================================