//=====[#include guards - begin]===============================================

#ifndef _WIFI_MODULE_H_
#define _WIFI_MODULE_H_

//=====[Libraries]=============================================================

//=====[Declaration of public defines]=========================================

#define WIFI_MODULE_CREDENTIAL_MAX_LEN   101

//=====[Declaration of public data types]======================================

typedef enum{
    WIFI_MODULE_AP_SSID_SAVED,
    WIFI_MODULE_AP_SSID_NOT_SAVED,
    WIFI_MODULE_AP_PASSWORD_SAVED,
    WIFI_MODULE_AP_PASSWORD_NOT_SAVED,
    WIFI_MODULE_BUSY,
    WIFI_MODULE_DETECTION_STARTED,
    WIFI_MODULE_DETECTED,
    WIFI_MODULE_NOT_DETECTED,
} wifiComRequestResult_t;

//=====[Declarations (prototypes) of public functions]=========================

void wifiComInit();

wifiComRequestResult_t wifiModuleSetAP_SSID( char const* ssid );
wifiComRequestResult_t wifiModuleSetAP_Password( char const* password );
char const* wifiModuleGetAP_SSID();
char const* wifiModuleGetAP_Password();

// Detect module
wifiComRequestResult_t wifiModuleStartDetection();
wifiComRequestResult_t wifiModuleDetectionResponse();

//=====[#include guards - end]=================================================

#endif // _WIFI_MODULE_H_
