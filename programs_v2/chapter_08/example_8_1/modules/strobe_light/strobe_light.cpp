#include "mbed.h"
#include "arm_book_lib.h"

#include "strobe_light.h"

#include "smart_home_system.h"
#include "alarm.h"

//=====[Declaration of private defines]======================================

//=====[Declaration of private data types]=====================================

//=====[Declaration and initialization of public global objects]===============

DigitalOut strobeLight(LED1);

//=====[Declaration of external public global variables]=======================

//=====[Declaration and initialization of public global variables]=============

//=====[Declaration and initialization of private global variables]============

static bool strobeLightState = OFF;

//=====[Declarations (prototypes) of private functions]========================

//=====[Implementations of public functions]===================================

void strobeLightInit()
{
    strobeLight = OFF;
}

bool strobeLightStateRead()
{
    return strobeLightState;
}

void strobeLightStateWrite( bool state )
{
    strobeLightState = state;
}

void strobeLightUpdate( int strobeTime )
{
    static int accumulatedTimeAlarm = 0;
    accumulatedTimeAlarm = accumulatedTimeAlarm + SYSTEM_TIME_INCREMENT_MS;
    
    if( strobeLightState ) {
        if( accumulatedTimeAlarm >= strobeTime ) {
            accumulatedTimeAlarm = 0;
            strobeLight= !strobeLight;
        }
    } else {
        strobeLight = OFF;
    }
}

//=====[Implementations of private functions]==================================