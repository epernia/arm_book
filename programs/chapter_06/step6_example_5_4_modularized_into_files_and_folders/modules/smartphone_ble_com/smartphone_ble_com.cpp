//=====[Libraries]=============================================================

#include "mbed.h"

#include "smartphone_ble_com.h"

//=====[Declaration of private constants]======================================

//=====[Declaration of private data types]=====================================

//=====[Declaration and initialization of public global objects]===============

Serial uartBle(D1, D0);

//=====[Declaration of external public global variables]=======================

//=====[Declaration and initialization of public global variables]=============

//=====[Declaration and initialization of private global variables]============

//=====[Declarations (prototypes) of private functions]========================

//=====[Implementations of public functions]===================================

void smartphoneBleComWrite( const char* str )
{
    uartBle.printf( "%s", str );
}

//=====[Implementations of private functions]==================================

