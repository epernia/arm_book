//=====[Libraries]=============================================================

#include "mbed.h"
#include "arm_book_lib.h"

#include "relay.h"

#include "irrigation_control.h"

//=====[Declaration of private defines]========================================

//=====[Declaration of private data types]=====================================

//=====[Declaration and initialization of public global objects]===============

static DigitalOut relayControlPin(PF_2);

//=====[Declaration and initialization of private global objects]===============

//=====[Declaration of external public global variables]=======================

//=====[Declaration and initialization of public global variables]=============

//=====[Declaration and initialization of private global variables]============

//=====[Implementations of public functions]===================================

void relayInit()
{
    relayControlPin.mode(OpenDrain);
    relayControlPin.input();
}

void relayUpdate()
{
    irrigationControlStatus_t irrigationControlStatusLocalCopy;

    irrigationControlStatusLocalCopy = irrigationControlRead();

    switch( irrigationControlStatusLocalCopy.irrigationState ) {

    case INITIAL_MODE_ASSESSMENT:
        relayControlPin.input(); 
        break;

    case CONTINUOUS_MODE_IRRIGATING:
        relayControlPin.output();                                     
        relayControlPin = LOW;	  
        break;

    case PROGRAMMED_MODE_WAITING_TO_IRRIGATE:
        relayControlPin.input(); 
        break;			

    case PROGRAMMED_MODE_IRRIGATION_SKIPPED:
        relayControlPin.input(); 
        break;	

    case PROGRAMMED_MODE_IRRIGATING:
        relayControlPin.output();                                     
        relayControlPin = LOW;	  
        break;

    default:
        relayControlPin.input(); 
        break;
    }
}

void relayRead()
{
    return;
}