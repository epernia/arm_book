//=====[Libraries]=============================================================

#include "wifi_module.h"

#include "sapi.h"

//=====[Declaration of private defines]========================================

#define ESP8266_BAUD_RATE                    115200
#define ESP8266_MOST_COMMON_AT_CMD_TIMEOUT   50

//=====[Declaration of private data types]=====================================

//=====[Declaration and initialization of public global objects]===============

//=====[Declaration of external public global variables]=======================

//=====[Declaration and initialization of public global variables]=============

//=====[Declaration and initialization of private global variables]============

//=====[Declarations (prototypes) of private functions]========================

//=====[Implementations of public functions]===================================

// Update module status -------------------------------------------------------

void wifiModuleUpdate() {
    esp8266Update();
}

// Detect module --------------------------------------------------------------

wifiModuleRequestResult_t wifiModuleStartDetection()
{
    switch( esp8266TestATSend() ) {
        case ESP8266_AT_SENT: return WIFI_MODULE_DETECTION_STARTED;
        case ESP8266_AT_NOT_SENT:
        default: return WIFI_MODULE_BUSY;
    }
}
// Responses:
// WIFI_MODULE_DETECTION_STARTED
// WIFI_MODULE_BUSY

wifiModuleRequestResult_t wifiModuleDetectionResponse()
{
    switch( esp8266TestATResponse() ) {
        case ESP8266_AT_RESPONDED: return WIFI_MODULE_DETECTED;
        case ESP8266_AT_TIMEOUT: return WIFI_MODULE_NOT_DETECTED;
        default: return WIFI_MODULE_BUSY;
    }
}
// Responses:
// WIFI_MODULE_DETECTED
// WIFI_MODULE_NOT_DETECTED
// WIFI_MODULE_BUSY

// Reset module ---------------------------------------------------------------

wifiModuleRequestResult_t wifiModuleStartReset()
{
    switch( esp8266ResetSend() ) {
        case ESP8266_AT_SENT: return WIFI_MODULE_RESET_STARTED;
        case ESP8266_AT_NOT_SENT:
        default: return WIFI_MODULE_BUSY;
    }
}
// Responses:
// WIFI_MODULE_RESET_STARTED
// WIFI_MODULE_BUSY

wifiModuleRequestResult_t wifiModuleResetResponse()
{
    switch( esp8266ResetResponse() ) {
        case ESP8266_AT_RESPONDED: return WIFI_MODULE_RESET_COMPLETE;
        case ESP8266_AT_TIMEOUT: return WIFI_MODULE_NOT_DETECTED;
        default: return WIFI_MODULE_BUSY;
    }
}
// Responses:
// WIFI_MODULE_RESET_COMPLETE
// WIFI_MODULE_NOT_DETECTED
// WIFI_MODULE_BUSY

// Initialize module ----------------------------------------------------------

wifiModuleRequestResult_t wifiModuleStartInit()
{
    return WIFI_MODULE_INIT_STARTED;
    switch( esp8266WiFiModeSetSend(ESP8266_WIFI_MODE_STATION) ) {
        case ESP8266_AT_SENT: return WIFI_MODULE_INIT_STARTED;
        case ESP8266_AT_NOT_SENT:
        default: return WIFI_MODULE_BUSY;
    }
}
// Responses:
// WIFI_MODULE_INIT_STARTED
// WIFI_MODULE_BUSY

wifiModuleRequestResult_t wifiModuleInitResponse()
{
    switch( esp8266WiFiModeSetResponse() ) {
        case ESP8266_AT_RESPONDED: return WIFI_MODULE_INIT_COMPLETE;
        case ESP8266_AT_TIMEOUT: return WIFI_MODULE_NOT_DETECTED;
        default: return WIFI_MODULE_BUSY;
    }
}
// Responses:
// WIFI_MODULE_INIT_COMPLETE
// WIFI_MODULE_NOT_DETECTED
// WIFI_MODULE_BUSY

// AP connection --------------------------------------------------------------

// Check if connected with AP

wifiModuleRequestResult_t wifiModuleStartIsConnectedWithAP()
{
    switch( esp8266LocalIPAddressGetSend() ) {
        case ESP8266_AT_SENT: return WIFI_MODULE_IS_CONNECTED_AP_STARTED;
        case ESP8266_AT_NOT_SENT:
        default: return WIFI_MODULE_BUSY;
    }
}
// Responses:
// WIFI_MODULE_IS_CONNECTED_AP_STARTED
// WIFI_MODULE_BUSY

wifiModuleRequestResult_t wifiModuleIsConnectedWithAPResponse( char* ip )
{
    esp8266ConnectionStatus_t connectionStatus;
    
    

    switch( esp8266LocalIPAddressGetResponse(ip, mac) ) {
        case ESP8266_AT_RESPONDED:
            if( strcmp(ip, "0.0.0.0") ) 
                return WIFI_MODULE_IS_NOT_CONNECTED;
            } else {
                return WIFI_MODULE_IS_CONNECTED;
            }
        case ESP8266_AT_TIMEOUT: return WIFI_MODULE_NOT_DETECTED;
        default: return WIFI_MODULE_BUSY;
    }
    return WIFI_MODULE_IS_CONNECTED;
}
// Responses:
// WIFI_MODULE_IS_CONNECTED (and fill ip variable)
// WIFI_MODULE_IS_NOT_CONNECTED
// WIFI_MODULE_NOT_DETECTED
// WIFI_MODULE_BUSY

// Connect with AP

wifiModuleRequestResult_t wifiModuleStartConnectWithAP(
    char const* ssid, char const* password )
{
    return WIFI_MODULE_CONNECT_AP_STARTED;
}
// Responses:
// WIFI_MODULE_CONNECT_AP_STARTED
// WIFI_MODULE_BUSY

wifiModuleRequestResult_t wifiModuleConnectWithAPResponse( char* ip )
{
    strcat( ip, "192.168.101.101" );
    return WIFI_MODULE_IS_CONNECTED;
}
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

wifiModuleRequestResult_t wifiModuleStartCreateTCPServerAtPort( int port )
{
    return WIFI_MODULE_CREATE_TCP_SERVER_STARTED;
}
// Responses:
// WIFI_MODULE_CREATE_TCP_SERVER_STARTED
// WIFI_MODULE_BUSY

wifiModuleRequestResult_t wifiModuleCreateTCPServerAtPortResponse()
{
    return WIFI_MODULE_CREATE_TCP_SERVER_COMPLETE;
}
// Responses:
// WIFI_MODULE_CREATE_TCP_SERVER_COMPLETE
// WIFI_MODULE_CREATE_TCP_SERVER_ERR
// WIFI_MODULE_NOT_DETECTED
// WIFI_MODULE_BUSY

// Delete TCP Server --------------

wifiModuleRequestResult_t wifiModuleStartDeleteTCPServer()
{
    return WIFI_MODULE_DELETE_TCP_SERVER_STARTED;
}
// Responses:
// WIFI_MODULE_DELETE_TCP_SERVER_STARTED
// WIFI_MODULE_BUSY

wifiModuleRequestResult_t wifiModuleDeleteTCPServerResponse()
{
    return WIFI_MODULE_DELETE_TCP_SERVER_COMPLETE;
}
// Responses:
// WIFI_MODULE_DELETE_TCP_SERVER_COMPLETE
// WIFI_MODULE_NOT_DETECTED
// WIFI_MODULE_BUSY

// Check if are a client pending request --------------
// Note: PC should also connect to the same Access Point.

wifiModuleRequestResult_t wifiModuleStartIsAClientRequestPending()
{
    return WIFI_MODULE_CLIENT_PENDIG_REQUEST_STARTED;
}
// Responses:
// WIFI_MODULE_CLIENT_PENDIG_REQUEST_STARTED
// WIFI_MODULE_BUSY

wifiModuleRequestResult_t wifiModuleIsAClientRequestPendingResponse( 
    int* linkID ) //, 
//    wifiComClientRequest_t* clientRequest )
{
    return WIFI_MODULE_NO_CLIENT_REQUEST_PENDING;
}
// Responses:
// WIFI_MODULE_CLIENT_REQUEST_PENDING
// WIFI_MODULE_NO_CLIENT_REQUEST_PENDING
// WIFI_MODULE_NOT_DETECTED
// WIFI_MODULE_BUSY

// Response to a client request --------------

wifiModuleRequestResult_t wifiModuleStartRespondToClientRequest( 
    int linkID, 
    char const* response )
{
    return WIFI_MODULE_RESPOND_TO_CLIENT_STARTED;
}
// Responses:
// WIFI_MODULE_RESPOND_TO_CLIENT_STARTED
// WIFI_MODULE_BUSY

wifiModuleRequestResult_t wifiModuleRespondToClientRequestResponse()
{
    return WIFI_MODULE_RESPOND_TO_CLIENT_COMPLETE;
}
// Responses:
// WIFI_MODULE_RESPOND_TO_CLIENT_COMPLETE
// WIFI_MODULE_NOT_DETECTED
// WIFI_MODULE_BUSY

//=====[Implementations of private functions]==================================
