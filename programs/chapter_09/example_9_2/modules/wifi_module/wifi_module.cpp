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

//=====[Implementations of public functions]===================================

void wifiModuleInit()
{  
    uartEsp8266.baud(ESP8266_BAUD_RATE);
    esp8266State = ESP8266_IDLE;
}

wifiModuleRequestResult_t wifiModuleSetAP_SSID( char const* ssid )
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

wifiModuleRequestResult_t wifiModuleSetAP_Password( char const* password )
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

wifiModuleRequestResult_t wifiModuleStartDetection()
{
    if( esp8266State == ESP8266_IDLE ){
        parserInit( &parser, "OK\r\n", strlen("OK\r\n"),
                    ESP8266_MOST_COMMON_AT_CMD_TIMEOUT );
        esp8266State = ESP8266_PROCESSING_AT_COMMAND;
        esp8266UartStringWrite( "AT\r\n" );
        return WIFI_MODULE_DETECTION_STARTED;
    } else {        
        return WIFI_MODULE_BUSY;   
    }
}

wifiModuleRequestResult_t wifiModuleDetectionResponse()
{
    char receivedChar = '\0';
    esp8266UartByteRead( &receivedChar );
    
    parserStatus = parserPatternMatchOrTimeout( &parser, receivedChar );

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
