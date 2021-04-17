#include "irrigation_system.h"

#include "sapi.h"
#include "buttons.h"
#include "user_interface.h"

//=====[Declaration of private defines]======================================

//=====[Declaration of private data types]=====================================

//=====[Declaration and initialization of public global objects]===============

//=====[Declaration of external public global variables]=======================

//=====[Declaration and initialization of public global variables]=============

static delay_t irrigationSystemDelay;

//=====[Declaration and initialization of private global variables]============

//=====[Declarations (prototypes) of private functions]========================

//=====[Implementations of public functions]===================================

void irrigationSystemInit()
{
    tickInit(1);
    buttonsInit();
    userInterfaceInit();
	delayInit( &irrigationSystemDelay, SYSTEM_TIME_INCREMENT_MS );
}

void irrigationSystemUpdate()
{
    if( delayRead(&irrigationSystemDelay) ) {
        buttonsUpdate();
        userInterfaceUpdate();
    }
}

//=====[Implementations of private functions]==================================
