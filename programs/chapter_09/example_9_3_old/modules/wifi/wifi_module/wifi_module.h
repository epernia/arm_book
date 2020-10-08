//=====[#include guards - begin]===============================================

#ifndef _WIFI_MODULE_H_
#define _WIFI_MODULE_H_

//=====[Libraries]=============================================================

//=====[Declaration of public defines]=========================================

//=====[Declaration of public data types]======================================

typedef enum{
    WIFI_MODULE_BUSY,

    WIFI_MODULE_DETECTION_STARTED,
    WIFI_MODULE_DETECTED,
    WIFI_MODULE_NOT_DETECTED,

    WIFI_MODULE_RESET_STARTED,
    WIFI_MODULE_RESET_COMPLETE,

    WIFI_MODULE_INIT_STARTED,
    WIFI_MODULE_INIT_COMPLETE,

    WIFI_MODULE_IS_CONNECTED_AP_STARTED,
    WIFI_MODULE_IS_CONNECTED,
    WIFI_MODULE_IS_NOT_CONNECTED,

    WIFI_MODULE_CONNECT_AP_STARTED,
    WIFI_MODULE_CONNECT_AP_ERR_TIMEOUT,
    WIFI_MODULE_CONNECT_AP_ERR_WRONG_PASS,
    WIFI_MODULE_CONNECT_AP_ERR_AP_NOT_FOUND,
    WIFI_MODULE_CONNECT_AP_ERR_CONN_FAIL,

    WIFI_MODULE_IP_GET_STARTED,
    WIFI_MODULE_IP_GET_COMPLETE,

    WIFI_MODULE_CREATE_TCP_SERVER_STARTED,
    WIFI_MODULE_CREATE_TCP_SERVER_COMPLETE,
    WIFI_MODULE_CREATE_TCP_SERVER_ERR,

    WIFI_MODULE_DELETE_TCP_SERVER_STARTED,
    WIFI_MODULE_DELETE_TCP_SERVER_COMPLETE,

    WIFI_MODULE_CLIENT_PENDIG_REQUEST_STARTED,
    WIFI_MODULE_CLIENT_REQUEST_PENDING,
    WIFI_MODULE_NO_CLIENT_REQUEST_PENDING,

    WIFI_MODULE_RESPOND_TO_CLIENT_STARTED,
    WIFI_MODULE_RESPOND_TO_CLIENT_COMPLETE,
} wifiModuleRequestResult_t;

/*
typedef enum{
    WIFI_MODULE_CLIENT_REQ_INDEX,
} wifiComClientRequest_t;
*/

//=====[Declarations (prototypes) of public functions]=========================

// Update module status -------------------------------------------------------

void wifiModuleUpdate();

// Detect module --------------------------------------------------------------

wifiModuleRequestResult_t wifiModuleStartDetection();
// Responses:
// WIFI_MODULE_DETECTION_STARTED
// WIFI_MODULE_BUSY

wifiModuleRequestResult_t wifiModuleDetectionResponse();
// Responses:
// WIFI_MODULE_DETECTED
// WIFI_MODULE_NOT_DETECTED;
// WIFI_MODULE_BUSY

// Reset module ---------------------------------------------------------------

wifiModuleRequestResult_t wifiModuleStartReset();
// Responses:
// WIFI_MODULE_RESET_STARTED
// WIFI_MODULE_BUSY

wifiModuleRequestResult_t wifiModuleResetResponse();
// Responses:
// WIFI_MODULE_RESET_COMPLETE
// WIFI_MODULE_NOT_DETECTED
// WIFI_MODULE_BUSY

// Initialize module ----------------------------------------------------------

wifiModuleRequestResult_t wifiModuleStartInit();
// Responses:
// WIFI_MODULE_INIT_STARTED
// WIFI_MODULE_BUSY

wifiModuleRequestResult_t wifiModuleInitResponse();
// Responses:
// WIFI_MODULE_INIT_COMPLETE
// WIFI_MODULE_NOT_DETECTED
// WIFI_MODULE_BUSY

// AP connection --------------------------------------------------------------

// Check if connected with AP

wifiModuleRequestResult_t wifiModuleStartIsConnectedWithAP();
// Responses:
// WIFI_MODULE_IS_CONNECTED_AP_STARTED
// WIFI_MODULE_BUSY

wifiModuleRequestResult_t wifiModuleIsConnectedWithAPResponse( char* ip );
// Responses:
// WIFI_MODULE_IS_CONNECTED (and fill ip variable)
// WIFI_MODULE_IS_NOT_CONNECTED
// WIFI_MODULE_NOT_DETECTED
// WIFI_MODULE_BUSY

// Connect with AP

wifiModuleRequestResult_t wifiModuleStartConnectWithAP(
    char const* ssid, char const* password );
// Responses:
// WIFI_MODULE_CONNECT_AP_STARTED
// WIFI_MODULE_BUSY

wifiModuleRequestResult_t wifiModuleConnectWithAPResponse( char* ip );
// Responses:
// WIFI_MODULE_IS_CONNECTED (and fill ip variable)
// WIFI_MODULE_CONNECT_AP_ERR_TIMEOUT
// WIFI_MODULE_CONNECT_AP_ERR_WRONG_PASS
// WIFI_MODULE_CONNECT_AP_ERR_AP_NOT_FOUND
// WIFI_MODULE_CONNECT_AP_ERR_CONN_FAIL
// WIFI_MODULE_NOT_DETECTED
// WIFI_MODULE_BUSY

// TCP Server -----------------------------------------------------------------

// Create a TCP Server --------------

wifiModuleRequestResult_t wifiModuleStartCreateTCPServerAtPort( int port );
// Responses:
// WIFI_MODULE_CREATE_TCP_SERVER_STARTED
// WIFI_MODULE_BUSY

wifiModuleRequestResult_t wifiModuleCreateTCPServerAtPortResponse();
// Responses:
// WIFI_MODULE_CREATE_TCP_SERVER_COMPLETE
// WIFI_MODULE_CREATE_TCP_SERVER_ERR
// WIFI_MODULE_NOT_DETECTED
// WIFI_MODULE_BUSY

// Delete TCP Server --------------

wifiModuleRequestResult_t wifiModuleStartDeleteTCPServer();
// Responses:
// WIFI_MODULE_DELETE_TCP_SERVER_STARTED
// WIFI_MODULE_BUSY

wifiModuleRequestResult_t wifiModuleDeleteTCPServerResponse();
// Responses:
// WIFI_MODULE_DELETE_TCP_SERVER_COMPLETE
// WIFI_MODULE_NOT_DETECTED
// WIFI_MODULE_BUSY

// Check if are a client pending request --------------
// Note: PC should also connect to the same Access Point.

wifiModuleRequestResult_t wifiModuleStartIsAClientRequestPending();
// Responses:
// WIFI_MODULE_CLIENT_PENDIG_REQUEST_STARTED
// WIFI_MODULE_BUSY

wifiModuleRequestResult_t wifiModuleIsAClientRequestPendingResponse( 
    int* linkID );//, 
//    wifiComClientRequest_t* clientRequest );
// Responses:
// WIFI_MODULE_CLIENT_REQUEST_PENDING
// WIFI_MODULE_NO_CLIENT_REQUEST_PENDING
// WIFI_MODULE_NOT_DETECTED
// WIFI_MODULE_BUSY

// Response to a client request --------------

wifiModuleRequestResult_t wifiModuleStartRespondToClientRequest( 
    int linkID, 
    char const* response );
// Responses:
// WIFI_MODULE_RESPOND_TO_CLIENT_STARTED
// WIFI_MODULE_BUSY

wifiModuleRequestResult_t wifiModuleRespondToClientRequestResponse();
// Responses:
// WIFI_MODULE_RESPOND_TO_CLIENT_COMPLETE
// WIFI_MODULE_NOT_DETECTED
// WIFI_MODULE_BUSY

//=====[#include guards - end]=================================================

#endif // _WIFI_COM_H_