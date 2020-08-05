//=====[Libraries]=============================================================

#include "mbed.h"

#include "temperature_sensor.h"

#include "smart_home_system.h"

//=====[Declaration of private constants]======================================

#define LM35_SAMPLE_TIME             100
#define LM35_NUMBER_OF_AVG_SAMPLES    10

//=====[Declaration of private data types]=====================================

//=====[Declaration and intitalization of public global objects]===============

AnalogIn lm35(A1);

//=====[Declaration of external public global variables]=======================

//=====[Declaration and intitalization of public global variables]=============

//=====[Declaration and intitalization of private global variables]============

float lm35TempC = 0.0;
float lm35AvgReadingsArray[LM35_NUMBER_OF_AVG_SAMPLES];

//=====[Declarations (prototypes) of private functions]========================

static float analogReadingScaledWithTheLM35Formula( float analogReading );
static void shiftLm35AvgReadingsArray();

//=====[Implementations of public functions]===================================

void temperatureSensorInit( void )
{
    return;
}

void temperatureSensorUpdate( void )
{
    static int accumulatedTimeLm35 = 0;
    static int lm35SampleIndex     = 0;
    static float lm35ReadingsMovingAverage = 0.0;

    accumulatedTimeLm35 = accumulatedTimeLm35 + SYSTEM_TIME_INCREMENT_MS;

    if ( accumulatedTimeLm35 >= LM35_SAMPLE_TIME ) {
        if ( lm35SampleIndex < LM35_NUMBER_OF_AVG_SAMPLES ) {
            lm35AvgReadingsArray[lm35SampleIndex] = lm35.read() / LM35_NUMBER_OF_AVG_SAMPLES;
            lm35ReadingsMovingAverage = lm35ReadingsMovingAverage +
                                        lm35AvgReadingsArray[lm35SampleIndex];
            lm35SampleIndex++;
        } else {
            lm35ReadingsMovingAverage = lm35ReadingsMovingAverage -
                                        lm35AvgReadingsArray[0];

            shiftLm35AvgReadingsArray();

            lm35AvgReadingsArray[LM35_NUMBER_OF_AVG_SAMPLES-1] =
                lm35.read() / LM35_NUMBER_OF_AVG_SAMPLES;

            lm35ReadingsMovingAverage =
                lm35ReadingsMovingAverage +
                lm35AvgReadingsArray[LM35_NUMBER_OF_AVG_SAMPLES-1];

            lm35TempC = analogReadingScaledWithTheLM35Formula(
                            lm35ReadingsMovingAverage );
        }
        accumulatedTimeLm35 = 0;
    }
}

float temperatureSensorReadCelsius( void )
{
    return lm35TempC;
}

float temperatureSensorReadFahrenheit( void )
{
    return celsiusToFahrenheit( lm35TempC );
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

static void shiftLm35AvgReadingsArray()
{
    int i = 0;
    for( i=1; i<LM35_NUMBER_OF_AVG_SAMPLES; i++ ) {
        lm35AvgReadingsArray[i-1] = lm35AvgReadingsArray[i];
    }
    lm35AvgReadingsArray[LM35_NUMBER_OF_AVG_SAMPLES-1] = 0.0;
}