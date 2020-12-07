//=====[Libraries]=============================================================

#include "mbed.h"

#include "light_level_control.h"

#include "smart_home_system.h"

//=====[Declaration of private defines]======================================

#define POTE_SAMPLE_TIME             100
#define POTE_NUMBER_OF_AVG_SAMPLES    10

//=====[Declaration of private data types]=====================================

//=====[Declaration and initialization of public global objects]===============

AnalogIn pote(A2);

//=====[Declaration of external public global variables]=======================

//=====[Declaration and initialization of public global variables]=============

//=====[Declaration and initialization of private global variables]============

static float poteReadingsMovingAverage = 0.0;
float poteAvgReadingsArray[POTE_NUMBER_OF_AVG_SAMPLES];

//=====[Declarations (prototypes) of private functions]========================

static void shiftPoteAvgReadingsArray();

//=====[Implementations of public functions]===================================

void lightLevelControlInit()
{
    return;
}

void lightLevelControlUpdate()
{
    static int accumulatedTimePote = 0;
    static int poteSampleIndex     = 0;

    accumulatedTimePote = accumulatedTimePote + SYSTEM_TIME_INCREMENT_MS;

    if ( accumulatedTimePote >= POTE_SAMPLE_TIME ) {
        if ( poteSampleIndex < POTE_NUMBER_OF_AVG_SAMPLES ) {
            poteAvgReadingsArray[poteSampleIndex] = pote.read() / 
                                                    POTE_NUMBER_OF_AVG_SAMPLES;
            poteReadingsMovingAverage = poteReadingsMovingAverage +
                                        poteAvgReadingsArray[poteSampleIndex];
            poteSampleIndex++;
        } else {
            poteReadingsMovingAverage = poteReadingsMovingAverage -
                                        poteAvgReadingsArray[0];

            shiftPoteAvgReadingsArray();

            poteAvgReadingsArray[POTE_NUMBER_OF_AVG_SAMPLES-1] =
                pote.read() / POTE_NUMBER_OF_AVG_SAMPLES;

            poteReadingsMovingAverage =
                poteReadingsMovingAverage +
                poteAvgReadingsArray[POTE_NUMBER_OF_AVG_SAMPLES-1];

        }
        accumulatedTimePote = 0;
    }
}

float lightLevelControlRead()
{
    return poteReadingsMovingAverage;
}

//=====[Implementations of private functions]==================================

static void shiftPoteAvgReadingsArray()
{
    int i = 0;
    for( i=1; i<POTE_NUMBER_OF_AVG_SAMPLES; i++ ) {
        poteAvgReadingsArray[i-1] = poteAvgReadingsArray[i];
    }
    poteAvgReadingsArray[POTE_NUMBER_OF_AVG_SAMPLES-1] = 0.0;
}