#include "mbed.h"
#include "arm_book_lib.h"

#include "alarm.h"
#include "siren.h"
#include "strobe_light.h"
#include "code.h"
#include "matrix_keypad.h"
#include "fire_alarm.h"
#include "intruder_alarm.h"

//=====[Declaration of private defines]======================================

#define STROBE_TIME_INTRUDER_ALARM          1000
#define STROBE_TIME_FIRE_ALARM               500
#define STROBE_TIME_FIRE_AND_INTRUDER_ALARM  100

//=====[Declaration of private data types]=====================================

//=====[Declaration and initialization of public global objects]===============

//=====[Declaration of external public global variables]=======================

//=====[Declaration and initialization of public global variables]=============

//=====[Declaration and initialization of private global variables]============

static bool alarmState;

//=====[Declarations (prototypes) of private functions]========================

static void alarmDeactivate();
static int alarmStrobeTime();

//=====[Implementations of public functions]===================================

void alarmInit()
{
    alarmState = OFF;
    sirenInit();
    strobeLightInit();	
}

void alarmUpdate()
{
    if ( alarmState ) {
        
        if ( codeMatchFrom(CODE_KEYPAD) ||
             codeMatchFrom(CODE_PC_SERIAL) ) {
            alarmDeactivate();
        }
        
        sirenUpdate( alarmStrobeTime() );
        strobeLightUpdate( alarmStrobeTime() );

    } else if ( gasDetectedRead() || 
                overTemperatureDetectedRead() || 
                intruderDetectedRead() )  {

        alarmState = ON;
        sirenStateWrite(ON);
        strobeLightStateWrite(ON);
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
    strobeLightStateWrite(OFF);
    intruderAlarmDeactivate();
    fireAlarmDeactivate();   
}

static int alarmStrobeTime()
{
    if ( ( gasDetectedRead() || overTemperatureDetectedRead() ) && 
           intruderDetectedRead() ) {
        return STROBE_TIME_FIRE_AND_INTRUDER_ALARM;

    } else if ( gasDetectedRead() || overTemperatureDetectedRead() ) {
        return STROBE_TIME_FIRE_ALARM;

    } else if ( intruderDetectedRead() ) {
        return STROBE_TIME_INTRUDER_ALARM;
        
    } else {
        return 0;
    }
}
