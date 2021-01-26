//=====[Libraries]=============================================================

#include "mbed.h"
#include "arm_book_lib.h"

#include "event_log.h"

#include "siren.h"
#include "fire_alarm.h"
#include "user_interface.h"
#include "date_and_time.h"
#include "pc_serial_com.h"

//=====[Declaration of private defines]======================================

//=====[Declaration of private data types]=====================================

typedef struct systemEvent {
    time_t seconds;
    char typeOfEvent[EVENT_LOG_NAME_MAX_LENGTH];
} systemEvent_t;

//=====[Declaration and initialization of public global objects]===============

//=====[Declaration of external public global variables]=======================

//=====[Declaration and initialization of public global variables]=============

//=====[Declaration and initialization of private global variables]============

static bool sirenLastState = OFF;
static bool gasLastState   = OFF;
static bool tempLastState  = OFF;
static bool ICLastState    = OFF;
static bool SBLastState    = OFF;
static int eventsIndex     = 0;
static systemEvent_t arrayOfStoredEvents[EVENT_LOG_MAX_STORAGE];

//=====[Declarations (prototypes) of private functions]========================

static void eventLogElementStateUpdate( bool lastState,
                                        bool currentState,
                                        const char* elementName );

//=====[Implementations of public functions]===================================

void eventLogUpdate()
{
    bool currentState = sirenStateRead();
    eventLogElementStateUpdate( sirenLastState, currentState, "ALARM" );
    sirenLastState = currentState;

    currentState = gasDetectorStateRead();
    eventLogElementStateUpdate( gasLastState, currentState, "GAS_DET" );
    gasLastState = currentState;

    currentState = overTemperatureDetectorStateRead();
    eventLogElementStateUpdate( tempLastState, currentState, "OVER_TEMP" );
    tempLastState = currentState;

    currentState = incorrectCodeStateRead();
    eventLogElementStateUpdate( ICLastState, currentState, "LED_IC" );
    ICLastState = currentState;

    currentState = systemBlockedStateRead();
    eventLogElementStateUpdate( SBLastState ,currentState, "LED_SB" );
    SBLastState = currentState;
}

int eventLogNumberOfStoredEvents()
{
    return eventsIndex;
}

void eventLogRead( int index, char* str )
{
    str[0] = 0;

    strncat( str, "Event = ", strlen("Event = ") );

    strncat( str, arrayOfStoredEvents[index].typeOfEvent,
             strlen(arrayOfStoredEvents[index].typeOfEvent) );

    strncat( str, "\r\nDate and Time = ", strlen("\r\nDate and Time = ") );

    strncat( str, ctime(&arrayOfStoredEvents[index].seconds),
             strlen(ctime(&arrayOfStoredEvents[index].seconds)) );

    strncat( str, "\r\n", strlen("\r\n") );
}

void eventLogWrite( bool currentState, const char* elementName )
{
    char eventAndStateStr[EVENT_LOG_NAME_MAX_LENGTH];
    eventAndStateStr[0] = 0;
    strncat( eventAndStateStr, elementName, strlen(elementName) );
    if ( currentState ) {
        strncat( eventAndStateStr, "_ON", strlen("_ON") );
    } else {
        strncat( eventAndStateStr, "_OFF", strlen("_OFF") );
    }

    arrayOfStoredEvents[eventsIndex].seconds = time(NULL);
    strcpy( arrayOfStoredEvents[eventsIndex].typeOfEvent, eventAndStateStr );
    if ( eventsIndex < EVENT_LOG_MAX_STORAGE ) {
        eventsIndex++;
    } else {
        eventsIndex = 0;
    }

    pcSerialComStringWrite(eventAndStateStr);
    pcSerialComStringWrite("\r\n");
}

//=====[Implementations of private functions]==================================

static void eventLogElementStateUpdate( bool lastState,
                                        bool currentState,
                                        const char* elementName )
{
    if ( lastState != currentState ) {        
        eventLogWrite( currentState, elementName );       
    }
}