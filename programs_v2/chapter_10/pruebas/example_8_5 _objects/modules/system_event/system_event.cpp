//=====[Libraries]=============================================================

#include "mbed.h"
#include "arm_book_lib.h"

#include "system_event.h"

//=====[Declaration of private defines]======================================

//=====[Declaration of private data types]=====================================

//=====[Declaration and initialization of public global objects]===============

//=====[Declaration of external public global variables]=======================

//=====[Declaration and initialization of public global variables]=============

//=====[Declaration and initialization of private global variables]============

//=====[Declarations (prototypes) of private functions]========================

//=====[Implementations of public functions]===================================

systemEvent::systemEvent(const char* eventLabel)
{
    strcpy( label, eventLabel );
    lastState = OFF;
}

void systemEvent::lastStateUpdate(bool state)
{
    lastState = state;
}

bool systemEvent::lastStateRead( )
{
    return lastState;
}

char* systemEvent::getLabel( )
{
    return label;
}

//=====[Implementations of private functions]==================================