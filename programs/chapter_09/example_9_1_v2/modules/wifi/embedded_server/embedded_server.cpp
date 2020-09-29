//=====[Libraries]=============================================================

#include "mbed.h"
#include "arm_book_lib.h"

#include "embedded_server.h"

//=====[Declaration of private defines]========================================

//=====[Declaration of private data types]=====================================

typedef enum{
    EMBEDDED_SERVER_DISABLE,
    EMBEDDED_SERVER_INIT,
    EMBEDDED_SERVER_UPDATE,
} wifiFsmComState_t;

//=====[Declaration and initialization of public global objects]===============

//=====[Declaration of external public global variables]=======================

//=====[Declaration and initialization of public global variables]=============

//=====[Declaration and initialization of private global variables]============

//=====[Declarations (prototypes) of private functions]========================

//=====[Implementations of public functions]===================================

void embeddedServerInit()
{
    
}

void embeddedServerUpdate()
{
    switch ( embeddedServerState ) {

        case EMBEDDED_SERVER_DISABLE:
            // Se queda en este estado hasta que la FSM superior WIFI la active (en embeddedServerInit()).         
        break;

        case EMBEDDED_SERVER_INIT:
            // Inicializa el servidor y pasa al estado:
            //     EMBEDDED_SERVER_UPDATE
            // En caso de errores: no conectado al AP, o modulo no detectado; pasa al 
            // estado: EMBEDDED_SERVER_DISABLE. Y retorna el error para que lo maneje 
            // la FSM superior WIFI          
        break;

        case EMBEDDED_SERVER_UPDATE: // In this state:
            //Chequea si hay peticiones desde los clients y las responde con la web.            
            //En caso de errores: no conectado al AP, o modulo no detectado; pasa al 
            //estado: EMBEDDED_SERVER_DISABLE. Y retorna el error para que lo maneje
            //la FSM superior WIFI
        break;

        default:
            embeddedServerState = EMBEDDED_SERVER_DISABLE;
        break;
    } 
}

//=====[Implementations of private functions]==================================
