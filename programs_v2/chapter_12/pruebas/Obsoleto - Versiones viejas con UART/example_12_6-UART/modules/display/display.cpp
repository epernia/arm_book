#include "mbed.h"
#include "arm_book_lib.h"

#include "display.h"

#include "buttons.h"
#include "moisture_sensor.h"

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
	float HL69AveragedValueLocalCopy;

    buttonsStatusLocalCopy = buttonsRead();
	HL69AveragedValueLocalCopy = moistureSensorRead();
	
	uartUsb.printf("Mode: Programmed-Wait | HowOften: %d hours | HowLong: %d seconds | Minimum Moisture: %d%% - Current Moisture: %2.0f%%\r\n",
	                buttonsStatusLocalCopy.howOften,buttonsStatusLocalCopy.howLong,buttonsStatusLocalCopy.moisture,100*HL69AveragedValueLocalCopy);
}

void displayRead()
{
    return;
}