//=====[Libraries]=============================================================

#include "mbed.h"
#include "arm_book_lib.h"

#include "bright_control.h"

//=====[Declaration of private defines]======================================

#define NUMBER_OF_AVERAGED_SAMPLES    5

//=====[Declaration of private data types]=====================================

//=====[Declaration and initialization of public global objects]===============

PwmOut ledRed(PE_14);
PwmOut ledGreen(PA_0);
PwmOut ledBlue(PD_12);

//=====[Declaration and initialization of private global objects]===============

static Serial uartUsb(USBTX, USBRX);

static AnalogIn setPoint(A2);

//=====[Declaration of external public global variables]=======================

//=====[Declaration and initialization of public global variables]=============

//=====[Declaration and initialization of private global variables]============

static float setPointAveragedValue = 0.0;
static float setPointReadingsArray[NUMBER_OF_AVERAGED_SAMPLES];

static float dutyCycle = 0.5f;
static float Gain = 0.01;

//=====[Implementations of public functions]===================================

void brightControlInit()
{
    ledRed.period(0.01f);
    ledGreen.period(0.01f);	
	ledBlue.period(0.01f);
	ledRed.write(dutyCycle); 
	ledGreen.write(dutyCycle); 
	ledBlue.write(dutyCycle);

    uartUsb.printf("Bright Control Test\r\n");
}

void brightControlUpdate()
{
	static int setPointSampleIndex = 0;
	
    int i;

	/////////////////////////////////////////////////////////////
	
    setPointReadingsArray[setPointSampleIndex] = setPoint.read();
    
	setPointAveragedValue = 0.0;
    for (i = 0; i < NUMBER_OF_AVERAGED_SAMPLES; i++) {
        setPointAveragedValue = setPointAveragedValue + setPointReadingsArray[i];
    }
    setPointAveragedValue = setPointAveragedValue / NUMBER_OF_AVERAGED_SAMPLES;

    setPointSampleIndex++;
    if ( setPointSampleIndex >= NUMBER_OF_AVERAGED_SAMPLES) {
        setPointSampleIndex = 0;
    }
	
	dutyCycle = setPointAveragedValue;
	
//	ledRed.write(0); 
//	ledGreen.write(0); 
//	ledBlue.write(dutyCycle);

// 	ledRed.write(0); 
//	ledGreen.write(dutyCycle); 
//	ledBlue.write(0);

	ledRed.write(dutyCycle); 
	ledGreen.write(0); 
	ledBlue.write(0);
	
	/////////////////////////////////////////////////////////////
	
	uartUsb.printf("SP: %0.3f | Duty: %0.5f \r\n",
	               setPointAveragedValue,dutyCycle);
	
    return;
}