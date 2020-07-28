//=====[Libraries]=============================================================

#include "mbed.h"

#include "gas_sensor.h"

//=====[Declaration of private constants]======================================

//=====[Declaration of private data types]=====================================

//=====[Declaration of external public global objects]=========================

DigitalIn gasDetector(D2);

//=====[Declaration and intitalization of public global objects]===============

//=====[Declaration and intitalization of private global objects]==============

//=====[Declaration of external public global variables]=======================

//=====[Declaration and intitalization of public global variables]=============

//=====[Declaration and intitalization of private global variables]============

//=====[Declarations (prototypes) of private functions]========================

//=====[Implementations of public functions]===================================

void gasSensorInit( void )
{
    gasDetector.mode(PullDown);
}

void gasSensorUpdate( void )
{
    return;
}

float gasSensorRead()
{
    return (float)gasDetector;
}

//=====[Implementations of private functions]==================================

