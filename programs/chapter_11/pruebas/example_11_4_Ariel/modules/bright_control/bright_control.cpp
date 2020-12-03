//=====[Libraries]=============================================================

#include "mbed.h"
#include "arm_book_lib.h"

#include "bright_control.h"

//=====[Declaration of private defines]======================================

#define NUMBER_OF_AVERAGED_SAMPLES    5

//=====[Declaration of private data types]=====================================

//=====[Declaration and initialization of public global objects]===============

PwmOut led1(PF_7);
PwmOut led2(PF_8);
PwmOut led3(PF_9);

//=====[Declaration and initialization of private global objects]===============

static Serial uartUsb(USBTX, USBRX);

static AnalogIn setPoint(A1);
static AnalogIn LDR(A2);

//=====[Declaration of external public global variables]=======================

//=====[Declaration and initialization of public global variables]=============

//=====[Declaration and initialization of private global variables]============

static float LDRAveragedValue = 0.0;
static float LDRReadingsArray[NUMBER_OF_AVERAGED_SAMPLES];

static float setPointAveragedValue = 0.0;
static float setPointReadingsArray[NUMBER_OF_AVERAGED_SAMPLES];

static float dutyCycle = 0.5;
static float Gain = 0.01;

//=====[Implementations of public functions]===================================

void brightControlInit()
{
    led1.period(0.01f);
    led2.period(0.01f);	
	led3.period(0.01f);
	led1.write(dutyCycle); 
	led2.write(dutyCycle); 
	led3.write(dutyCycle); 	
	return;
}

void brightControlUpdate()
{
	static int setPointSampleIndex = 0;
	static int LDRSampleIndex = 0;
	
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
	
	/////////////////////////////////////////////////////////////
		
    LDRReadingsArray[LDRSampleIndex] = LDR.read();
    
	LDRAveragedValue = 0.0;
    for (i = 0; i < NUMBER_OF_AVERAGED_SAMPLES; i++) {
        LDRAveragedValue = LDRAveragedValue + LDRReadingsArray[i];
    }
    LDRAveragedValue = LDRAveragedValue / NUMBER_OF_AVERAGED_SAMPLES;

    LDRSampleIndex++;
    if ( LDRSampleIndex >= NUMBER_OF_AVERAGED_SAMPLES) {
        LDRSampleIndex = 0;
    }

	dutyCycle = dutyCycle + Gain * (setPointAveragedValue - LDRAveragedValue);
	
	led1.write(dutyCycle); 
	led2.write(dutyCycle); 
	led3.write(dutyCycle);
	
	/////////////////////////////////////////////////////////////
	
	uartUsb.printf("SP: %0.3f | LDR: %0.3f | Duty: %0.5f | Added: %0.5f\r\n",
	               setPointAveragedValue,LDRAveragedValue,dutyCycle,Gain * (setPointAveragedValue - LDRAveragedValue));
	
    return;
}