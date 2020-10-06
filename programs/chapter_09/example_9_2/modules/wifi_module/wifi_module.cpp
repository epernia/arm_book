//=====[Libraries]=============================================================

#include "wifi_module.h"
#include "sapi.h"

//=====[Declaration of private defines]========================================

#define ESP8266_BAUD_RATE   115200

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

static bool flagStartParser = true;
static parser_t parser;
static parserStatus_t parserStatus;

static esp8266State_t esp8266State;

//=====[Declarations (prototypes) of private functions]========================

static bool esp8266UartByteRead( char* receivedByte );
static void esp8266UartByteWrite( char byteToSend );
static void esp8266UartStringWrite( char const* str );

//=====[Implementations of public functions]===================================

void wifiComInit()
{  
    uartEsp8266.baud(ESP8266_BAUD_RATE);
    esp8266State = ESP8266_IDLE;
}

wifiComRequestResult_t wifiModuleStartDetection()
{    
    if( esp8266State == ESP8266_IDLE ){
        parserInit( &parser, "OK\r\n", strlen("OK\r\n"), 50 );
        esp8266State = ESP8266_PROCESSING_AT_COMMAND;
        esp8266UartStringWrite( "AT\r\n" );
        return WIFI_MODULE_DETECTION_STARTED;
    } else {        
        return WIFI_MODULE_BUSY;   
    }
}

wifiComRequestResult_t wifiModuleDetectionResponse()
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
