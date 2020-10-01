//=====[#include guards - begin]===============================================

#ifndef _WIFI_COM_H_
#define _WIFI_COM_H_

//=====[Libraries]=============================================================

//=====[Declaration of public defines]=========================================

//=====[Declaration of public data types]======================================

typedef enum{
    WIFI_MODULE_INIT,
    WIFI_MODULE_BUSY,
    WIFI_MODULE_DETECTION_STARTED,
    WIFI_MODULE_DETECTED,
    WIFI_MODULE_NOT_DETECTED,
    WIFI_MODULE_RESET_STARTED,
    WIFI_MODULE_RESET_COMPLETE,
    WIFI_MODULE_WAIT_FOR_AP_CONNECTION,
    WIFI_COMMUNICATION_INIT,
    WIFI_COMMUNICATION_UPDATE,
} wifiComStatus_t;

//=====[Declarations (prototypes) of public functions]=========================

void wifiComInit();
void wifiComUpdate();

// Detect module
wifiComStatus_t wifiModuleStartDetection();
wifiComStatus_t wifiModuleDetectionResponse();

// Reset module
wifiComStatus_t wifiModuleStartReset();
wifiComStatus_t wifiModuleResetResponse();

//=====[#include guards - end]=================================================

#endif // _WIFI_COM_H_