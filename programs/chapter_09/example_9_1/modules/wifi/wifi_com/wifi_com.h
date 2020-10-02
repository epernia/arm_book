//=====[#include guards - begin]===============================================

#ifndef _WIFI_COM_H_
#define _WIFI_COM_H_

//=====[Libraries]=============================================================

//=====[Declaration of public defines]=========================================

//=====[Declaration of public data types]======================================

typedef enum{
    WIFI_MODULE_BUSY,
    WIFI_MODULE_DETECTION_STARTED,
    WIFI_MODULE_DETECTED,
    WIFI_MODULE_NOT_DETECTED,
} wifiComRequestResult_t;

//=====[Declarations (prototypes) of public functions]=========================

void wifiComInit();

// Detect module
wifiComRequestResult_t wifiModuleStartDetection();
wifiComRequestResult_t wifiModuleDetectionResponse();

//=====[#include guards - end]=================================================

#endif // _WIFI_COM_H_