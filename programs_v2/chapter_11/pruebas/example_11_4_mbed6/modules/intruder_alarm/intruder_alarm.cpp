//=====[Libraries]=============================================================

#include "mbed.h"
#include "arm_book_lib.h"

#include "intruder_alarm.h"
#include "motion_sensor.h"

//=====[Declaration of private defines]========================================

//=====[Declaration of private data types]=====================================

//=====[Declaration and initialization of public global objects]===============

//=====[Declaration of external public global variables]=======================

//=====[Declaration and initialization of public global variables]=============

//=====[Declaration and initialization of private global variables]============

static bool intruderDetected = OFF;
static bool intruderDetectorState = OFF;

//=====[Declarations (prototypes) of private functions]========================

//=====[Implementations of public functions]===================================

void intruderAlarmInit()
{
    motionSensorInit();
}

void intruderAlarmUpdate()
{
    intruderDetectorState = motionSensorRead();

    if ( intruderDetectorState ) {
        intruderDetected = ON;
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

void intruderAlarmDeactivate()
{
    intruderDetected = OFF;
}
//=====[Implementations of private functions]==================================
