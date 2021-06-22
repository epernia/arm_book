//=====[Libraries]=============================================================

#include "arm_book_lib.h"

#include "irrigation_system.h"

#include "buttons.h"
#include "display.h"
#include "moisture_sensor.h"


//=====[Declaration of private defines]========================================

//=====[Declaration of private data types]=====================================

//=====[Declaration and initialization of public global objects]===============

//=====[Declaration of external public global variables]=======================

//=====[Declaration and initialization of public global variables]=============

//=====[Declaration and initialization of private global variables]============

//=====[Declarations (prototypes) of private functions]========================

//=====[Implementations of public functions]===================================

void irrigationSystemInit()
{
    buttonsInit();
    displayInit();
	moistureSensorInit();
}

void irrigationSystemUpdate()
{
    buttonsUpdate();
    displayUpdate();
	moistureSensorUpdate();

    delay(SYSTEM_TIME_INCREMENT_MS);
}

//=====[Implementations of private functions]==================================
