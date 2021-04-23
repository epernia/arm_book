//=====[Libraries]=============================================================

#include "mbed.h"
#include "arm_book_lib.h"

#include "display.h"

#include "buttons.h"

//=====[Declaration of private defines]======================================

//=====[Declaration of private data types]=====================================

//=====[Declaration and initialization of public global objects]===============

//=====[Declaration and initialization of private global objects]===============

static Serial uartUsb(USBTX, USBRX);

//=====[Declaration of external public global variables]=======================

//=====[Declaration and initialization of public global variables]=============

//=====[Declaration and initialization of private global variables]============

//=====[Implementations of public functions]===================================

void displayInit()
{
    return;
}

void displayUpdate()
{
    buttonsStatus_t buttonsStatusLocalCopy;

    buttonsStatusLocalCopy = buttonsRead();
	
	uartUsb.printf("Mode:Programmed-Wait | HowOften:%dhours | HowLong:%dseconds | MinMois:%d%%-Curr:15%%\r\n",
	                buttonsStatusLocalCopy.howOften,buttonsStatusLocalCopy.howLong,buttonsStatusLocalCopy.moisture);
}

void displayRead()
{
    return;
}