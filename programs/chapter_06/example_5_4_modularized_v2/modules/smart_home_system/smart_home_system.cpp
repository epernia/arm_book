//=====[Libraries]=============================================================

#include "arm_book_lib.h"

#include "smart_home_system.h"

#include "alarm.h"
#include "matrix_keypad.h"
#include "date_and_time.h"
#include "pc_serial_communication.h"
#include "smartphone_ble_communication.h"
#include "event_log.h"

//=====[Declaration of private constants]======================================

//=====[Declaration of private data types]=====================================

//=====[Declaration and intitalization of public global objects]===============

//=====[Declaration and intitalization of private global variables]============

//=====[Declarations (prototypes) of private functions]========================

//=====[Implementations of public functions]===================================

void smartHomeSystemInit()
{
    alarmInit();
    matrixKeypadInit();
}

void smartHomeSystemUpdate()
{
    alarmUpdate();
    pcSerialCommunicationCommandUpdate();
    eventLogUpdate();
    delay(SYSTEM_TIME_INCREMENT_MS);
}

//=====[Implementations of private functions]==================================
