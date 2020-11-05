//=====[Libraries]=============================================================

#include "mbed.h"
#include "arm_book_lib.h"

#include "pir.h"

//=====[Declaration of private defines]======================================

//=====[Declaration of private data types]=====================================

//=====[Declaration and initialization of public global objects]===============

InterruptIn pir(PG_0);

//=====[Declaration of external public global variables]=======================

//=====[Declaration and initialization of public global variables]=============

//=====[Declaration and initialization of private global variables]============

static bool pirState;

//=====[Declarations (prototypes) of private functions]========================

void pirRise();
void pirFall();

//=====[Implementations of public functions]===================================

void pirSensorInit()
{
    pir.rise(&pirRise);
    pirState = OFF;
}

bool pirSensorRead()
{
    return pirState;
}

//=====[Implementations of private functions]==================================

void pirRise()
{
    pir.fall(&pirFall);
    pirState = ON;
}

void pirFall()
{
    pir.rise(&pirRise);
    pirState = OFF;
}
