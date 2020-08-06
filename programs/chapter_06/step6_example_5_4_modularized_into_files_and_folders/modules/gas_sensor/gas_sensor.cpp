//=====[Libraries]=============================================================

#include "mbed.h"

#include "gas_sensor.h"

//=====[Declaration of private constants]======================================

//=====[Declaration of private data types]=====================================

//=====[Declaration and intitalization of public global objects]===============

DigitalIn gasDetector(D2);

//=====[Declaration of external public global variables]=======================

//=====[Declaration and intitalization of public global variables]=============

//=====[Declaration and intitalization of private global variables]============

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

