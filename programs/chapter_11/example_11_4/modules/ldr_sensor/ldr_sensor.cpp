//=====[Libraries]=============================================================

#include "arm_book_lib.h"

#include "smart_home_system.h"
#include "ldr_sensor.h"

//=====[Declaration of private defines]======================================

#define LDR_SAMPLE_TIME             100
#define LDR_NUMBER_OF_AVG_SAMPLES    10

//=====[Declaration of private data types]=====================================

//=====[Declaration and initialization of public global objects]===============

AnalogIn LDR(A4);

//=====[Declaration of external public global variables]=======================

//=====[Declaration and initialization of public global variables]=============

//=====[Declaration and initialization of private global variables]============

static float LDRReadingsMovingAverage = 0.0;
float LDRAvgReadingsArray[LDR_NUMBER_OF_AVG_SAMPLES];

//=====[Declarations (prototypes) of private functions]========================

static void shiftLDRAvgReadingsArray();

//=====[Implementations of public functions]===================================

void LDRSensorInit()
{
    return;
}

void LDRSensorUpdate()
{
    static int accumulatedTimeLDR = 0;
    static int LDRSampleIndex     = 0;

    accumulatedTimeLDR = accumulatedTimeLDR + SYSTEM_TIME_INCREMENT_MS;

    if ( accumulatedTimeLDR >= LDR_SAMPLE_TIME ) {
        if ( LDRSampleIndex < LDR_NUMBER_OF_AVG_SAMPLES ) {
            LDRAvgReadingsArray[LDRSampleIndex] = LDR.read() / 
                                                    LDR_NUMBER_OF_AVG_SAMPLES;
            LDRReadingsMovingAverage = LDRReadingsMovingAverage +
                                        LDRAvgReadingsArray[LDRSampleIndex];
            LDRSampleIndex++;
        } else {
            LDRReadingsMovingAverage = LDRReadingsMovingAverage -
                                        LDRAvgReadingsArray[0];

            shiftLDRAvgReadingsArray();

            LDRAvgReadingsArray[LDR_NUMBER_OF_AVG_SAMPLES-1] =
                LDR.read() / LDR_NUMBER_OF_AVG_SAMPLES;

            LDRReadingsMovingAverage =
                LDRReadingsMovingAverage +
                LDRAvgReadingsArray[LDR_NUMBER_OF_AVG_SAMPLES-1];

        }
        accumulatedTimeLDR = 0;
    }
}

float LDRSensorRead()
{
    return LDRReadingsMovingAverage;
}

//=====[Implementations of private functions]==================================

static void shiftLDRAvgReadingsArray()
{
    int i = 0;
    for( i=1; i<LDR_NUMBER_OF_AVG_SAMPLES; i++ ) {
        LDRAvgReadingsArray[i-1] = LDRAvgReadingsArray[i];
    }
    LDRAvgReadingsArray[LDR_NUMBER_OF_AVG_SAMPLES-1] = 0.0;
}