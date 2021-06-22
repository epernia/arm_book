//=====[Libraries]=============================================================

#include "mbed.h"
#include "arm_book_lib.h"

#include "display.h"

#include "buttons.h"
#include "moisture_sensor.h"
#include "irrigation_control.h"
#include "irrigation_timer.h"  // Esto despues se puede sacar

//=====[Declaration of private defines]========================================

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
	irrigationControlStatus_t irrigationControlStatusLocalCopy;
	irrigationTimer_t irrigationTimerLocalCopy;

    buttonsStatusLocalCopy = buttonsRead();
	HL69AveragedValueLocalCopy = moistureSensorRead();
	irrigationControlStatusLocalCopy = irrigationControlRead();
	irrigationTimerLocalCopy = irrigationTimerRead();

    if ( irrigationControlStatusLocalCopy.irrigationState == INITIAL_MODE_ASSESSMENT ) {
        uartUsb.printf("Mode: Initializing    | HowOften: %d hours | HowLong: %d seconds | Minimum Moisture: %d%% - Current Moisture: %2.0f%% | waitedTime: %d | irrigatedTime: %d \r\n",
	                    buttonsStatusLocalCopy.howOften,buttonsStatusLocalCopy.howLong,buttonsStatusLocalCopy.moisture,100*HL69AveragedValueLocalCopy,
						irrigationTimerLocalCopy.waitedTime,irrigationTimerLocalCopy.irrigatedTime);
    }	
	
    if ( irrigationControlStatusLocalCopy.irrigationState == CONTINUOUS_MODE_IRRIGATING ) {
        uartUsb.printf("Mode: Continuous-ON   | HowOften: %d hours | HowLong: %d seconds | Minimum Moisture: %d%% - Current Moisture: %2.0f%% | waitedTime: %d | irrigatedTime: %d \r\n",
	                    buttonsStatusLocalCopy.howOften,buttonsStatusLocalCopy.howLong,buttonsStatusLocalCopy.moisture,100*HL69AveragedValueLocalCopy,
						irrigationTimerLocalCopy.waitedTime,irrigationTimerLocalCopy.irrigatedTime);
    }

    if ( irrigationControlStatusLocalCopy.irrigationState == PROGRAMMED_MODE_WAITING_TO_IRRIGATE ) {
	    uartUsb.printf("Mode: Programmed-Wait | HowOften: %d hours | HowLong: %d seconds | Minimum Moisture: %d%% - Current Moisture: %2.0f%% | waitedTime: %d | irrigatedTime: %d \r\n",
	                    buttonsStatusLocalCopy.howOften,buttonsStatusLocalCopy.howLong,buttonsStatusLocalCopy.moisture,100*HL69AveragedValueLocalCopy,
						irrigationTimerLocalCopy.waitedTime,irrigationTimerLocalCopy.irrigatedTime);
    }	
	
    if ( irrigationControlStatusLocalCopy.irrigationState == PROGRAMMED_MODE_IRRIGATION_SKIPPED ) {
	    uartUsb.printf("Mode: Programmed-Skip | HowOften: %d hours | HowLong: %d seconds | Minimum Moisture: %d%% - Current Moisture: %2.0f%% | waitedTime: %d | irrigatedTime: %d \r\n",
	                    buttonsStatusLocalCopy.howOften,buttonsStatusLocalCopy.howLong,buttonsStatusLocalCopy.moisture,100*HL69AveragedValueLocalCopy,
						irrigationTimerLocalCopy.waitedTime,irrigationTimerLocalCopy.irrigatedTime);
    }

    if ( irrigationControlStatusLocalCopy.irrigationState == PROGRAMMED_MODE_IRRIGATING ) {
	    uartUsb.printf("Mode: Programmed-ON   | HowOften: %d hours | HowLong: %d seconds | Minimum Moisture: %d%% - Current Moisture: %2.0f%% | waitedTime: %d | irrigatedTime: %d \r\n",
	                    buttonsStatusLocalCopy.howOften,buttonsStatusLocalCopy.howLong,buttonsStatusLocalCopy.moisture,100*HL69AveragedValueLocalCopy,
						irrigationTimerLocalCopy.waitedTime,irrigationTimerLocalCopy.irrigatedTime);
    }	
	
}

void displayRead()
{
    return;
}