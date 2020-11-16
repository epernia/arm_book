//=====[Libraries]=============================================================

#include "mbed.h"
#include "pir.h"

//=====[Declaration of private defines]======================================

//=====[Declaration of private data types]=====================================

//=====[Declaration and initialization of public global objects]===============

InterruptIn pir(PC_3);
DigitalOut pirOutput(PF_3);

//=====[Declaration of external public global variables]=======================

//=====[Declaration and initialization of public global variables]=============

//=====[Declaration and initialization of private global variables]============

//=====[Declarations (prototypes) of private functions]========================

void pirUpdate();

//=====[Implementations of public functions]===================================

void pirSensorInit()
{
    pir.rise(&pirUpdate);
}

void pirUpdate()
{
    pirOutput = !pirOutput;
}

//=====[Implementations of private functions]==================================

