//=====[Libraries]=============================================================

#include "arm_book_lib.h"

#include "smart_home_system.h"

#include "siren.h"
#include "user_interface.h"
#include "fire_alarm.h"
#include "pc_serial_com.h"
#include "event_log.h"
#include "sd_card.h"
#include "sapi.h"
#include "wifi_com.h"

//=====[Declaration of private defines]======================================

//=====[Declaration of private data types]=====================================

//=====[Declaration and initialization of public global objects]===============

//=====[Declaration of external public global variables]=======================

//=====[Declaration and initialization of public global variables]=============

char systemBuffer[EVENT_STR_LENGTH*EVENT_LOG_MAX_STORAGE];
static delay_t smartHomeSystemDelay;

//=====[Declaration and initialization of private global variables]============

//=====[Declarations (prototypes) of private functions]========================

//=====[Implementations of public functions]===================================

void smartHomeSystemInit()
{
    tickInit(1);          // Set 1 ms tick counter
    userInterfaceInit();
    fireAlarmInit();
    pcSerialComInit();
    sdCardInit();
    wifiComInit();
    delayInit( &smartHomeSystemDelay, SYSTEM_TIME_INCREMENT_MS );
}

void smartHomeSystemUpdate()
{
    if( delayRead(&smartHomeSystemDelay) ) {
        userInterfaceUpdate();
        fireAlarmUpdate();
        eventLogUpdate();
    }
    pcSerialComUpdate();
    wifiComUpdate();
}

//=====[Implementations of private functions]==================================
