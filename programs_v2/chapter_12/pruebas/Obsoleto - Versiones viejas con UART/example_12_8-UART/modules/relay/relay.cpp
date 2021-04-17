#include "mbed.h"
#include "arm_book_lib.h"

#include "relay.h"

#include "irrigation_control.h"

//=====[Declaration of private defines]======================================

//=====[Declaration of private data types]=====================================

//=====[Declaration and initialization of public global objects]===============

static DigitalOut relayControlPin(D4);

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

    if ( irrigationControlStatusLocalCopy.irrigationState == INITIAL_MODE_ASSESSMENT ) {
        relayControlPin = ON; // OJO: Acá debería ser High-Z!!!
    }	
	
    if ( irrigationControlStatusLocalCopy.irrigationState == CONTINUOUS_MODE_IRRIGATING ) {
        relayControlPin = OFF;
    }

    if ( irrigationControlStatusLocalCopy.irrigationState == PROGRAMMED_MODE_WAITING_TO_IRRIGATE ) {
        relayControlPin = ON; // OJO: Acá debería ser High-Z!!!
    }	
	
    if ( irrigationControlStatusLocalCopy.irrigationState == PROGRAMMED_MODE_IRRIGATION_SKIPPED ) {
        relayControlPin = ON;
    }

    if ( irrigationControlStatusLocalCopy.irrigationState == PROGRAMMED_MODE_IRRIGATING ) {
        relayControlPin = OFF; // OJO: Acá debería ser High-Z!!!
    }		
}	

void relayRead()
{
    return;
}