//=====[Libraries]=============================================================

#include "mbed.h"

#include "smartphone_ble_communication.h"

//=====[Declaration of private constants]======================================

//=====[Declaration of private data types]=====================================

//=====[Declaration of external public global objects]=========================

//=====[Declaration and intitalization of public global objects]===============

//=====[Declaration and intitalization of private global objects]==============

Serial uartBle(D1, D0);

//=====[Declaration of external public global variables]=======================

//=====[Declaration and intitalization of public global variables]=============

//=====[Declaration and intitalization of private global variables]============

//=====[Declarations (prototypes) of private functions]========================

//=====[Implementations of public functions]===================================

void smarphoneBleCommunicationWrite( const char* str )
{
    uartBle.printf( "%s", str );
}

//=====[Implementations of private functions]==================================

