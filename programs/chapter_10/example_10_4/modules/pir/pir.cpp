//=====[Libraries]=============================================================

#include "mbed.h"
#include "arm_book_lib.h"

#include "pir.h"
#include "motor.h"
#include "siren.h"

//=====[Declaration of private defines]======================================

//=====[Declaration of private data types]=====================================

//=====[Declaration and initialization of public global objects]===============

InterruptIn pirOutputSignal(PG_0);

//=====[Declaration of external public global variables]=======================

//=====[Declaration and initialization of public global variables]=============

//=====[Declaration and initialization of private global variables]============

static bool pirState;

//=====[Declarations (prototypes) of private functions]========================

void motionDetected();
void motionCeased();

//=====[Implementations of public functions]===================================

void pirSensorInit()
{
    pirOutputSignal.rise(&motionDetected);
    pirState = OFF;
}

bool pirSensorRead()
{
    return pirState;
}

//=====[Implementations of private functions]==================================

void motionDetected()
{
    pirState = ON;
    motorCloseGate();
    sirenStateWrite(ON);
    pirOutputSignal.fall(&motionCeased);
}

void motionCeased()
{
    pirState = OFF;
    pirOutputSignal.rise(&motionDetected);
}