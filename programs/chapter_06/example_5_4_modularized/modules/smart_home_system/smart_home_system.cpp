//=====[Libraries]=============================================================

#include "arm_book_lib.h"

#include "smart_home_system.h"

#include "siren.h"
#include "user_interface.h"
#include "fire_alarm.h"
#include "pc_serial_com.h"
#include "event_log.h"

//=====[Declaration of private constants]======================================

//=====[Declaration of private data types]=====================================

//=====[Declaration and intitalization of public global objects]===============

//=====[Declaration and intitalization of private global variables]============

//=====[Declarations (prototypes) of private functions]========================

//=====[Implementations of public functions]===================================

void smartHomeSystemInit()
{
	sirenInit();
	userInterfaceInit();
    fireAlarmInit();
}

void smartHomeSystemUpdate()
{
    fireAlarmUpdate();	
    sirenIndicatorUpdate();
    userInterfaceUpdate();
    pcSerialComUpdate();
    eventLogUpdate();
    delay(SYSTEM_TIME_INCREMENT_MS);
}

//=====[Implementations of private functions]==================================
