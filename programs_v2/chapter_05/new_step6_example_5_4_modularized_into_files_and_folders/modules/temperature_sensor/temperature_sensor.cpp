//=====[Libraries]=============================================================

#include "mbed.h"

#include "temperature_sensor.h"

#include "smart_home_system.h"

//=====[Declaration of private defines]======================================

#define LM35_SAMPLE_TIME             100
#define LM35_NUMBER_OF_AVG_SAMPLES    10

//=====[Declaration of private data types]=====================================

//=====[Declaration and initialization of public global objects]===============

AnalogIn lm35(A1);

//=====[Declaration of external public global variables]=======================

//=====[Declaration and initialization of public global variables]=============

//=====[Declaration and initialization of private global variables]============

float lm35TemperatureC = 0.0;
float lm35ReadingsArray[LM35_NUMBER_OF_AVG_SAMPLES];

//=====[Declarations (prototypes) of private functions]========================

static float analogReadingScaledWithTheLM35Formula( float analogReading );

//=====[Implementations of public functions]===================================

void temperatureSensorInit()
{
    return;
}

void temperatureSensorUpdate()
{
    static int accumulatedTimeLm35 = 0;
    static int lm35SampleIndex     = 0;
    static float lm35AveragedValue = 0.0;
    int i;

    accumulatedTimeLm35 = accumulatedTimeLm35 + SYSTEM_TIME_INCREMENT_MS;

    if ( accumulatedTimeLm35 >= LM35_SAMPLE_TIME ) {
        
        lm35ReadingsArray[lm35SampleIndex] = lm35.read();
        
        lm35AveragedValue = 0.0;
        for (i = 0; i < LM35_NUMBER_OF_AVG_SAMPLES; i++) {
            lm35AveragedValue = lm35AveragedValue + lm35ReadingsArray[i];
        }
        lm35AveragedValue = lm35AveragedValue / LM35_NUMBER_OF_AVG_SAMPLES;

        lm35SampleIndex++;
        if ( lm35SampleIndex >= LM35_NUMBER_OF_AVG_SAMPLES) {
            lm35SampleIndex = 0;
        }

            lm35TemperatureC = analogReadingScaledWithTheLM35Formula(
                            lm35AveragedValue );
        accumulatedTimeLm35 = 0;
    }
}


float temperatureSensorReadCelsius()
{
    return lm35TemperatureC;
}

float temperatureSensorReadFahrenheit()
{
    return celsiusToFahrenheit( lm35TemperatureC );
}

float celsiusToFahrenheit( float tempInCelsiusDegrees )
{
    return ( tempInCelsiusDegrees * 9.0 / 5.0 + 32.0 );
}

//=====[Implementations of private functions]==================================

static float analogReadingScaledWithTheLM35Formula( float analogReading )
{
    return ( analogReading * 3.3 / 0.01 );
}