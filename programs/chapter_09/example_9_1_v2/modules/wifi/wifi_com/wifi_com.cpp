//=====[Libraries]=============================================================

#include "mbed.h"
#include "arm_book_lib.h"

#include "wifi_credentials.h"
#include "wifi_com.h"

/*
#include "siren.h"
#include "fire_alarm.h"
#include "code.h"
#include "date_and_time.h"
#include "temperature_sensor.h"
#include "gas_sensor.h"
#include "event_log.h"
#include "sd_card.h"
*/
//=====[Declaration of private defines]========================================

// #define PC_SERIAL_COM_BAUD_RATE   115200

//=====[Declaration of private data types]=====================================

typedef enum{
    WIFI_MODULE_INIT,
    WIFI_MODULE_NOT_DETECTED,
    WIFI_MODULE_WAIT_FOR_AP_CONNECTION,
    WIFI_COMMUNICATION_INIT,
    WIFI_COMMUNICATION_UPDATE,
} wifiFsmComState_t;

//=====[Declaration and initialization of public global objects]===============

//=====[Declaration of external public global variables]=======================

//=====[Declaration and initialization of public global variables]=============

//=====[Declaration and initialization of private global variables]============

static wifiFsmComState_t wifiComFsmState;

static esp8266Status_t esp8266Status;

//=====[Declarations (prototypes) of private functions]========================

//=====[Implementations of public functions]===================================

void wifiComInit()
{
    //wifiComFsmState = WIFI_MODULE_INIT;
    
    esp8266Init();
}

void wifiComUpdate()
{
    switch ( wifiComFsmState ) {

        case WIFI_MODULE_INIT:
            // En este estado si detecta el modulo pasa al estado WIFI_MODULE_WAIT_FOR_AP_CONNECTION.
            // Si no pasa al estado: WIFI_MODULE_NOT_DETECTED            
        break;

        case WIFI_MODULE_NOT_DETECTED:
            // En este estado reintenta detectar el módulo Wi-Fi cada 1 minuto (lo resetea y luego le manda AT para chequear).
            // Cuando lo detecta pas al estado: WIFI_MODULE_WAIT_FOR_AP_CONNECTION            
        break;

        case WIFI_MODULE_WAIT_FOR_AP_CONNECTION:
            // Chequea si esta conectado y tiene IP.
            // Si está conectado y tiene IP pasa al estado: WIFI_COMMUNICATIONS_INIT
            // Si no está conectado reintenta conectarse cada 1 minuto.
            // Si no detecta el módulo vuelve a: 
            //     WIFI_MODULE_NOT_DETECTED.   
        break;

        case WIFI_COMMUNICATION_INIT:
            // En este estado:
            //     embeddedServerInit(); // Chapter 9 y 10
            //     clientInit();         // Chapter 10
            //     Y  cuando termina pasa al estado:
            //        WIFI_COMMUNICATION_UPDATE
            // En este estado levanta el Server y se pasa al siguiente estado.
            // Si no detecta el módulo vuelve a: 
            //     WIFI_MODULE_NOT_DETECTED.
            // Si se pierde la conexión entre el módulo y el AP vuelve a: 
            //     WIFI_MODULE_WAIT_FOR_AP_CONNECTION  
        break;

        case WIFI_COMMUNICATION_UPDATE:
            // embeddedServerUpdate(); // Chapter 9 y 10: FSM de Embedded Server
            // clientUpdate();         // Chapter 10: FSM de Client
            // Si no detecta el módulo vuelve a: 
            //     WIFI_MODULE_NOT_DETECTED.
            // Si se pierde la conexión entre el módulo y el AP vuelve a: 
            //     WIFI_MODULE_WAIT_FOR_AP_CONNECTION  
        break;

        default:
            wifiComFsmState = WIFI_MODULE_INIT;
        break;
    } 
}

void wifiModuleStartDetection()
{
}

bool wifiModuleIsDetected()
{
    return true;
}




//=====[Implementations of private functions]==================================
