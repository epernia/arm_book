//=====[Libraries]=============================================================

#include "mbed.h"
#include "arm_book_lib.h"

#include "siren.h"

#include "smart_home_system.h"
#include "fire_alarm.h"

//=====[Declaration of private defines]======================================

//=====[Declaration of private data types]=====================================

//=====[Declaration and initialization of public global objects]===============

DigitalOut alarmLed(LED1);

//=====[Declaration of external public global variables]=======================

//=====[Declaration and initialization of public global variables]=============

//=====[Declaration and initialization of private global variables]============

static bool sirenState = OFF;

//=====[Declarations (prototypes) of private functions]========================

//=====[Implementations of public functions]===================================

void sirenInit()
{
    alarmLed = OFF;
}

bool sirenStateRead()
{
    return sirenState;
}

void sirenStateWrite( bool state )
{
    sirenState = state;
}

void sirenIndicatorUpdate( int blinkTime )
{
    static int accumulatedTimeAlarm = 0;
    accumulatedTimeAlarm = accumulatedTimeAlarm + SYSTEM_TIME_INCREMENT_MS;
    
    if( sirenState ) {
        if( accumulatedTimeAlarm >= blinkTime ) {
            accumulatedTimeAlarm = 0;
            alarmLed = !alarmLed;
        }
    } else {
        alarmLed = OFF;
    }
}

//=====[Implementations of private functions]==================================