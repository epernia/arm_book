//=====[Libraries]=============================================================

#include "alarm.h"
#include "shared.h"


#include "mbed.h"
#include "arm_book_lib.h"

#include "pc_serial_communication.h"
#include "smartphone_ble_communication.h"
#include "event_log.h"

#include "temperature_sensor.h"
#include "gas_sensor.h"

#include "matrix_keypad.h"

//=====[Declaration of private data types]=====================================



//=====[Declaration of private constants]======================================

#define NUMBER_OF_KEYS                           4

#define BLINKING_TIME_GAS_ALARM               1000
#define BLINKING_TIME_OVER_TEMP_ALARM          500
#define BLINKING_TIME_GAS_AND_OVER_TEMP_ALARM  100

#define OVER_TEMP_LEVEL                         50

//=====[Declaration of external public global objects]=========================

extern DigitalIn gasDetector;

//=====[Declaration and intitalization of public global objects]===============

DigitalOut alarmLed(LED1);
DigitalOut incorrectCodeLed(LED3);
DigitalOut systemBlockedLed(LED2);

//=====[Declaration and intitalization of private global objects]==============



//=====[Declaration of external public global variables]=======================

bool alarmState       = OFF;
bool incorrectCode    = false;
bool overTempDetector = OFF;

bool gasDetectorState      = OFF;
bool overTempDetectorState = OFF;

int numberOfIncorrectCodes = 0;
int keyBeingCompared       = 0;
int accumulatedTimeAlarm   = 0;
int numberOfEnterButtonReleasedEvents = 0;

char codeSequence[NUMBER_OF_KEYS]   = { '1', '8', '0', '5' };
char buttonsPressed[NUMBER_OF_KEYS] = { '0', '0', '0', '0' };

int matrixKeypadCodeIndex = 0;

//=====[Declaration and intitalization of public global variables]=============



//=====[Declaration and intitalization of private global variables]============



//=====[Declarations (prototypes) of private functions]========================

static void alarmActivationUpdate();
static void alarmDeactivationUpdate();
static bool areEqual();

//=====[Implementations of public functions]===================================

void alarmInit()
{
    gasDetector.mode(PullDown);
    alarmLed = OFF;
    incorrectCodeLed = OFF;
    systemBlockedLed = OFF;
    matrixKeypadInit();
}

int alarmUpdate()
{
    alarmActivationUpdate();
    alarmDeactivationUpdate();
    uartTask();
    eventLogUpdate();
    delay(TIME_INCREMENT_MS);
}

//=====[Implementations of private functions]==================================

static void alarmActivationUpdate()
{
    accumulatedTimeLm35 = accumulatedTimeLm35 + TIME_INCREMENT_MS;

    if ( accumulatedTimeLm35 >= LM35_SAMPLE_TIME ) {
 
        if ( lm35SampleIndex < NUMBER_OF_AVG_SAMPLES ) {
            lm35AvgReadingsArray[lm35SampleIndex] = lm35.read() / NUMBER_OF_AVG_SAMPLES;
            lm35ReadingsMovingAverage = lm35ReadingsMovingAverage +
                                        lm35AvgReadingsArray[lm35SampleIndex];
            lm35SampleIndex++;
        } else {
            lm35ReadingsMovingAverage = lm35ReadingsMovingAverage -
                                        lm35AvgReadingsArray[0];

            shiftLm35AvgReadingsArray();

            lm35AvgReadingsArray[NUMBER_OF_AVG_SAMPLES-1] =
                lm35.read() / NUMBER_OF_AVG_SAMPLES;

            lm35ReadingsMovingAverage =
                lm35ReadingsMovingAverage +
                lm35AvgReadingsArray[NUMBER_OF_AVG_SAMPLES-1];

            lm35TempC = analogReadingScaledWithTheLM35Formula(
                            lm35ReadingsMovingAverage );
        }
        accumulatedTimeLm35 = 0;
    }
    
    

    if ( lm35TempC > OVER_TEMP_LEVEL ) {
        overTempDetector = ON;
    } else {
        overTempDetector = OFF;
    }

    if( overTempDetector ) {
        overTempDetectorState = ON;
        alarmState = ON;
    }

    if( gasDetector) {
        gasDetectorState = ON;
        alarmState = ON;
    }

    if( alarmState ) {
        accumulatedTimeAlarm = accumulatedTimeAlarm + TIME_INCREMENT_MS;

        if( gasDetectorState && overTempDetectorState ) {
            if( accumulatedTimeAlarm >= BLINKING_TIME_GAS_AND_OVER_TEMP_ALARM ) {
                accumulatedTimeAlarm = 0;
                alarmLed = !alarmLed;
            }
        } else if( gasDetectorState ) {
            if( accumulatedTimeAlarm >= BLINKING_TIME_GAS_ALARM ) {
                accumulatedTimeAlarm = 0;
                alarmLed = !alarmLed;
            }
        } else if ( overTempDetectorState ) {
            if( accumulatedTimeAlarm >= BLINKING_TIME_OVER_TEMP_ALARM  ) {
                accumulatedTimeAlarm = 0;
                alarmLed = !alarmLed;
            }
        }
    } else {
        alarmLed = OFF;
        gasDetectorState = OFF;
        overTempDetectorState = OFF;
    }
}

static void alarmDeactivationUpdate()
{
    if ( numberOfIncorrectCodes < 5 ) {
        char keyReleased = matrixKeypadUpdate();
        if( keyReleased != '\0' && keyReleased != '#' ) {
            buttonsPressed[matrixKeypadCodeIndex] = keyReleased;
            if( matrixKeypadCodeIndex >= NUMBER_OF_KEYS ) {
                matrixKeypadCodeIndex = 0;
            } else {
                matrixKeypadCodeIndex++;
            }
        }
        if( keyReleased == '#' ) {
            if( incorrectCodeLed ) {
                numberOfEnterButtonReleasedEvents++;
                if( numberOfEnterButtonReleasedEvents >= 2 ) {
                    incorrectCodeLed = OFF;
                    numberOfEnterButtonReleasedEvents = 0;
                    matrixKeypadCodeIndex = 0;
                }
            } else {
                if ( alarmState ) {
                    if ( areEqual() ) {
                        alarmState = OFF;
                        numberOfIncorrectCodes = 0;
                        matrixKeypadCodeIndex = 0;
                    } else {
                        incorrectCodeLed = ON;
                        numberOfIncorrectCodes++;
                    }
                }
            }
        }
    } else {
        systemBlockedLed = ON;
    }
}

static bool areEqual()
{
    int i;

    for (i = 0; i < NUMBER_OF_KEYS; i++) {
        if (codeSequence[i] != buttonsPressed[i]) {
            return false;
        }
    }

    return true;
}