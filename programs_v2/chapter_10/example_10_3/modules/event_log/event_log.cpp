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
#include "sd_card.h"
#include "ble_com.h"

//=====[Declaration of private defines]======================================

//=====[Declaration of private data types]=====================================

typedef struct storedEvent {
    time_t seconds;
    char typeOfEvent[EVENT_LOG_NAME_MAX_LENGTH];
    bool storedInSd;
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

//=====[Implementations of public functions]===================================

void eventLogUpdate()
{
    alarmEvent.stateUpdate( sirenStateRead() );
    gasEvent.stateUpdate(  gasDetectorStateRead() );
    overTempEvent.stateUpdate(  overTemperatureDetectorStateRead() );
    ledICEvent.stateUpdate(  incorrectCodeStateRead() );
    ledSBEvent.stateUpdate( systemBlockedStateRead() );
    motionEvent.stateUpdate( motionSensorRead() );
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
    
    arrayOfStoredEvents[eventsIndex].storedInSd = false;

    pcSerialComStringWrite(eventAndStateStr);
    pcSerialComStringWrite("\r\n");
    
    bleComStringWrite(eventAndStateStr);
    bleComStringWrite("\r\n");
}

bool eventLogSaveToSdCard()
{
    char fileName[SD_CARD_FILENAME_MAX_LENGTH];
    char eventStr[EVENT_LOG_NAME_MAX_LENGTH];
    bool eventsStored = false;

    time_t seconds;
    int i;

    seconds = time(NULL);
    fileName[0] = 0;

    strftime( fileName, SD_CARD_FILENAME_MAX_LENGTH, "%Y_%m_%d_%H_%M_%S", localtime(&seconds) );
    strncat( fileName, ".txt", strlen(".txt") );

    for (i = 0; i < eventLogNumberOfStoredEvents(); i++) {
        if ( !arrayOfStoredEvents[i].storedInSd ) {
            eventLogRead( i, eventStr );
            if ( sdCardWriteFile( fileName, eventStr ) ){
                arrayOfStoredEvents[i].storedInSd = true;
                pcSerialComStringWrite("Storing event ");
                pcSerialComIntWrite(i+1);
                pcSerialComStringWrite(" in file ");
                pcSerialComStringWrite(fileName);
                pcSerialComStringWrite("\r\n");
                eventsStored = true;
            }
        }
    }

    if ( eventsStored ) {
        pcSerialComStringWrite("New events successfully stored in SD card\r\n\r\n");
    } else {
        pcSerialComStringWrite("No new events to store in SD card\r\n\r\n");
    }

    return true;
}

//=====[Implementations of private functions]==================================
