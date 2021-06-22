//=====[Libraries]=============================================================

#include "arm_book_lib.h"

#include "smart_home_system.h"

#include "alarm.h"
#include "user_interface.h"
#include "fire_alarm.h"
#include "intruder_alarm.h"
#include "pc_serial_com.h"
#include "event_log.h"
#include "motion_sensor.h"
#include "motor.h"
#include "gate.h"
#include "light_system.h"
#include "audio.h"
#include "sd_card.h"
#include "ble_com.h"

#include "sapi.h"
#include "http_server.h"

//=====[Declaration of private defines]========================================

//=====[Declaration of private data types]=====================================

//=====[Declaration and initialization of public global objects]===============

char systemBuffer[EVENT_STR_LENGTH*EVENT_LOG_MAX_STORAGE];
static delay_t smartHomeSystemDelay;

//=====[Declaration of external public global variables]=======================

//=====[Declaration and initialization of public global variables]=============

//=====[Declaration and initialization of private global variables]============

//=====[Declarations (prototypes) of private functions]========================

//=====[Implementations of public functions]===================================

void smartHomeSystemInit()
{
    tickInit();
    alarmInit();
    fireAlarmInit();
    intruderAlarmInit();
    pcSerialComInit();
    motorControlInit();
    gateInit();
    lightSystemInit();
    sdCardInit();
    userInterfaceInit();
    httpServerInit();
    audioInit();
    delayInit( &smartHomeSystemDelay, SYSTEM_TIME_INCREMENT_MS );
}

void smartHomeSystemUpdate()
{
    if( delayRead(&smartHomeSystemDelay) ) {
        userInterfaceUpdate();
        fireAlarmUpdate();
        intruderAlarmUpdate();
        alarmUpdate();
        eventLogUpdate();
        pcSerialComUpdate();
        motorControlUpdate();
        lightSystemUpdate();
        bleComUpdate();
    }    
    httpServerUpdate();
}

//=====[Implementations of private functions]==================================
