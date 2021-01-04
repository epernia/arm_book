//=====[Libraries]=============================================================

#include "arm_book_lib.h"

#include "smart_home_system.h"

#include "siren.h"
#include "user_interface.h"
#include "fire_alarm.h"
#include "pc_serial_com.h"
#include "event_log.h"
#include "motor.h"
#include "gate.h"
#include "motion_sensor.h"

//=====[Declaration of private defines]======================================

//=====[Declaration of private data types]=====================================

//=====[Declaration and initialization of public global objects]===============

//=====[Declaration of external public global variables]=======================

//=====[Declaration and initialization of public global variables]=============

//=====[Declaration and initialization of private global variables]============

//=====[Declarations (prototypes) of private functions]========================

//=====[Implementations of public functions]===================================

void smartHomeSystemInit()
{
    userInterfaceInit();
    fireAlarmInit();
    pcSerialComInit();
    motorControlInit();
    gateInit();
    motionSensorInit();
}

void smartHomeSystemUpdate()
{
    userInterfaceUpdate();
    fireAlarmUpdate();    
    pcSerialComUpdate();
    eventLogUpdate();
    delay(SYSTEM_TIME_INCREMENT_MS);
}

//=====[Implementations of private functions]==================================
