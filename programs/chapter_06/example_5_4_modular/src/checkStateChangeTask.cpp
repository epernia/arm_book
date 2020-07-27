#include "checkStateChangeTask.h"

typedef struct systemEvent {
    time_t seconds;
    char typeOfEvent[MAX_NUMBER_OF_CHARACTERS];
} systemEvent_t;

bool alarmLastState = OFF;
bool gasLastState   = OFF;
bool tempLastState  = OFF;
bool ICLastState    = OFF;
bool SBLastState    = OFF;

extern bool gasDetectorState;
extern bool overTempDetectorState;

extern bool alarmState;
extern DigitalIn gasDetector;
extern bool overTempDetector;
extern DigitalOut incorrectCodeLed;
extern DigitalOut systemBlockedLed;
time_t seconds;
extern float lm35TempC;
extern systemEvent_t arrayOfStoredEvents[MAX_NUMBER_OF_EVENTS];
extern int eventsIndex;

void checkStateChangeTask()
{
    checkStateChange( alarmLastState,alarmState, "ALARM" );
    alarmLastState = alarmState;

    checkStateChange( gasLastState,gasDetector, "GAS_DET" );
    gasLastState = gasDetector;

    checkStateChange( tempLastState,overTempDetector, "OVER_TEMP" );
    tempLastState = overTempDetector;

    checkStateChange( ICLastState,incorrectCodeLed, "LED_IC" );
    ICLastState = incorrectCodeLed;

    checkStateChange( SBLastState,systemBlockedLed, "LED_SB" );
    SBLastState = systemBlockedLed;
}                                                                              

void checkStateChange( bool lastState,
        bool currentState,
        const char* elementName )
{
    char typeOfEventAux[MAX_NUMBER_OF_CHARACTERS];

    if ( lastState != currentState ) {
        typeOfEventAux[0] = 0;
        strncat( typeOfEventAux, elementName, strlen( elementName ) );
        if ( currentState ) {
            strncat( typeOfEventAux, "_ON", strlen("_ON") );
        } else {
            strncat( typeOfEventAux, "_OFF", strlen("_OFF") );
        }

        arrayOfStoredEvents[eventsIndex].seconds = time(NULL);
        strcpy( arrayOfStoredEvents[eventsIndex].typeOfEvent,typeOfEventAux );

        if ( eventsIndex < MAX_NUMBER_OF_EVENTS ) {
            eventsIndex++;
        } else {
            eventsIndex = 0;
        }
    }
}