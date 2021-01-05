//=====[Libraries]=============================================================

#include "mbed.h"
#include "arm_book_lib.h"

#include "moisture_sensor.h"

//=====[Declaration of private defines]======================================

#define NUMBER_OF_AVERAGED_SAMPLES    10

//=====[Declaration of private data types]=====================================

//=====[Declaration and initialization of public global objects]===============

//=====[Declaration and initialization of private global objects]===============

static AnalogIn HL69(A0);
	
//=====[Declaration of external public global variables]=======================

//=====[Declaration and initialization of public global variables]=============

//=====[Declaration and initialization of private global variables]============

static float HL69AveragedValue = 0.0;
static float HL69ReadingsArray[NUMBER_OF_AVERAGED_SAMPLES];

//=====[Implementations of public functions]===================================

void moistureSensorInit()
{
    return;
}

void moistureSensorUpdate()
{
	static int HL69SampleIndex = 0;
    int i;

    HL69ReadingsArray[HL69SampleIndex] = 1 - HL69.read();
    
	HL69AveragedValue = 0.0;
    for (i = 0; i < NUMBER_OF_AVERAGED_SAMPLES; i++) {
        HL69AveragedValue = HL69AveragedValue + HL69ReadingsArray[i];
    }
    HL69AveragedValue = HL69AveragedValue / NUMBER_OF_AVERAGED_SAMPLES;

    HL69SampleIndex++;
    if ( HL69SampleIndex >= NUMBER_OF_AVERAGED_SAMPLES) {
        HL69SampleIndex = 0;
    }
}

float moistureSensorRead()
{
    return HL69AveragedValue;
}