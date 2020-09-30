//=====[Libraries]=============================================================

#include "mbed.h"
#include "arm_book_lib.h"

#include "esp8266_at.h"

#include "sapi.h"

//=====[Declaration of private defines]========================================

//=====[Declaration of private data types]=====================================

//=====[Declaration and initialization of public global objects]===============

//static Serial uartEsp8266( USBTX, USBRX );

// D42 = PE_8 = UART7_TX
// D41 = PE_7 = UART7_RX
static Serial uartEsp8266( D42, D41 );

//=====[Declaration of external public global variables]=======================

//=====[Declaration and initialization of public global variables]=============

//=====[Declaration and initialization of private global variables]============

static bool flagStartParser = true;
static parser_t parser;
static parserStatus_t parserStatus;

char esp8266ReceivedData[ESP8266_BUFFER_SIZE];
char esp8266DataToSend[ESP8266_BUFFER_SIZE];

static esp8266Status_t esp8266Status;
static esp8266SendingATStatus_t esp8266SendingATStatus;
static esp8266ReceivingStatus_t esp8266ReceivingStatus;

//=====[Declarations (prototypes) of private functions]========================

static esp8266SendingATStatus_t esp8266ATCommandUpdate();

static esp8266ReceivingStatus_t esp8266ReceiveDataUpdate();

// Send commands that response "OK\r\n"
static void esp8266SendCommandWithOkResponse( char const* cmd );

// Check response for previously sended commands that only response "OK\r\n"
static esp8266SendingATStatus_t esp8266CheckOkResponse( char const receivedChar );

// Check response for previously sended commands that response parameter(s) and "\r\nOK\r\n"
static esp8266SendingATStatus_t esp8266CheckParametersAndOkResponse();

//=====[Implementations of public functions]===================================

// UART used for the ESP8266 --------------------------------------------------

void esp8266UartInit( int baudrate )
{
    uartEsp8266.baud(baudrate);
}

bool esp8266UartDataIsReceived()
{
    return uartEsp8266.readable();
}

char esp8266UartDataRead()
{
    return uartEsp8266.getc();
}

void esp8266UartReceptorFlush()
{
    while( esp8266UartDataIsReceived() ) {
        esp8266UartDataRead();
    }
}

bool esp8266UartByteRead( char* receivedByte )
{
    if( esp8266UartDataIsReceived() ) {
        *receivedByte = esp8266UartDataRead();
        return true;
    }
    return false;
}

void esp8266UartByteWrite( char sendedByte )
{
    uartEsp8266.putc( sendedByte );
    // TODO: Ver si se atora la uart con put C o como hace para no atorarse
}

void esp8266UartStringWrite( char const* str )
{
    while ( *str != NULL ) {
        esp8266UartByteWrite( (uint8_t)*str );
        str++;
    }
}

// FSM Initialization and Update ----------------------------------------------

esp8266Status_t esp8266Init()
{
    esp8266UartInit( ESP8266_BAUDRATE );

    esp8266SendingATStatus = ESP8266_AT_IDLE;
    esp8266ReceivingStatus = ESP8266_RECEIVE_IDLE;

    esp8266Status = ESP8266_IDLE;

    return esp8266Status;
}



// Notas para mi:

// el modulo ESP tiene que manejar la UART, decidiendo cuando recibe y cuando parsea respuestas. 

// Debo entonces pasarle caracteres y actualizar el parser con cada caracter que le doy de comer.

// 

esp8266Status_t esp8266Update() 
{
    switch( esp8266Status ) {
        
        // Waiting for send and AT command to ESP8266 or
        // receive data spontaniously from ESP8266
        case ESP8266_IDLE:
            if( esp8266UartDataIsReceived() ) {
                esp8266Status = ESP8266_RECEIVING_DATA;
            }
        break;

        // Sending AT command to ESP8266
        case ESP8266_PROCESSING_AT_COMMAND:
            if( esp8266ATCommandUpdate() == ESP8266_AT_IDLE ){
                esp8266Status = ESP8266_IDLE;
            }
        break;

        // Receive data sponaniously form ESP8266
        case ESP8266_RECEIVING_DATA:
            if( esp8266ReceiveDataUpdate() == ESP8266_RECEIVE_IDLE ){
                esp8266Status = ESP8266_IDLE;
            }
        break;

        default:
            esp8266Status = ESP8266_IDLE;
        break;
    }
    return esp8266Status;
}

// Tests AT startup. ----------------------------------------------------------

// "AT\r\n"
/*esp8266SendingATStatus_t esp8266TestAT()
{
    esp8266SendCommandWithOkResponse( "AT\r\n" );
    return esp8266CheckOkResponse();
}*/

void esp8266TestAT()
{
    esp8266SendCommandWithOkResponse( "AT\r\n" );
}

//=====[Implementations of private functions]==================================

esp8266SendingATStatus_t esp8266ATCommandUpdate() 
{
    char receivedChar = '\0';
    switch( esp8266SendingATStatus ) {

        // Module waiting to send AT command
        case ESP8266_AT_IDLE:
        break;

        // Waiting module response 
        case ESP8266_AT_PENDING_RESPONSE:
            esp8266UartByteRead( &receivedChar );
            esp8266SendingATStatus = esp8266CheckOkResponse( receivedChar );
        break;

        // Module already response
        case ESP8266_AT_RESPONSED:
        break;

        // Time-out waiting for a response
        case ESP8266_AT_TIMEOUT_WAITING_RESPONSE:
        break;

        default:
            esp8266SendingATStatus = ESP8266_AT_IDLE;
        break;
    }
    return esp8266SendingATStatus;
}

esp8266ReceivingStatus_t esp8266ReceiveDataUpdate() 
{
    switch( esp8266ReceivingStatus ) {

        // Module already receive all data
        case ESP8266_DATA_RECEIVED:
        break;

        // Module is receiving data
        case ESP8266_DATA_RECEIVING:
        break;

        // Module waiting to receive data
        case ESP8266_DATA_RECEIVE_IDLE:
        break;

        default:
            esp8266ReceivingStatus = ESP8266_DATA_RECEIVE_IDLE;
        break;
    }
    return esp8266ReceivingStatus;
}


typedef int esp8266Cmd_t;

/*
// Send commands that response "OK\r\n"
static void esp8266SendCommand( esp8266Cmd_t cmd )
{
    // Send command once.
    // only alows to send again after receive a final response.
    if( esp8266SendingATStatus == ESP8266_AT_IDLE ){
        esp8266SendingATStatus = false;
        parserInit( &parser, "OK\r\n", strlen("OK\r\n"), 50 );
        parserStart( &parser );
        esp8266SendingATStatus = ;
        esp8266UartStringWrite( cmd );
    }
}*/

static void esp8266SendCommandWithOkResponse( char const* cmd )
{
    if( esp8266Status == ESP8266_IDLE ){
        //esp8266SendingATStatus = ESP8266_PENDING_RESPONSE;
        esp8266Status = ESP8266_PROCESSING_AT_COMMAND;
        parserInit( &parser, "OK\r\n", strlen("OK\r\n"), 50 );
        parserStart( &parser );
        esp8266UartStringWrite( cmd );
    }
}

// Check response for previously sended commands that only response "OK\r\n"
static esp8266SendingATStatus_t esp8266CheckOkResponse( char const receivedChar )
{
    // Update parser status
    parserStatus = parserPatternMatchOrTimeout( &parser, receivedChar );

    // Chech parser response
    switch( parserStatus ) {
        case PARSER_TIMEOUT:
            esp8266Status = ESP8266_IDLE;
            return ESP8266_TIMEOUT_WAITING_RESPONSE;
        break;
        case PARSER_PATTERN_MATCH:
            esp8266Status = ESP8266_IDLE;
            return ESP8266_RESPONSED;
        break;
        default:
            return ESP8266_PENDING_RESPONSE;
        break;
    }
}

// Check response for previously sended commands that response parameter(s) and "\r\nOK\r\n"
static esp8266SendingATStatus_t esp8266CheckParametersAndOkResponse()
{
    return ESP8266_RESPONSED; // TODO: Implement
}