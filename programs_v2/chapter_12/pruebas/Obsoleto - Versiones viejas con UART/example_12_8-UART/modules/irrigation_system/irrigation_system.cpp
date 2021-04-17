#include "arm_book_lib.h"

#include "irrigation_system.h"

#include "buttons.h"
#include "display.h"
#include "moisture_sensor.h"
#include "relay.h"
#include "irrigation_control.h"
#include "irrigation_timer.h"

//=====[Declaration of private defines]======================================

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
    moistureSensorInit();
	relayInit();
    irrigationControlInit();
    irrigationTimerInit();
    displayInit();
}

void irrigationSystemUpdate()
{
    buttonsUpdate();
    moistureSensorUpdate();
    relayUpdate();
    irrigationControlUpdate();
    irrigationTimerUpdate();
    displayUpdate();

    delay(SYSTEM_TIME_INCREMENT_MS);
}

//=====[Implementations of private functions]==================================
