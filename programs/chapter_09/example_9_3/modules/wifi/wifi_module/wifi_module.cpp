//=====[Libraries]=============================================================

#include "wifi_module.h"
#include "sapi.h"

//=====[Declaration of private defines]========================================

#define ESP8266_BAUD_RATE                    115200
#define ESP8266_MOST_COMMON_AT_CMD_TIMEOUT   50

//=====[Declaration of private data types]=====================================

typedef enum{   
    ESP8266_IDLE,
    ESP8266_PROCESSING_AT_COMMAND,
}esp8266State_t;

//=====[Declaration and initialization of public global objects]===============

static Serial uartEsp8266( D42, D41 );

//=====[Declaration of external public global variables]=======================

//=====[Declaration and initialization of public global variables]=============

//=====[Declaration and initialization of private global variables]============

static char credential_ssid[WIFI_MODULE_CREDENTIAL_MAX_LEN] = ""; 
static char credential_password[WIFI_MODULE_CREDENTIAL_MAX_LEN] = "";

static parser_t parser;
static parserStatus_t parserStatus;

static esp8266State_t esp8266State;

//=====[Declarations (prototypes) of private functions]========================

static bool esp8266UartByteRead( char* receivedByte );
static void esp8266UartByteWrite( char byteToSend );
static void esp8266UartStringWrite( char const* str );

// Send commands that response "OK\r\n"
static wifiComRequestResult_t esp8266SendCommandWithOkResponse( char const* cmd );
// Check response for previously sended commands that only response "OK\r\n"
static wifiComRequestResult_t esp8266CheckOkResponse();

//=====[Implementations of public functions]===================================

// Init module and status -----------------------------------------------------

void wifiModuleInit()
{  
    uartEsp8266.baud(ESP8266_BAUD_RATE);
    esp8266State = ESP8266_IDLE;
}

// Update module status -------------------------------------------------------

void wifiModuleUpdate() {
    esp8266Update();
}

// Set/Get AP credentials -----------------------------------------------------

// Responses:
// WIFI_MODULE_AP_SSID_SAVED
// WIFI_MODULE_AP_SSID_NOT_SAVED
wifiComRequestResult_t wifiModuleSetAP_SSID( char const* ssid )
{
    if( *ssid == '\0' ) {
        return WIFI_MODULE_AP_SSID_NOT_SAVED;
    }
    int i=0;
    while ( *ssid != '\0' && i<WIFI_MODULE_CREDENTIAL_MAX_LEN ) {
        credential_ssid[i] = *ssid;
        ssid++;
        i++;
    }
    if( i >= WIFI_MODULE_CREDENTIAL_MAX_LEN ) {
        return WIFI_MODULE_AP_SSID_NOT_SAVED;
    }
    return WIFI_MODULE_AP_SSID_SAVED;
}

// Responses:
// WIFI_MODULE_AP_PASSWORD_SAVED
// WIFI_MODULE_AP_PASSWORD_NOT_SAVED
wifiComRequestResult_t wifiModuleSetAP_Password( char const* password )
{
    if( *password == '\0' ) {
        return WIFI_MODULE_AP_PASSWORD_NOT_SAVED;
    }
    int i=0;
    while ( *password != '\0' && i<WIFI_MODULE_CREDENTIAL_MAX_LEN ) {
        credential_password[i] = *password;
        password++;
        i++;
    }
    if( i >= WIFI_MODULE_CREDENTIAL_MAX_LEN ) {
        return WIFI_MODULE_AP_PASSWORD_NOT_SAVED;
    }
    return WIFI_MODULE_AP_PASSWORD_SAVED;
}

char const* wifiModuleGetAP_SSID()
{
    return (char const*) credential_ssid;
}

char const* wifiModuleGetAP_Password()
{
    return (char const*) credential_password;
}

// Detect module --------------------------------------------------------------

// Responses:
// WIFI_MODULE_DETECTION_STARTED
// WIFI_MODULE_BUSY
wifiComRequestResult_t wifiModuleStartDetection()
{
    return esp8266SendCommandWithOkResponse( "AT\r\n" );
}

// Responses:
// WIFI_MODULE_DETECTED
// WIFI_MODULE_NOT_DETECTED
// WIFI_MODULE_BUSY
wifiComRequestResult_t wifiModuleDetectionResponse()
{
    return esp8266CheckOkResponse();
}

// Reset module ---------------------------------------------------------------

// Responses:
// WIFI_MODULE_RESET_STARTED
// WIFI_MODULE_BUSY
wifiModuleRequestResult_t wifiModuleStartReset()
{
    // TODO: @Eric Falta implementar!
    switch( esp8266ResetSend() ) {
        case ESP8266_AT_SENT: return WIFI_MODULE_RESET_STARTED;
        case ESP8266_AT_NOT_SENT:
        default: return WIFI_MODULE_BUSY;
    }
}

// Responses:
// WIFI_MODULE_RESET_COMPLETE
// WIFI_MODULE_NOT_DETECTED
// WIFI_MODULE_BUSY
wifiModuleRequestResult_t wifiModuleResetResponse()
{
    // TODO: @Eric Falta implementar!
    switch( esp8266ResetResponse() ) {
        case ESP8266_AT_RESPONDED: return WIFI_MODULE_RESET_COMPLETE;
        case ESP8266_AT_TIMEOUT: return WIFI_MODULE_NOT_DETECTED;
        default: return WIFI_MODULE_BUSY;
    }
}

// Initialize module ----------------------------------------------------------

// Responses:
// WIFI_MODULE_INIT_STARTED
// WIFI_MODULE_BUSY
wifiModuleRequestResult_t wifiModuleStartInit()
{
    // TODO: @Eric Falta implementar!
    return WIFI_MODULE_INIT_STARTED;
    switch( esp8266WiFiModeSetSend(ESP8266_WIFI_MODE_STATION) ) {
        case ESP8266_AT_SENT: return WIFI_MODULE_INIT_STARTED;
        case ESP8266_AT_NOT_SENT:
        default: return WIFI_MODULE_BUSY;
    }
}

// Responses:
// WIFI_MODULE_INIT_COMPLETE
// WIFI_MODULE_NOT_DETECTED
// WIFI_MODULE_BUSY
wifiModuleRequestResult_t wifiModuleInitResponse()
{
    // TODO: @Eric Falta implementar!
    switch( esp8266WiFiModeSetResponse() ) {
        case ESP8266_AT_RESPONDED: return WIFI_MODULE_INIT_COMPLETE;
        case ESP8266_AT_TIMEOUT: return WIFI_MODULE_NOT_DETECTED;
        default: return WIFI_MODULE_BUSY;
    }
}

// AP connection --------------------------------------------------------------

// Check if connected with AP

// Responses:
// WIFI_MODULE_IS_CONNECTED_AP_STARTED
// WIFI_MODULE_BUSY
wifiModuleRequestResult_t wifiModuleStartIsConnectedWithAP()
{
    // TODO: @Eric Falta implementar!
    switch( esp8266LocalIPAddressGetSend() ) {
        case ESP8266_AT_SENT: return WIFI_MODULE_IS_CONNECTED_AP_STARTED;
        case ESP8266_AT_NOT_SENT:
        default: return WIFI_MODULE_BUSY;
    }
}

// Responses:
// WIFI_MODULE_IS_CONNECTED (and fill ip variable)
// WIFI_MODULE_IS_NOT_CONNECTED
// WIFI_MODULE_NOT_DETECTED
// WIFI_MODULE_BUSY
wifiModuleRequestResult_t wifiModuleIsConnectedWithAPResponse( char* ip )
{
    esp8266ConnectionStatus_t connectionStatus;
    
    
    // TODO: @Eric Falta implementar!

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

// Connect with AP

// Responses:
// WIFI_MODULE_CONNECT_AP_STARTED
// WIFI_MODULE_BUSY
wifiModuleRequestResult_t wifiModuleStartConnectWithAP()
{
    // TODO: @Eric Falta implementar!
    return WIFI_MODULE_CONNECT_AP_STARTED;
}

// Responses:
// WIFI_MODULE_IS_CONNECTED (and fill ip variable)
// WIFI_MODULE_CONNECT_AP_ERR_TIMEOUT
// WIFI_MODULE_CONNECT_AP_ERR_WRONG_PASS
// WIFI_MODULE_CONNECT_AP_ERR_AP_NOT_FOUND
// WIFI_MODULE_CONNECT_AP_ERR_CONN_FAIL
// WIFI_MODULE_NOT_DETECTED
// WIFI_MODULE_BUSY
wifiModuleRequestResult_t wifiModuleConnectWithAPResponse( char* ip )
{
    // TODO: @Eric Falta implementar!
    strcat( ip, "192.168.101.101" );
    return WIFI_MODULE_IS_CONNECTED;
}

//=====[Implementations of private functions]==================================

static bool esp8266UartByteRead( char* receivedByte )
{
    if( uartEsp8266.readable() ) {
        *receivedByte = uartEsp8266.getc();
        return true;
    }
    return false;
}

static void esp8266UartByteWrite( char byteToSend )
{
    uartEsp8266.putc( byteToSend );
}

static void esp8266UartStringWrite( char const* str )
{
    while ( *str != NULL ) {
        esp8266UartByteWrite( (uint8_t)*str );
        str++;
    }
}

// Send commands that response "OK\r\n"
static wifiComRequestResult_t esp8266SendCommandWithOkResponse( char const* cmd )
{
    if( esp8266State == ESP8266_IDLE ){
        parserInit( &parser, "OK\r\n", strlen("OK\r\n"),
                    ESP8266_MOST_COMMON_AT_CMD_TIMEOUT );
        esp8266State = ESP8266_PROCESSING_AT_COMMAND;
        esp8266UartStringWrite( cmd );
        return WIFI_MODULE_DETECTION_STARTED;
    } else {        
        return WIFI_MODULE_BUSY;   
    }
}

// Check response for previously sended commands that only response "OK\r\n"
static wifiComRequestResult_t esp8266CheckOkResponse()
{
    char receivedChar = '\0';
    esp8266UartByteRead( &receivedChar );    
    parserStatus = parserUpdate( &parser, receivedChar );
    switch( parserStatus ) {
        case PARSER_TIMEOUT:
            esp8266State = ESP8266_IDLE;
            return WIFI_MODULE_NOT_DETECTED;
        break;
        case PARSER_PATTERN_MATCH:
            esp8266State = ESP8266_IDLE;
            return WIFI_MODULE_DETECTED;
        break;
        default:
            return WIFI_MODULE_BUSY;
        break;
    }
}
