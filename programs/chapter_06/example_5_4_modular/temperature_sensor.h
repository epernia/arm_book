//=====[#include guards - begin]===============================================

#ifndef _TEMPERATURE_SENSOR_H_
#define _TEMPERATURE_SENSOR_H_

//=====[Libraries]=============================================================

#include "mbed.h"
#include "arm_book_lib.h"

//=====[Declaration of public data types]======================================



//=====[Declaration of public constants]=======================================



//=====[Declarations (prototypes) of public functions]=========================

float analogReadingScaledWithTheLM35Formula( float analogReading );
float celsiusToFahrenheit( float tempInCelsiusDegrees );
void shiftLm35AvgReadingsArray();

//=====[#include guards - end]=================================================

#endif // _TEMPERATURE_SENSOR_H_