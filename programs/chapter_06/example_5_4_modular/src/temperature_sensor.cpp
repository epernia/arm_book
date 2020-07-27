//=====[Libraries]=============================================================

#include "temperature_sensor.h"

//=====[Declaration of private data types]=====================================



//=====[Declaration of private constants]======================================

#define LM35_SAMPLE_TIME                       100
#define NUMBER_OF_AVG_SAMPLES                   10

//=====[Declaration of external public global objects]=========================



//=====[Declaration and intitalization of public global objects]===============



//=====[Declaration and intitalization of private global objects]==============

AnalogIn lm35(A1);

//=====[Declaration of external public global variables]=======================

float lm35AvgReadingsArray[NUMBER_OF_AVG_SAMPLES];
float lm35ReadingsMovingAverage = 0.0;
float lm35TempC                 = 0.0;

int accumulatedTimeLm35    = 0;
int lm35SampleIndex        = 0;

//=====[Declaration and intitalization of public global variables]=============



//=====[Declaration and intitalization of private global variables]============



//=====[Declarations (prototypes) of private functions]========================



//=====[Implementations of public functions]===================================


float analogReadingScaledWithTheLM35Formula( float analogReading )
{
    return ( analogReading * 3.3 / 0.01 );
}

float celsiusToFahrenheit( float tempInCelsiusDegrees )
{
    return ( tempInCelsiusDegrees * 9.0 / 5.0 + 32.0 );
}

void shiftLm35AvgReadingsArray()
{
    int i = 0;
    for( i=1; i<NUMBER_OF_AVG_SAMPLES; i++ ) {
        lm35AvgReadingsArray[i-1] = lm35AvgReadingsArray[i];
    }
    lm35AvgReadingsArray[NUMBER_OF_AVG_SAMPLES-1] = 0.0;
}

//=====[Implementations of private functions]==================================

