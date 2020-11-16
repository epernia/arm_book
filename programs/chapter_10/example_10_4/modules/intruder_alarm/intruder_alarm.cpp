//=====[Libraries]=============================================================

#include "mbed.h"
#include "arm_book_lib.h"

#include "intruder_alarm.h"
#include "motion_sensor.h"

//=====[Declaration of private defines]======================================

//=====[Declaration of private data types]=====================================

//=====[Declaration and initialization of public global objects]===============

//=====[Declaration of external public global variables]=======================

//=====[Declaration and initialization of public global variables]=============

//=====[Declaration and initialization of private global variables]============

static bool intruderAlarmState           = ON;
static bool intruderDetected             = OFF;
static bool intruderDetectorState        = OFF;

//=====[Declarations (prototypes) of private functions]========================

//=====[Implementations of public functions]===================================

void intruderAlarmInit()
{
    motionSensorInit();
}

void intruderAlarmUpdate()
{
    if ( motionSensorRead() ) {
        intruderDetected = ON;
    }

    intruderDetectorState = motionSensorRead();
    
    if ( intruderDetected && intruderAlarmState )  {
        intruderDetectorState = ON;
    }
}

bool intruderDetectorStateRead()
{
    return intruderDetectorState;
}

bool intruderDetectedRead()
{
    return intruderDetected;
}

bool intruderAlarmStateRead()
{
    return intruderAlarmState;
}

void intruderAlarmDeactivate()
{
    intruderDetected = OFF;
}
//=====[Implementations of private functions]==================================