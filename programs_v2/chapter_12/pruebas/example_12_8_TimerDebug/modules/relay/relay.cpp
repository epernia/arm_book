#include "mbed.h"
#include "arm_book_lib.h"

#include "relay.h"

#include "irrigation_control.h"

//=====[Declaration of private defines]======================================

//=====[Declaration of private data types]=====================================

//=====[Declaration and initialization of public global objects]===============

static DigitalOut relayControlPin(D0);

//=====[Declaration and initialization of private global objects]===============

//=====[Declaration of external public global variables]=======================

//=====[Declaration and initialization of public global variables]=============

//=====[Declaration and initialization of private global variables]============

//=====[Implementations of public functions]===================================

void relayInit()
{
    relayControlPin = ON; // OJO: Acá debería ser High-Z!!!

    return;
}

void relayUpdate()
{
	irrigationControlStatus_t irrigationControlStatusLocalCopy;

    irrigationControlStatusLocalCopy = irrigationControlRead();
	
	switch( irrigationControlStatusLocalCopy.irrigationState ) {

    case INITIAL_MODE_ASSESSMENT:
        relayControlPin = ON; // OJO: Acá debería ser High-Z!!!
        break;

    case CONTINUOUS_MODE_IRRIGATING:
        relayControlPin = OFF;
        break;

    case PROGRAMMED_MODE_WAITING_TO_IRRIGATE:
        relayControlPin = ON; // OJO: Acá debería ser High-Z!!!
        break;			

    case PROGRAMMED_MODE_IRRIGATION_SKIPPED:
        relayControlPin = ON; // OJO: Acá debería ser High-Z!!!
        break;	

    case PROGRAMMED_MODE_IRRIGATING:
        relayControlPin = OFF;
        break;

    default:
        relayControlPin = ON; // OJO: Acá debería ser High-Z!!!
        break;
    }
}

void relayRead()
{
    return;
}