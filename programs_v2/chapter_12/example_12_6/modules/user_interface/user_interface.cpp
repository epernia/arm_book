//=====[Libraries]=============================================================

#include "mbed.h"
#include "arm_book_lib.h"

#include "user_interface.h"

#include "display.h"
#include "buttons.h"
#include "moisture_sensor.h"

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
    displayStringWrite( "Mode:Programmed-Wait" );
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

    buttonsStatusLocalCopy = buttonsRead();
	HL69AveragedValueLocalCopy = moistureSensorRead();
	
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
}

void userInterfaceRead()
{
    return;
}