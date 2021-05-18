//=====[Libraries]=============================================================

#include "mbed.h"
#include "arm_book_lib.h"

#include "irrigation_control.h"

#include "buttons.h"
#include "irrigation_timer.h"
#include "moisture_sensor.h"

//=====[Declaration of private defines]======================================

//=====[Declaration of private data types]=====================================

//=====[Declaration and initialization of public global objects]===============

//=====[Declaration and initialization of private global objects]===============

//=====[Declaration of external public global variables]=======================

//=====[Declaration and initialization of public global variables]=============

//=====[Declaration and initialization of private global variables]============

static irrigationControlStatus_t irrigationControlStatus;

//=====[Implementations of public functions]===================================

void irrigationControlInit()
{
    irrigationControlStatus.irrigationState = INITIAL_MODE_ASSESSMENT;
    irrigationControlStatus.waitedTimeMustBeReset = true;
    irrigationControlStatus.irrigatedTimeMustBeReset = true;
	return;
}

void irrigationControlUpdate()
{
    buttonsStatus_t buttonsStatusLocalCopy;
    irrigationTimer_t irrigationTimerLocalCopy;
    float HL69AveragedValueLocalCopy;

    buttonsStatusLocalCopy = buttonsRead();
    irrigationTimerLocalCopy = irrigationTimerRead(); 
    HL69AveragedValueLocalCopy = moistureSensorRead();

    switch( irrigationControlStatus.irrigationState ) {

    case INITIAL_MODE_ASSESSMENT:
        
        if( buttonsStatusLocalCopy.changeMode ) {
            irrigationControlStatus.irrigationState = CONTINUOUS_MODE_IRRIGATING;
        } else {
                irrigationControlStatus.irrigationState = 
                                            PROGRAMMED_MODE_WAITING_TO_IRRIGATE;
                irrigationControlStatus.waitedTimeMustBeReset = true;
            }
        
        break;

    case CONTINUOUS_MODE_IRRIGATING:
        
        if( buttonsStatusLocalCopy.changeMode ) {
            irrigationControlStatus.irrigationState = 
                                            PROGRAMMED_MODE_WAITING_TO_IRRIGATE;
            irrigationControlStatus.waitedTimeMustBeReset = true;
        } 
        break;

    case PROGRAMMED_MODE_WAITING_TO_IRRIGATE:
	
        irrigationControlStatus.waitedTimeMustBeReset = false;
        if( buttonsStatusLocalCopy.changeMode ) {
            irrigationControlStatus.irrigationState = 
                                            CONTINUOUS_MODE_IRRIGATING;
            }
            else if( irrigationTimerLocalCopy.waitedTime >= ( buttonsStatusLocalCopy.howOften * TO_HOURS) ) {
                if( ( buttonsStatusLocalCopy.howLong != 0 ) && 
                    ( (int) (100*HL69AveragedValueLocalCopy) < buttonsStatusLocalCopy.moisture ) ) {
                irrigationControlStatus.irrigationState = 
                                            PROGRAMMED_MODE_IRRIGATING;
                irrigationControlStatus.irrigatedTimeMustBeReset = true;
                }
                else if ( ( buttonsStatusLocalCopy.howLong == 0 ) ||
                        ( (int) (100*HL69AveragedValueLocalCopy) >= buttonsStatusLocalCopy.moisture ) ) { 
                irrigationControlStatus.irrigationState = 
                                            PROGRAMMED_MODE_IRRIGATION_SKIPPED;
                } 
            }
        
        break;

    case PROGRAMMED_MODE_IRRIGATION_SKIPPED:
        
        irrigationControlStatus.waitedTimeMustBeReset = true;
		irrigationControlStatus.irrigationState = 
                                            PROGRAMMED_MODE_WAITING_TO_IRRIGATE;
        break;

    case PROGRAMMED_MODE_IRRIGATING:
        
        irrigationControlStatus.irrigatedTimeMustBeReset = false;
        
        if( irrigationTimerLocalCopy.irrigatedTime >= ( buttonsStatusLocalCopy.howLong * TO_SECONDS) ) {
            irrigationControlStatus.irrigationState = 
                                            PROGRAMMED_MODE_WAITING_TO_IRRIGATE;
            irrigationControlStatus.waitedTimeMustBeReset = true;
        } 
        break;

  default:
        irrigationControlInit();
        break;
    }
    return;
}

irrigationControlStatus_t irrigationControlRead()
{
    return irrigationControlStatus;
}