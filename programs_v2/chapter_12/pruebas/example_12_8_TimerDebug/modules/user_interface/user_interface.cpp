//=====[Libraries]=============================================================

#include "mbed.h"
#include "arm_book_lib.h"

#include "user_interface.h"

#include "display.h"
#include "buttons.h"
#include "moisture_sensor.h"
#include "irrigation_control.h"
#include "irrigation_timer.h" // Esto no va en el chapter.

//=====[Declaration of private defines]======================================

//=====[Declaration of private data types]=====================================

//=====[Declaration and initialization of public global objects]===============

//=====[Declaration and initialization of private global objects]===============

//=====[Declaration of external public global variables]=======================

//=====[Declaration and initialization of public global variables]=============

//=====[Declaration and initialization of private global variables]============

//=====[Implementations of public functions]===================================

void userInterfaceInit()
{
    displayInit( 20, 4 );
	
	displayClearAndHome();
	displayCharPositionWrite( 0, 0 );
    displayStringWrite( "Mode:" );
	displayCharPositionWrite( 0, 1 );
    displayStringWrite( "HowOften:  hours" );
	displayCharPositionWrite( 0, 2 );
    displayStringWrite( "HowLong:  seconds" );
	displayCharPositionWrite( 0, 3 );
    displayStringWrite( "MinMois:  %-Curr:  %" );
	return;
}

void userInterfaceUpdate()
{
	char number[3];

    buttonsStatus_t buttonsStatusLocalCopy;
	float HL69AveragedValueLocalCopy;
    irrigationControlStatus_t irrigationControlStatusLocalCopy;
    irrigationTimer_t irrigationTimerLocalCopy; // Esto no va en el chapter.

    buttonsStatusLocalCopy = buttonsRead();
	HL69AveragedValueLocalCopy = moistureSensorRead();
    irrigationControlStatusLocalCopy = irrigationControlRead();
    irrigationTimerLocalCopy = irrigationTimerRead(); // Esto no va en el chapter.
	
	displayCharPositionWrite( 9, 1 );	
	sprintf( number, "%02d", buttonsStatusLocalCopy.howOften );
	displayStringWrite( number );
	
	displayCharPositionWrite( 8, 2 );	
	sprintf( number, "%02d", buttonsStatusLocalCopy.howLong );
	displayStringWrite( number );
	
    displayCharPositionWrite( 8, 3 );	
	sprintf( number, "%02d", buttonsStatusLocalCopy.moisture );
	displayStringWrite( number );
	
	displayCharPositionWrite( 17, 3 );	
	sprintf( number, "%2.0f", 100*HL69AveragedValueLocalCopy );
	displayStringWrite( number );

	displayCharPositionWrite( 5, 0 );	
	switch( irrigationControlStatusLocalCopy.irrigationState ) {

    case INITIAL_MODE_ASSESSMENT:
        displayStringWrite( "Initializing..." ); 
        break;

    case CONTINUOUS_MODE_IRRIGATING:
        displayStringWrite( "Continuous-ON  " ); 
        break;

    case PROGRAMMED_MODE_WAITING_TO_IRRIGATE:
        displayStringWrite( "Programmed-Wait" ); 
        break;			

    case PROGRAMMED_MODE_IRRIGATION_SKIPPED:
        displayStringWrite( "Programmed-Skip" ); 
        break;	

    case PROGRAMMED_MODE_IRRIGATING:
        displayStringWrite( "Programmed-ON  " ); 
        break;

    default:
        displayStringWrite( "Non-supported  " ); 
        break;
    }

	// Esto no va en el chapter.
	
	displayCharPositionWrite( 18, 1 );	
	sprintf( number, "%02d", irrigationTimerLocalCopy.waitedTime );
	displayStringWrite( number );

	displayCharPositionWrite( 18, 2 );	
	sprintf( number, "%02d", irrigationTimerLocalCopy.irrigatedTime );
	displayStringWrite( number );	
}

void userInterfaceRead()
{
    return;
}