//=====[Libraries]=============================================================

#include "mbed.h"
#include "arm_book_lib.h"

#include "buttons.h"

//=====[Declaration of private defines]========================================

//=====[Declaration of private data types]=====================================

//=====[Declaration and initialization of public global objects]===============

static DigitalIn modeButton(D0);
static DigitalIn howOftenButton(D1);
static DigitalIn howLongButton(D2);
static DigitalIn moistureButton(D3);

//=====[Declaration of external public global variables]=======================

//=====[Declaration and initialization of public global variables]=============

//=====[Declaration and initialization of private global variables]============

static buttonsStatus_t buttonsStatus;

//=====[Implementations of public functions]===================================

void buttonsInit()
{
    modeButton.mode(PullDown);
    howOftenButton.mode(PullDown);
    howLongButton.mode(PullDown);
    moistureButton.mode(PullDown);

	buttonsStatus.mode = OFF;	
	buttonsStatus.howOften = HOW_OFTEN_MIN;
	buttonsStatus.howLong = HOW_LONG_MIN;
	buttonsStatus.moisture = MOISTURE_MIN;
}

void buttonsUpdate()
{
    buttonsStatus.mode = modeButton;

    if ( howOftenButton ) {
		buttonsStatus.howOften = buttonsStatus.howOften + HOW_OFTEN_INCREMENT;
		if ( buttonsStatus.howOften >= HOW_OFTEN_MAX + HOW_OFTEN_INCREMENT) {
			buttonsStatus.howOften = HOW_OFTEN_MIN;
		}	
    }	

	if ( howLongButton ) {
		buttonsStatus.howLong = buttonsStatus.howLong + HOW_LONG_INCREMENT;
		if ( buttonsStatus.howLong >= HOW_LONG_MAX + HOW_LONG_INCREMENT) {
			buttonsStatus.howLong = HOW_LONG_MIN;
		}	
	}

    if ( moistureButton ) {
		buttonsStatus.moisture = buttonsStatus.moisture + MOISTURE_INCREMENT;
		if ( buttonsStatus.moisture >= MOISTURE_MAX + MOISTURE_INCREMENT) {
			buttonsStatus.moisture = MOISTURE_MIN;
		}	
    }	
}

buttonsStatus_t buttonsRead()
{
    return buttonsStatus;
}
