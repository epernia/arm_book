//=====[Libraries]=============================================================

#include "event_log.h"

//=====[Declaration of private data types]=====================================

typedef struct systemEvent {
    time_t seconds;
    char typeOfEvent[EVENT_NAME_MAX_LENGTH];
} systemEvent_t;

//=====[Declaration of private constants]======================================

#define EVENT_MAX_STORAGE                      100
#define EVENT_NAME_MAX_LENGTH                   15

//=====[Declaration of external public global objects]=========================



//=====[Declaration and intitalization of public global objects]===============



//=====[Declaration and intitalization of private global objects]==============



//=====[Declaration of external public global variables]=======================



//=====[Declaration and intitalization of public global variables]=============



//=====[Declaration and intitalization of private global variables]============

bool alarmLastState        = OFF;
bool gasLastState          = OFF;
bool tempLastState         = OFF;
bool ICLastState           = OFF;
bool SBLastState           = OFF;

int eventsIndex            = 0;
systemEvent_t arrayOfStoredEvents[EVENT_MAX_STORAGE];

//=====[Declarations (prototypes) of private functions]========================



//=====[Implementations of public functions]===================================

void eventLogUpdate()
{
    systemElementStateUpdate( alarmLastState,alarmState, "ALARM" );
    alarmLastState = alarmState;

    systemElementStateUpdate( gasLastState,gasDetector, "GAS_DET" );
    gasLastState = gasDetector;

    systemElementStateUpdate( tempLastState,overTempDetector, "OVER_TEMP" );
    tempLastState = overTempDetector;

    systemElementStateUpdate( ICLastState,incorrectCodeLed, "LED_IC" );
    ICLastState = incorrectCodeLed;

    systemElementStateUpdate( SBLastState,systemBlockedLed, "LED_SB" );
    SBLastState = systemBlockedLed;
}

void systemElementStateUpdate( bool lastState,
                               bool currentState,
                               const char* elementName )
{
    if ( lastState != currentState ) {
        
        char eventAndStateStr[EVENT_NAME_MAX_LENGTH];
        eventAndStateStr[0] = 0;
        strncat( eventAndStateStr, elementName, strlen(elementName) );
        if ( currentState ) {
            strncat( eventAndStateStr, "_ON", strlen("_ON") );
        } else {
            strncat( eventAndStateStr, "_OFF", strlen("_OFF") );
        }

        arrayOfStoredEvents[eventsIndex].seconds = time(NULL);
        strcpy( arrayOfStoredEvents[eventsIndex].typeOfEvent,eventAndStateStr );
        if ( eventsIndex < EVENT_MAX_STORAGE ) {
            eventsIndex++;
        } else {
            eventsIndex = 0;
        }

        uartUsb.printf("%s",eventAndStateStr);
        uartUsb.printf("\r\n");

        uartBle.printf("%s",eventAndStateStr);
        uartBle.printf("\r\n");
    }
}

//=====[Implementations of private functions]==================================

