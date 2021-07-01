//=====[Libraries]=============================================================

#include "mbed.h"

#include "ble_com.h"
#include "gate.h"
#include "event_log.h"

//=====[Declaration of private defines]========================================

//=====[Declaration of private data types]=====================================

//=====[Declaration and initialization of public global objects]===============

BufferedSerial uartBle(PD_5, PD_6, 9600);

//=====[Declaration of external public global variables]=======================

//=====[Declaration and initialization of public global variables]=============

//=====[Declaration and initialization of private global variables]============

//=====[Declarations (prototypes) of private functions]========================

static char bleComCharRead();

//=====[Implementations of public functions]===================================

void bleComUpdate()
{
    char receivedChar = bleComCharRead();
    if( receivedChar != '\0' ) {
        switch (receivedChar) {
            case 'O': gateOpen(); break;
            case 'C': gateClose(); break;
            case 'b': eventLogReport(); break;
        }
    }
}

void bleComStringWrite( const char* str )
{
    uartBle.write( str, strlen(str) );
}

//=====[Implementations of private functions]==================================

static char bleComCharRead()
{
    char receivedChar = '\0';
    if( uartBle.readable() ) {
        uartBle.read(&receivedChar,1);
    }
    return receivedChar;
}