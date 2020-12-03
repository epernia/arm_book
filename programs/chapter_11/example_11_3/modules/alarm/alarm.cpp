//=====[Libraries]=============================================================

#include "mbed.h"
#include "arm_book_lib.h"

#include "alarm.h"
#include "siren.h"
#include "code.h"
#include "matrix_keypad.h"
#include "fire_alarm.h"
#include "intruder_alarm.h"

//=====[Declaration of private defines]======================================

//=====[Declaration of private data types]=====================================

//=====[Declaration and initialization of public global objects]===============

//=====[Declaration of external public global variables]=======================

//=====[Declaration and initialization of public global variables]=============

//=====[Declaration and initialization of private global variables]============

static bool alarmState;

//=====[Declarations (prototypes) of private functions]========================

static void alarmDeactivate();

//=====[Implementations of public functions]===================================

void alarmInit()
{
    alarmState = OFF;
    sirenInit();
}

void alarmUpdate()
{
    if ( alarmState ) {
        
        if ( codeMatchFrom(CODE_KEYPAD) ||
             codeMatchFrom(CODE_PC_SERIAL) ) {
            alarmDeactivate();
        }
        
        sirenIndicatorUpdate( sirenBlinkTime() );

    } else if ( gasDetectedRead() || 
                overTemperatureDetectedRead() || 
                intruderDetectedRead() )  {

        alarmState = ON;
        sirenStateWrite(ON);
    }
}

bool alarmStateRead()
{
    return alarmState;
}

//=====[Implementations of private functions]==================================

static void alarmDeactivate()
{
    alarmState = OFF;
    sirenStateWrite(OFF);
    intruderAlarmDeactivate();
    fireAlarmDeactivate();   
}

