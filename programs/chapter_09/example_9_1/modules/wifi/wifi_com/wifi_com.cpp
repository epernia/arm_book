//=====[Libraries]=============================================================

#include "wifi_com.h"
#include "esp8266_at.h"

//=====[Declaration of private defines]========================================

// #define PC_SERIAL_COM_BAUD_RATE   115200

//=====[Declaration of private data types]=====================================

//=====[Declaration and initialization of public global objects]===============

//=====[Declaration of external public global variables]=======================

//=====[Declaration and initialization of public global variables]=============

//=====[Declaration and initialization of private global variables]============

//=====[Declarations (prototypes) of private functions]========================

//=====[Implementations of public functions]===================================

void wifiComInit()
{  
    esp8266Init();
}

// Detect module

wifiComStatus_t wifiModuleStartDetection()
{
    switch( esp8266TestATSend() ) {
        case ESP8266_AT_SENT: return WIFI_MODULE_DETECTION_STARTED;
        case ESP8266_AT_NOT_SENT:
        default: return WIFI_MODULE_BUSY;
    }
}

wifiComStatus_t wifiModuleDetectionResponse()
{
    switch( esp8266TestATResponse() ) {
        case ESP8266_AT_RESPONDED: return WIFI_MODULE_DETECTED;
        case ESP8266_AT_TIMEOUT: return WIFI_MODULE_NOT_DETECTED;
        default: return WIFI_MODULE_BUSY;
    }
}

//=====[Implementations of private functions]==================================
