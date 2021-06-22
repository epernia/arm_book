//=====[Libraries]=============================================================

#include "mbed.h"
#include "arm_book_lib.h"

#include "event_log.h"

#include "siren.h"
#include "fire_alarm.h"
#include "user_interface.h"
#include "date_and_time.h"
#include "pc_serial_com.h"
#include "motion_sensor.h"
#include "system_event.h"

//=====[Declaration of private defines]========================================

#define EVENT_LOG_MAX_STORAGE       100

//=====[Declaration of private data types]=====================================

typedef struct storedEvent {
    time_t seconds;
    char typeOfEvent[EVENT_LOG_NAME_MAX_LENGTH];
} storedEvent_t;

//=====[Declaration and initialization of public global objects]===============

systemEvent alarmEvent("ALARM");
systemEvent gasEvent("GAS_DET");
systemEvent overTempEvent("OVER_TEMP");
systemEvent ledICEvent("LED_IC");
systemEvent ledSBEvent("LED_SB");
systemEvent motionEvent("MOTION");

//=====[Declaration of external public global variables]=======================

//=====[Declaration and initialization of public global variables]=============

//=====[Declaration and initialization of private global variables]============

static int eventsIndex     = 0;
static storedEvent_t arrayOfStoredEvents[EVENT_LOG_MAX_STORAGE];

//=====[Declarations (prototypes) of private functions]========================

static systemEvent eventLogElementStateUpdate( systemEvent event, bool state );

//=====[Implementations of public functions]===================================

void eventLogUpdate()
{
    alarmEvent = eventLogElementStateUpdate( alarmEvent, sirenStateRead() );
    gasEvent = eventLogElementStateUpdate( gasEvent, gasDetectorStateRead() );
    overTempEvent = eventLogElementStateUpdate( overTempEvent, overTemperatureDetectorStateRead() );
    ledICEvent = eventLogElementStateUpdate( ledICEvent, incorrectCodeStateRead() );
    ledSBEvent = eventLogElementStateUpdate( ledSBEvent, systemBlockedStateRead() );
    motionEvent = eventLogElementStateUpdate( motionEvent, motionSensorRead() );
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
    if ( eventsIndex < EVENT_LOG_MAX_STORAGE - 1 ) {
        eventsIndex++;
    } else {
        eventsIndex = 0;
    }

    pcSerialComStringWrite(eventAndStateStr);
    pcSerialComStringWrite("\r\n");
}

//=====[Implementations of private functions]==================================

static systemEvent eventLogElementStateUpdate( systemEvent event, bool state )
{
    if ( state != event.lastStateRead() ) {        
        eventLogWrite( state, event.getLabel() );       
    }
    event.lastStateUpdate( state );

    return event;
}