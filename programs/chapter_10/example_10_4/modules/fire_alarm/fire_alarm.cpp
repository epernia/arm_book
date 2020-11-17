//=====[Libraries]=============================================================

#include "mbed.h"
#include "arm_book_lib.h"

#include "fire_alarm.h"

#include "user_interface.h"
#include "date_and_time.h"
#include "temperature_sensor.h"
#include "gas_sensor.h"

//=====[Declaration of private defines]======================================

#define TEMPERATURE_C_LIMIT_ALARM               50.0
#define GAS_CONCENTRATION_LIMIT_ALARM            0.0 // TODO: Ver que valor de umbral poner

//=====[Declaration of private data types]=====================================

//=====[Declaration and initialization of public global objects]===============

//=====[Declaration and initialization of private global variables]============

//=====[Declaration of external public global variables]=======================

//=====[Declaration and initialization of public global variables]=============

//=====[Declaration and initialization of private global variables]============

static bool gasDetected                  = OFF;
static bool overTemperatureDetected      = OFF;
static bool gasDetectorState             = OFF;
static bool overTemperatureDetectorState = OFF;

//=====[Declarations (prototypes) of private functions]========================

static void fireAlarmActivationUpdate();
static int fireAlarmBlinkTime();

//=====[Implementations of public functions]===================================

void fireAlarmInit()
{
    temperatureSensorInit();
    gasSensorInit();
}

void fireAlarmUpdate()
{
    fireAlarmActivationUpdate();
}

bool gasDetectorStateRead()
{
    return gasDetectorState;
}

bool overTemperatureDetectorStateRead()
{
    return overTemperatureDetectorState;
}

bool gasDetectedRead()
{
    return gasDetected;
}

bool overTemperatureDetectedRead()
{
    return overTemperatureDetected;
}

void fireAlarmDeactivate()
{
    overTemperatureDetected = OFF;
    gasDetected             = OFF;    
}

//=====[Implementations of private functions]==================================

static void fireAlarmActivationUpdate()
{
    temperatureSensorUpdate();
    gasSensorUpdate();

    overTemperatureDetectorState = temperatureSensorReadCelsius() > 
                                   TEMPERATURE_C_LIMIT_ALARM;

    if ( overTemperatureDetectorState ) {
        overTemperatureDetected = ON;
    }

    gasDetectorState = gasSensorRead() > GAS_CONCENTRATION_LIMIT_ALARM;

    if ( gasDetectorState ) {
        gasDetected = ON;
    }
}
