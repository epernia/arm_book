//=====[#include guards - begin]===============================================

#ifndef _WIFI_MODULE_H_
#define _WIFI_MODULE_H_

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
    WIFI_MODULE_ERR,
    WIFI_MODULE_WAIT_FOR_AP_CONNECTION,
    WIFI_COMMUNICATION_INIT,
    WIFI_COMMUNICATION_UPDATE,
} wifiComRequestResult_t;

//=====[Declarations (prototypes) of public functions]=========================

// Detect module
wifiComRequestResult_t wifiModuleStartDetection();
wifiComRequestResult_t wifiModuleDetectionResponse();

// Reset module
wifiComRequestResult_t wifiModuleStartReset();
wifiComRequestResult_t wifiModuleResetResponse();

//=====[#include guards - end]=================================================

#endif // _WIFI_COM_H_