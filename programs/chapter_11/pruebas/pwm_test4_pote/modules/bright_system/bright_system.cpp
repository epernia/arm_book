//=====[Libraries]=============================================================

#include "bright_system.h"

#include "sapi.h"

#include "bright_control.h"

//=====[Declaration of private defines]======================================

//=====[Declaration of private data types]=====================================

//=====[Declaration and initialization of public global objects]===============

//=====[Declaration of external public global variables]=======================

//=====[Declaration and initialization of public global variables]=============

static delay_t brightSystemDelay;

//=====[Declaration and initialization of private global variables]============

//=====[Declarations (prototypes) of private functions]========================

//=====[Implementations of public functions]===================================

void brightSystemInit()
{
    tickInit(1);
    brightControlInit();
	delayInit( &brightSystemDelay, SYSTEM_TIME_INCREMENT_MS );
}

void brightSystemUpdate()
{
    if( delayRead(&brightSystemDelay) ) {
        brightControlUpdate();
    }
}

//=====[Implementations of private functions]==================================
