//=====[Libraries]=============================================================

#include "mbed.h"
#include "arm_book_lib.h"

#include "motion_sensor.h"

//=====[Declaration of private defines]======================================

//=====[Declaration of private data types]=====================================

//=====[Declaration and initialization of public global objects]===============

InterruptIn pirOutputSignal(PG_0);

//=====[Declaration of external public global variables]=======================

//=====[Declaration and initialization of public global variables]=============

//=====[Declaration and initialization of private global variables]============

static bool pirState;

//=====[Declarations (prototypes) of private functions]========================

static void motionDetected();
static void motionCeased();

//=====[Implementations of public functions]===================================

void motionSensorInit()
{
    pirOutputSignal.rise(&motionDetected);
    pirState = OFF;
}

bool motionSensorRead()
{
    return pirState;
}

//=====[Implementations of private functions]==================================

static void motionDetected()
{
    pirState = ON;
    //motorCloseGate();
    //sirenStateWrite(ON);
    pirOutputSignal.fall(&motionCeased);
}

static void motionCeased()
{
    pirState = OFF;
    pirOutputSignal.rise(&motionDetected);
}