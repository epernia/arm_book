//=====[Libraries]=============================================================

#include "mbed.h"
#include "arm_book_lib.h"

#include "fire_alarm.h"

#include "siren.h"
#include "user_interface.h"
#include "code.h"
#include "date_and_time.h"
#include "temperature_sensor.h"
#include "gas_sensor.h"
#include "matrix_keypad.h"

//=====[Declaration of private defines]======================================

#define TEMPERATURE_C_LIMIT_ALARM               50.0
#define GAS_CONCENTRATION_LIMIT_ALARM            0.0 // TODO: Ver que valor de umbral poner
#define SIREN_BLINKING_TIME_GAS               1000
#define SIREN_BLINKING_TIME_OVER_TEMP          500
#define SIREN_BLINKING_TIME_GAS_AND_OVER_TEMP  100

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
static void fireAlarmDeactivationUpdate();
static void fireAlarmDeactivate();
static int fireAlarmBlinkTime();

//=====[Implementations of public functions]===================================

void fireAlarmInit()
{
    temperatureSensorInit();
    gasSensorInit();
    sirenInit();
}

void fireAlarmUpdate()
{
    fireAlarmActivationUpdate();
    fireAlarmDeactivationUpdate();
    if ( overTemperatureDetected || gasDetected )  {
        sirenIndicatorUpdate( fireAlarmBlinkTime() );
    }
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

//=====[Implementations of private functions]==================================

static void fireAlarmActivationUpdate()
{
    temperatureSensorUpdate();
    gasSensorUpdate();

    overTemperatureDetectorState = temperatureSensorReadCelsius() > 
                                   TEMPERATURE_C_LIMIT_ALARM;

    if ( overTemperatureDetectorState ) {
        overTemperatureDetected = ON;
        sirenStateWrite(ON);
    }

    gasDetectorState = gasSensorRead() > GAS_CONCENTRATION_LIMIT_ALARM;

    if ( gasDetectorState ) {
        gasDetected = ON;
        sirenStateWrite(ON);
    }
}

static void fireAlarmDeactivationUpdate()
{
    if ( overTemperatureDetected || gasDetected )  { 
        if ( codeMatchFrom(CODE_KEYPAD) ||
             codeMatchFrom(CODE_PC_SERIAL) ) {
            fireAlarmDeactivate();
        }
    }
}

static void fireAlarmDeactivate()
{
    sirenStateWrite(OFF);
    overTemperatureDetected = OFF;
    gasDetected             = OFF;    
}

static int fireAlarmBlinkTime()
{
    if( gasDetectedRead() && overTemperatureDetectedRead() ) {
        return SIREN_BLINKING_TIME_GAS_AND_OVER_TEMP;
    } else if ( gasDetectedRead() ) {
        return SIREN_BLINKING_TIME_GAS;
    } else if ( overTemperatureDetectedRead() ) {
        return SIREN_BLINKING_TIME_OVER_TEMP;
    } else {
        return 0;
    }
}
