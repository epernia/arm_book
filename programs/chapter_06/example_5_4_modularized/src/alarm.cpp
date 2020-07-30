//=====[Libraries]=============================================================

#include "mbed.h"
#include "arm_book_lib.h"

#include "alarm.h"

#include "temperature_sensor.h"
#include "gas_sensor.h"
#include "matrix_keypad.h"

//=====[Declaration of private constants]======================================

//=====[Declaration of private data types]=====================================

#define ALARM_BLINKING_TIME_GAS               1000
#define ALARM_BLINKING_TIME_OVER_TEMP          500
#define ALARM_BLINKING_TIME_GAS_AND_OVER_TEMP  100
#define ALARM_OVER_TEMP_LEVEL_CELSIUS         50.0
#define ALARM_GAS_LEVEL                        0.0 // TODO: Ver que valor de umbral poner

//=====[Declaration and intitalization of public global objects]===============

DigitalOut alarmLed(LED1);
DigitalOut incorrectCodeLed(LED3);
DigitalOut systemBlockedLed(LED2);

//=====[Declaration and intitalization of private global variables]============

static bool gasDetectorState      = OFF;
static bool overTempDetectorState = OFF;
static bool alarmState            = OFF;
static bool incorrectCodeState    = false;
static bool systemBlockedState    = false;
static int numberOfIncorrectCodes = 0;
static char codeSequence[ALARM_CODE_NUMBER_OF_KEYS] = { '1', '8', '0', '5' };

//=====[Declarations (prototypes) of private functions]========================

static void alarmLedUpdate();
static void incorrectCodeLedUpdate();
static void systemBlockedLedUpdate();

static void alarmActivationUpdate();
static void alarmDeactivationUpdate();
static void alarmCodeCheckFromMatrixKeypad();

//=====[Implementations of public functions]===================================


void alarmInit()
{
    alarmLed = OFF;
    incorrectCodeLed = OFF;
    systemBlockedLed = OFF;
    gasSensorInit();
    temperatureSensorInit();
}

void alarmUpdate()
{
    alarmActivationUpdate();
    alarmDeactivationUpdate();
}

bool alarmGasDetectorReadState()
{
    return gasDetectorState;
}

bool alarmOverTempDetectorReadState()
{
    return overTempDetectorState;
}

bool alarmReadState()
{
    return alarmState;
}

bool alarmIncorrectCodeReadState()
{
    return incorrectCodeState;
}

bool alarmSystemBlockedReadState()
{
    return systemBlockedState;
}

void alarmCodeWrite( char* newCodeSequence )
{
    for ( int i = 0; i < ALARM_CODE_NUMBER_OF_KEYS; i++) {
        codeSequence[i] = newCodeSequence[i];
    }
}

bool alarmCodeMatch( char* codeToCompare )
{
    int i;

    for (i = 0; i < ALARM_CODE_NUMBER_OF_KEYS; i++) {
        if ( codeSequence[i] != codeToCompare[i] ) {
            incorrectCodeState = true;
            numberOfIncorrectCodes = numberOfIncorrectCodes + 1;
            return false;
        }
    }

    incorrectCodeState = false;
    alarmState = OFF;
    numberOfIncorrectCodes = 0;
    return true;
}

//=====[Implementations of private functions]==================================

static void alarmLedUpdate()
{
    static int accumulatedTimeAlarm = 0;
    accumulatedTimeAlarm = accumulatedTimeAlarm + SYSTEM_TIME_INCREMENT_MS;
    
    if( alarmState ) {
        if( gasDetectorState && overTempDetectorState ) {
            if( accumulatedTimeAlarm >= ALARM_BLINKING_TIME_GAS_AND_OVER_TEMP ) {
                accumulatedTimeAlarm = 0;
                alarmLed = !alarmLed;
            }
        } else if( gasDetectorState ) {
            if( accumulatedTimeAlarm >= ALARM_BLINKING_TIME_GAS ) {
                accumulatedTimeAlarm = 0;
                alarmLed = !alarmLed;
            }
        } else if ( overTempDetectorState ) {
            if( accumulatedTimeAlarm >= ALARM_BLINKING_TIME_OVER_TEMP  ) {
                accumulatedTimeAlarm = 0;
                alarmLed = !alarmLed;
            }
        }
    } else {
        alarmLed = OFF;
    }
}

static void incorrectCodeLedUpdate()
{
    incorrectCodeLed = incorrectCodeState;
}

static void systemBlockedLedUpdate()
{
    systemBlockedLed = systemBlockedState;
}

static void alarmActivationUpdate()
{
    temperatureSensorUpdate();
    if ( temperatureSensorReadCelsius() > ALARM_OVER_TEMP_LEVEL_CELSIUS ) {
        overTempDetectorState = ON;
        alarmState = ON;
    }

    gasSensorUpdate();
    if ( gasSensorRead() > ALARM_GAS_LEVEL ) {
        gasDetectorState = ON;
        alarmState = ON;
    }

    if( alarmState == OFF ) {
        gasDetectorState = OFF;
        overTempDetectorState = OFF;
    }

    alarmLedUpdate();
}

static void alarmCodeCheckFromMatrixKeypad()
{
    static int matrixKeypadCodeIndex = 0;
    static int numberOfEnterButtonReleased = 0;
    static char buttonsPressed[ALARM_CODE_NUMBER_OF_KEYS] = {'0','0','0','0'};

    char keyReleased = matrixKeypadUpdate();

    if( keyReleased != '\0' && keyReleased != '#' ) {
        buttonsPressed[matrixKeypadCodeIndex] = keyReleased;
        if( matrixKeypadCodeIndex >= ALARM_CODE_NUMBER_OF_KEYS ) {
            matrixKeypadCodeIndex = 0;
        } else {
            matrixKeypadCodeIndex++;
        }
    }

    if( keyReleased == '#' ) {
        if( incorrectCodeState ) {
            numberOfEnterButtonReleased++;
            if( numberOfEnterButtonReleased >= 2 ) {
                numberOfEnterButtonReleased = 0;
                matrixKeypadCodeIndex = 0;
                incorrectCodeState = OFF;
            }
        } else {
            if ( alarmState ) {
                alarmCodeMatch(buttonsPressed);
            }
        }
    }
}

static void alarmDeactivationUpdate()
{
    if ( numberOfIncorrectCodes < 5 ) {
        alarmCodeCheckFromMatrixKeypad();
    } else {
        systemBlockedState = ON;
    }
    incorrectCodeLedUpdate();
    systemBlockedLedUpdate();
}