//=====[Libraries]=============================================================

#include "mbed.h"

#include "gas_sensor.h"

//=====[Declaration of private defines]======================================

//=====[Declaration of private data types]=====================================

//=====[Declaration and initialization of public global objects]===============

DigitalIn gasDetector(D2);

//=====[Declaration of external public global variables]=======================

//=====[Declaration and initialization of public global variables]=============

//=====[Declaration and initialization of private global variables]============

//=====[Declarations (prototypes) of private functions]========================

//=====[Implementations of public functions]===================================

void gasSensorInit()
{
    gasDetector.mode(PullDown);
}

void gasSensorUpdate()
{
    return;
}

float gasSensorRead()
{
    return (float)gasDetector;
}

//=====[Implementations of private functions]==================================

