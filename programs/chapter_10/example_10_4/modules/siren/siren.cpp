//=====[Libraries]=============================================================

#include "mbed.h"
#include "arm_book_lib.h"

#include "siren.h"

#include "smart_home_system.h"
#include "fire_alarm.h"
#include "intruder_alarm.h"

//=====[Declaration of private defines]======================================

#define SIREN_BLINKING_TIME_INTRUDER_ALARM          1000
#define SIREN_BLINKING_TIME_FIRE_ALARM               500
#define SIREN_BLINKING_TIME_FIRE_AND_INTRUDER_ALARM  100

//=====[Declaration of private data types]=====================================

//=====[Declaration and initialization of public global objects]===============

DigitalOut sirenLed(LED1);

//=====[Declaration of external public global variables]=======================

//=====[Declaration and initialization of public global variables]=============

//=====[Declaration and initialization of private global variables]============

static bool sirenState = OFF;

//=====[Declarations (prototypes) of private functions]========================

//=====[Implementations of public functions]===================================

void sirenInit()
{
    sirenLed = OFF;
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
            sirenLed = !sirenLed;
        }
    } else {
        sirenLed = OFF;
    }
}

int sirenBlinkTime()
{
    if ( ( gasDetectedRead() || overTemperatureDetectedRead() ) && 
           intruderDetectedRead() ) {
        return SIREN_BLINKING_TIME_FIRE_AND_INTRUDER_ALARM;

    } else if ( gasDetectedRead() || overTemperatureDetectedRead() ) {
        return SIREN_BLINKING_TIME_FIRE_ALARM;

    } else if ( intruderDetectedRead() ) {
        return SIREN_BLINKING_TIME_INTRUDER_ALARM;
        
    } else {
        return 0;
    }
}
//=====[Implementations of private functions]==================================

