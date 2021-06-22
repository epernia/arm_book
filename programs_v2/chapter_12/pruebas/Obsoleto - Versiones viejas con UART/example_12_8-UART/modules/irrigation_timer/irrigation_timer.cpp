//=====[Libraries]=============================================================

#include "mbed.h"
#include "arm_book_lib.h"

#include "irrigation_timer.h"

#include "irrigation_control.h"

//=====[Declaration of private defines]========================================

//=====[Declaration of private data types]=====================================

//=====[Declaration and initialization of public global objects]===============

//=====[Declaration and initialization of private global objects]===============

//=====[Declaration of external public global variables]=======================

//=====[Declaration and initialization of public global variables]=============

//=====[Declaration and initialization of private global variables]============

static irrigationTimer_t irrigationTimer;

//=====[Implementations of public functions]===================================

void irrigationTimerInit()
{
    irrigationTimer.waitedTime = 0;
    irrigationTimer.irrigatedTime = 0;
}

void irrigationTimerUpdate()
{
	irrigationControlStatus_t irrigationControlStatusLocalCopy;

    irrigationControlStatusLocalCopy = irrigationControlRead();

    if ( irrigationControlStatusLocalCopy.waitedTimeMustBeReset ) {
        irrigationTimer.waitedTime = 0;
    }

    if ( irrigationControlStatusLocalCopy.irrigatedTimeMustBeReset ) {
        irrigationTimer.irrigatedTime = 0;
    }	

    if ( irrigationControlStatusLocalCopy.irrigationState == PROGRAMMED_MODE_WAITING_TO_IRRIGATE ) {
        irrigationTimer.waitedTime++;
    }	
	
    if ( irrigationControlStatusLocalCopy.irrigationState == PROGRAMMED_MODE_IRRIGATING ) {
        irrigationTimer.irrigatedTime++;
    }
}

irrigationTimer_t irrigationTimerRead()
{
    return irrigationTimer;
}