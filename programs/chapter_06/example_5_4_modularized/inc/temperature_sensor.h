//=====[#include guards - begin]===============================================

#ifndef _TEMPERATURE_SENSOR_H_
#define _TEMPERATURE_SENSOR_H_

//=====[Libraries]=============================================================

//=====[Declaration of public constants]=======================================

//=====[Declaration of public data types]======================================

//=====[Declarations (prototypes) of public functions]=========================

void temperatureSensorInit( void );
void temperatureSensorUpdate( void );
float temperatureSensorReadCelsius( void );
float temperatureSensorReadFahrenheit( void );
float celsiusToFahrenheit( float tempInCelsiusDegrees );

//=====[#include guards - end]=================================================

#endif // _TEMPERATURE_SENSOR_H_