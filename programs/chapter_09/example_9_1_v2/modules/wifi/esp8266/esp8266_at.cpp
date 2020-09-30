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

static bool esp8266ATCommandsPendingToSend = false;
static esp8266SendingATStatus_t esp8266SendingATStatus;

static esp8266ReceivingStatus_t esp8266ReceivingStatus;

//=====[Declarations (prototypes) of private functions]========================

// Anwer true if there is an AT command pending to send
static bool esp8266AreAnATCommandPendingToSend();

// Send commands that response "OK\r\n"
static void esp8266SendCommandWithOkResponse( char const* cmd );

// Check response for previously sended commands that only response "OK\r\n"
static esp8266SendingATStatus_t esp8266CheckOkResponse();

// Check response for previously sended commands that response parameter(s) and "\r\nOK\r\n"
static esp8266SendingATStatus_t esp8266CheckParametersAndOkResponse();

//=====[Implementations of public functions]===================================

// UART used for the ESP8266 --------------------------------------------------

void esp8266UartInit( int baudrate )
{
    uartEsp8266.baud(baudrate);
}

void esp8266UartReceptorFlush()
{
    while( esp8266UartDataIsReceived() ) {
        esp8266UartDataRead();
    }
}

bool esp8266UartDataIsReceived()
{
    return uartEsp8266.readable();
}

uint8_t esp8266UartDataRead()
{
    return uartEsp8266.getc();
}

bool esp8266UartByteRead( uint8_t* receivedByte )
{
    if( esp8266UartDataIsReceived() ) {
        *receivedByte = esp8266UartDataRead();
        return true;
    }
    return false;
}

void esp8266UartByteWrite( uint8_t sendedByte )
{
    uartEsp8266.putc( (char)(sendedByte) );
    // TODO: Ver si se atora la uart con put C o como hace para no atorarse
}

void esp8266UartStringWrite( char const* str )
{
    while ( *str != NULL ) {
        parserUartByteWrite( (uint8_t)*str );
        str++;
    }
}

// FSM Initialization and Update ----------------------------------------------

esp8266Status_t esp8266Init() /* uartMap_t uartConnectedToEsp, 
                             uartMap_t uartForConsoleInfo,
                             int baudRateForBothUarts );*/
{
    esp8266UartInit( ESP8266_BAUDRATE );

    esp8266SendingATStatus = ESP8266_AT_WAITING;
    esp8266ReceivingStatus = ESP8266_RECEIVE_WAITING;

    esp8266Status = ESP8266_IDLE

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
            } else if( esp8266AreAnATCommandPendingToSend() ) {
                esp8266Status = ESP8266_SENDING_AT_COMMAND;
            }
        break;

        // Sending AT command to ESP8266
        case ESP8266_SENDING_AT_COMMAND:
            if( esp8266ATCommandUpdate() == ESP8266_AT_WAITING ){
                esp8266Status = ESP8266_IDLE;
            }
        break;

        // Receive data sponaniously form ESP8266
        case ESP8266_RECEIVING_DATA:
            if( esp8266ReceiveDataUpdate() == ESP8266_RECEIVE_WAITING ){
                esp8266Status = ESP8266_IDLE;
            }
        break;

        default:
            esp8266Status = ESP8266_IDLE;
        break;
    }
    return esp8266Status;
}

esp8266SendingATStatus_t esp8266ATCommandUpdate() 
{
    switch( esp8266SendingATStatus ) {

        // Module waiting to send AT command
        case ESP8266_AT_WAITING:
        break;

        // Waiting module response
        case ESP8266_PENDING_RESPONSE:
        break;

        // Module already response
        case ESP8266_RESPONSED:
        break;

        // Time-out waiting for a response
        case ESP8266_TIMEOUT_WAITING_RESPONSE:
        break;

        default:
            esp8266SendingATStatus = ESP8266_AT_WAITING;
        break;
    }
    return esp8266SendingATStatus;
}

esp8266ReceivingStatus_t esp8266ReceiveDataUpdate() 
{
    switch( esp8266ReceivingStatus ) {

        // Module already receive all data
        case ESP8266_RECEIVED:
        break;

        // Module is receiving data
        case ESP8266_RECEIVING:
        break;

        // Module waiting to receive data
        case ESP8266_RECEIVE_WAITING:
        break;

        default:
            esp8266ReceivingStatus = ESP8266_RECEIVE_WAITING;
        break;
    }
    return esp8266ReceivingStatus;
}

// Tests AT startup. ----------------------------------------------------------

// "AT\r\n"
esp8266SendingATStatus_t esp8266TestAT()
{
    esp8266SendCommandWithOkResponse( "AT\r\n" );
    return esp8266CheckOkResponse();
}

//=====[Implementations of private functions]==================================

static bool esp8266AreAnATCommandPendingToSend()
{
    return esp8266AreAnATCommandPendingToSend;
}

// Send commands that response "OK\r\n"
static void esp8266SendCommandWithOkResponse( char const* cmd )
{
    // Send command once.
    // only alows to send again after receive a final response.
    if( flagStartParser ){
        flagStartParser = false;
        parserInit( &parser, "OK\r\n", strlen("OK\r\n"), 50 );
        parserStart( &parser ); 
        esp8266UartStringWrite( cmd );
    }
}

// Check response for previously sended commands that only response "OK\r\n"
static esp8266SendingATStatus_t esp8266CheckOkResponse()
{
    
    // Update parser status
    char receivedChar = esp8266UartDataRead();
    parserStatus = parserPatternMatchOrTimeout( &parser, receivedChar );

    // Chech parser response
    switch( parserStatus ) {
        case PARSER_TIMEOUT:
            flagStartParser = true;
            return ESP8266_TIMEOUT_WAITING_RESPONSE;
        break;
        case PARSER_PATTERN_MATCH:
            flagStartParser = true;
            return ESP8266_RESPONSED;
        break;
        default:
            flagStartParser = false;
            return ESP8266_PENDING_RESPONSE;
        break;
    }
}

// Check response for previously sended commands that response parameter(s) and "\r\nOK\r\n"
static esp8266SendingATStatus_t esp8266CheckParametersAndOkResponse()
{
    return ESP8266_RESPONSED; // TODO: Implement
}