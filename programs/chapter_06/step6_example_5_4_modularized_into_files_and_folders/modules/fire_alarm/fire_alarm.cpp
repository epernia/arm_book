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

//=====[Declaration of private constants]======================================

#define TEMPERATURE_C_LIMIT_ALARM       50.0
#define GAS_CONCENTRATION_LIMIT_ALARM   0.0 // TODO: Ver que valor de umbral poner

//=====[Declaration of private data types]=====================================

//=====[Declaration and intitalization of public global objects]===============

//=====[Declaration and intitalization of private global variables]============

//=====[Declaration of external public global variables]=======================

//=====[Declaration and intitalization of public global variables]=============

//=====[Declaration and intitalization of private global variables]============

static bool gasDetected           = OFF;
static bool overTempDetected      = OFF;
static bool gasDetectorState      = OFF;
static bool overTempDetectorState = OFF;

//=====[Declarations (prototypes) of private functions]========================

static void fireAlarmActivationUpdate();

static void fireAlarmDeactivationUpdate();

static void fireAlarmDeactivate();

//=====[Implementations of public functions]===================================

void fireAlarmInit()
{
    gasSensorInit();
    temperatureSensorInit();
}

void fireAlarmUpdate()
{
    fireAlarmActivationUpdate();
    fireAlarmDeactivationUpdate();
}

bool gasDetectorStateRead()
{
    return gasDetectorState;
}

bool overTempDetectorStateRead()
{
    return overTempDetectorState;
}

bool gasDetectedRead()
{
    return gasDetected;
}

bool overTempDetectedRead()
{
    return overTempDetected;
}

//=====[Implementations of private functions]==================================

static void fireAlarmActivationUpdate()
{
    temperatureSensorUpdate();
    gasSensorUpdate();

    overTempDetectorState = temperatureSensorReadCelsius() > 
                            TEMPERATURE_C_LIMIT_ALARM;

    if ( overTempDetectorState ) {
        overTempDetected = ON;
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
    if ( sirenStateRead() ) {
        if ( codeMatchFrom(CODE_KEYPAD) ||
             codeMatchFrom(CODE_PC_SERIAL) ) {
            fireAlarmDeactivate();
        }
    }
}

static void fireAlarmDeactivate()
{
	sirenStateWrite(OFF);
    overTempDetected = OFF;
    gasDetected      = OFF;    
}