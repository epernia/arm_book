//=====[Libraries]=============================================================

#include "esp8266_at.h"

#include "mbed.h"
#include "arm_book_lib.h"
#include "sapi.h"

#ifdef ARMBOOK_DEBUG

#endif


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

static esp8266State_t esp8266State;

//=====[Declarations (prototypes) of private functions]========================

static esp8266Status_t esp8266ATCommandUpdate();

static esp8266Status_t esp8266ReceiveDataUpdate();

// Send commands that response "OK\r\n"
static esp8266Status_t esp8266SendCommandWithOkResponse( char const* cmd );

// Check response for previously sended commands that only response "OK\r\n"
static esp8266Status_t esp8266CheckOkResponse();

// Check response for previously sended commands that response parameter(s) and "\r\nOK\r\n"
static esp8266Status_t esp8266CheckParametersAndOkResponse();

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

void esp8266Init()
{
    esp8266UartInit( ESP8266_BAUDRATE );
    esp8266State = ESP8266_IDLE;
}

esp8266Status_t esp8266Update() 
{/*
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
    }*/
    return ESP8266_AT_RESPONSED;
}

/*
esp8266Status_t esp8266StatusGet()
{
    return esp8266Status;
}*/

    // Tests AT startup. ----------------------------------------------------------

    // "AT\r\n"

    esp8266Status_t esp8266TestATSend()
    {
        return esp8266SendCommandWithOkResponse( "AT\r\n" );
    }

    esp8266Status_t esp8266TestATResponse()
    {
        return esp8266CheckOkResponse();
    }

    // Restarts the ESP8266 module. -----------------------------------------------

    // "AT+RST\r\n"
    esp8266Status_t esp8266ResetSend()
    {
        return esp8266SendCommandWithOkResponse( "AT+RST\r\n" );
    }

    esp8266Status_t esp8266ResetResponse()
    {
        return esp8266CheckOkResponse();
    }

    // Sets the Wi-Fi mode of ESP32 (Station/AP/Station+AP). ----------------------

    // "AT+CWMODE=3\r\n"
    esp8266Status_t esp8266WiFiModeSetSend( esp8266WiFiMode_t mode )
    {
        // "AT+CWMODE=" + mode +  "\r\n"  (debo concatenar estos 3 strings)
        return esp8266SendCommandWithOkResponse( "AT+CWMODE=3\r\n" ); // FIXME: Hacer comando con mode variable
    }

    esp8266Status_t esp8266WiFiModeSetResponse()
    {
        return esp8266CheckOkResponse();
    }

// Query the current Wi-Fi mode of ESP32 (Station/AP/Station+AP). -------------

// "AT+CWMODE?"
esp8266Status_t esp826SendCmd6WiFiModeGetSend()
{
    return ESP8266_AT_RESPONSED; // TODO: Falta implementar
}

esp8266Status_t esp826SendCmd6WiFiModeGetResponse(
    esp8266WiFiMode_t* responseMode )
{
    return ESP8266_AT_RESPONSED; // TODO: Falta implementar
}

// Lists available APs. -------------------------------------------------------
 
// "AT+CWLAP\r\n"
esp8266Status_t esp8266ListAPsSend() {
    return ESP8266_AT_RESPONSED; // TODO: Falta implementar
}

esp8266Status_t esp8266ListAPsResponse( char* listOfAPs, 
                                        int listOfAPsMaxLen ) {
    // FIXME: It can be improved
    return ESP8266_AT_RESPONSED; // TODO: Falta implementar
}

    // Disconnects from the AP. ---------------------------------------------------

    // "AT+CWQAP\r\n"
    esp8266Status_t esp8266DisconnectFromAPSend()
    {
        return esp8266SendCommandWithOkResponse( "AT+CWQAP\r\n" );
    }

    esp8266Status_t esp8266DisconnectFromAPResponse()
    {
        return esp8266CheckOkResponse();
    }

// Connects to an AP. ---------------------------------------------------------

// AT+CWJAP=<ssid>,<pwd>
esp8266Status_t esp8266ConnectToAPSend( char const* ssid, char const* pwd )
{
    return ESP8266_AT_RESPONSED; // TODO: Falta implementar
}

esp8266Status_t esp8266ConnectToAPResponse()
{
    return ESP8266_AT_RESPONSED; // TODO: Falta implementar
}

// AT+CWJAP=<ssid>,<pwd>[,<bssid>]                        
esp8266Status_t esp8266ConnectToAPWithMACSend( char const* ssid, 
                                               char const* pwd , 
                                               char const* bssid )
{
    // Examples:
    // 
    // AT+CWJAP="abc","0123456789"
    // 
    // For example, if the target AP’s SSID is "ab\,c" and the password is
    // "0123456789"\", the command isas follows:
    //     AT+CWJAP="ab\\\,c","0123456789\"\\"
    // 
    // If multiple APs have the same SSID as "abc", the target AP can be found
    // by BSSID:
    //     AT+CWJAP="abc","0123456789","ca:d7:19:d8:a6:44"
    return ESP8266_AT_RESPONSED; // TODO: Falta implementar
}

esp8266Status_t esp8266ConnectToAPWithMACResponse()
{
    return ESP8266_AT_RESPONSED; // TODO: Falta implementar
}

// Query the AP to which the ESP32 Station is already connected. --------------

// AT+CWJAP?
esp8266Status_t esp8266WhichAPIsConnectedSend()
{
    return ESP8266_AT_RESPONSED; // TODO: Falta implementar
}

esp8266Status_t esp8266WhichAPIsConnectedResponse( char* response,
                                                   int responseMaxLen )
{
    return ESP8266_AT_RESPONSED; // TODO: Falta implementar
}

// Configures the multiple connections mode. ----------------------------------

// "AT+CIPMUX?\r\n"
esp8266Status_t esp8266ConnectionsModeGetSend()
{
    return ESP8266_AT_RESPONSED; // TODO: Falta implementar
}

esp8266Status_t esp8266ConnectionsModeGetResponse(
    esp8266ConnectionsMode_t* respose )
{
    return ESP8266_AT_RESPONSED; // TODO: Falta implementar
}

    // "AT+CIPMUX=1\r\n"
    esp8266Status_t esp8266ConnectionsModeSetSend( esp8266ConnectionsMode_t mode )
    {
        // - The default mode is single connection mode.
        // - This mode can only be changed after all connections are disconnected, use:
        //   "AT+CIPCLOSE=5\r\n" // ID = 5 all connections will be closed
        // - If the TCP server is running, it must be deleted (AT+CIPSERVER=0)
        //   before the single connection mode is activated.
        // - Multiple connections can only be set when transparent transmission
        //   is disabled (AT+CIPMODE=0).
        if( mode == ESP8266_SIGNLE_CONNECTION ) {
            return esp8266SendCommandWithOkResponse( "AT+CIPMUX=0\r\n" );
        } else if( mode == ESP8266_MULTIPLE_CONNECTIONS ) {
            return esp8266SendCommandWithOkResponse( "AT+CIPMUX=1\r\n" );
        } else{
            return ESP8266_AT_INVALID_PARAMETER;
        }
    }

    esp8266Status_t esp8266ConnectionsModeSetResponse()
    {
        return esp8266CheckOkResponse();
    }

    // Deletes/Creates TCP server. ------------------------------------------------

    // "AT+CIPSERVER=1,80\r\n"
    esp8266Status_t esp8266CreateTCPServerSend( int port )
    {
        // Example
        // "AT+CIPSERVER=1,80\r\n"
        // In this case creates a Server with port 80 

        // <mode>:
        //   0: delete server.
        //   1: create server.
        // <port>: port number; 333 by default

        // - A TCP server can only be created when multiple connections are
        //   activated (AT+CIPMUX=1).
        // - A server monitor will automatically be created when the TCP server is
        //   created.
        // - When a client is connected to the server, it will take up one
        //   connection and be assigned an ID.
        // - Use ESP8266_SERVER_DEFAULT_PORT for port number = 333

        if( port == ESP8266_SERVER_DEFAULT_PORT ) {
            return esp8266SendCommandWithOkResponse( "AT+CIPSERVER=1\r\n" );
        } else {
            return esp8266SendCommandWithOkResponse( "AT+CIPSERVER=1,80\r\n" ); // FIXME: allow to select a port
        }
    }

    esp8266Status_t esp8266CreateTCPServerResponse()
    {
        return esp8266CheckOkResponse();
    }

    // "AT+CIPSERVER=0\r\n"
    esp8266Status_t esp8266DeleteTCPServerSend()
    {
        return esp8266SendCommandWithOkResponse( "AT+CIPSERVER=0\r\n" );
    }

    esp8266Status_t esp8266DeleteTCPServerResponse()
    {
        return esp8266CheckOkResponse();
    }

// Gets the local IP address. -------------------------------------------------

// "AT+CIFSR\r\n"
esp8266Status_t esp8266LocalIPAddressGetSend()
{    
    return ESP8266_AT_RESPONSED; // TODO: Falta implementar
}
                                          
esp8266Status_t esp8266LocalIPAddressGetResponse( char* softAP_IPaddress,
                                                  char* station_IPaddress )
{    
    return ESP8266_AT_RESPONSED; // TODO: Falta implementar
}

// Gets the connection status. ------------------------------------------------

// "AT+CIPSTATUS\r\n"
esp8266Status_t esp8266ConnectionStatusGetSend()
{
    return ESP8266_AT_RESPONSED; // TODO: Falta implementar
}

esp8266Status_t esp8266ConnectionStatusGetResponse(
    esp8266ConnectionStatus_t* result )
{
    return ESP8266_AT_RESPONSED; // TODO: Falta implementar
}

// Sends data. ----------------------------------------------------------------

// "AT+CIPSEND=[<link ID>,]<length>[,<remote IP>,<remote port>]\r\n"
esp8266Status_t esp8266SendTCPOrSSLDataSend(
    int linkID,  // ID of the connection (0~4), for multiple connections.
                 // (-1 = single conection = ESP8266_SINGLE_CONNECTION).
    int length,  // Data length, MAX: 2048 bytes. 
                 // If the number of sent bytes is bigger than the size defined
                 // as n, the response will be BUSY. After sending the first n 
                 // number of bytes, SEND OK will be returned.
    char* data   // Data to send (String NULL terminated).
)
{
    return ESP8266_AT_RESPONSED; // TODO: Falta implementar
}

esp8266Status_t esp8266SendTCPOrSSLDataResponse()
{
    return ESP8266_AT_RESPONSED; // TODO: Falta implementar
}

// "AT+CIPSEND=[<link ID>,]<length>[,<remote IP>,<remote port>]\r\n"
esp8266Status_t esp8266SendUDPDataSend(
    int linkID, // ID of the connection (0~4), for multiple connections.
                // (-1 = single conection = ESP8266_SINGLE_CONNECTION).
    int length,          // Data length, MAX: 2048 bytes.
    char remoteIP[16],   // Remote IP can be set in UDP transmission.
    char remotePort[10], // Remote port can be set in UDP transmission.
    char* data           // Data to send (String NULL terminated).
)
{
    // Returns ">", 
    // Send data.
    // After data is sended:
    //    Returns "ERROR\r\n" if the connection cannot be established or gets
    //    disrupted during data transmission.
    //    Returns "SEND OK\r\n" if data is transmitted successfully.

    /* Example
    "AT+CIPSEND=0,93\r\n"
    "<!DOCTYPE HTML><html><head><title>ESP8266 test</title></head><body>Hello World!</body></html>"
    */
    return ESP8266_AT_RESPONSED; // TODO: Falta implementar
}

esp8266Status_t esp8266SendUDPDataResponse()
{
    return ESP8266_AT_RESPONSED; // TODO: Falta implementar
}

    // Closes TCP/UDP/SSL connection. ---------------------------------------------

    // "AT+CIPCLOSE=" + linkID + "\r\n"
    esp8266Status_t esp8266CloseConnectionSend( int linkID )
    {
        // linkID: ID number of connections to be closed;
        // when ID = 5, all connections will be closed.
        // Remember to use ESP8266_CLOSE_ALL_CONNECTIONS for linkID=5
        return esp8266SendCommandWithOkResponse( "AT+CIPCLOSE=5\r\n" ); // FIXME: Lo hice fijo para cerrar todas las conexiones, hacer variable
    }

    esp8266Status_t esp8266CloseConnectionResponse()
    {
        return esp8266CheckOkResponse();
    }

// Establishes TCP connection -------------------------------------------------

// AT+CIPSTART="TCP","iot.espressif.cn",8000
// AT+CIPSTART="TCP","192.168.101.110",1000
esp8266Status_t esp8266EstablishTCPConnectionSend( 
   int linkID, // ID of network connection (0~4), used for multiple connections.
               // (-1 = single conection = ESP8266_SINGLE_CONNECTION).
   char* remoteIP, // String parameter indicating the remote IP address.
   int remotePort, // The remote port number.
   int TCP_keepAlive // Optional. Detection time interval when TCP is kept
                     // alive; this function is disabled by default.
                     // - 0: disable TCP keep-alive.
                     // - 1~7200: detection time interval; unit: second (s)
)
{
    /*
    "AT+CIPSTART=" + intToString(linkID) + "," + "\"TCP\",\"" + 
    remoteIP + "\"," + intToString(remotePort) + 
    intToString(TCP_keepAlive) + "\r\n"
    */

    // Return:
    //     ESP8266_AT_RESPONSED
    //     ESP8266_ERR
    
    return ESP8266_AT_RESPONSED; // TODO: Falta implementar
}

esp8266Status_t esp8266EstablishTCPConnectionResponse()
{
    return ESP8266_AT_RESPONSED; // TODO: Falta implementar
}

// Establishes UDP transmission -----------------------------------------------

// AT+CIPSTART="UDP","192.168.101.110",1000,1002,2
esp8266Status_t esp8266EstablishUDPTransmissionSend( 
   int linkID, // ID of network connection (0~4), used for multiple connections.
               // (-1 = single conection = ESP8266_SINGLE_CONNECTION).
   char* remoteIP,    // String parameter indicating the remote IP address.
   int remotePort,    // The remote port number.
   int UDP_localPort, // The local UDP port number.
   esp8266UDP_mode_t UDP_mode // Fixed or Changeable
)
{
    /*
    "AT+CIPSTART=" + intToString(linkID) + "," + "\"SSL\",\"" + 
    remoteIP + "\"," + intToString(remotePort) + 
    intToString(TCP_keepAlive) + "\r\n"
    */
    // Return:
    //     ESP8266_AT_RESPONSED
    //     ESP8266_ERR
    //     ESP8266_ALREADY_CONNECT

    return ESP8266_AT_RESPONSED; // TODO: Falta implementar
}

esp8266Status_t esp8266EstablishUDPTransmissionResponse()
{
    return ESP8266_AT_RESPONSED; // TODO: Falta implementar
}

// Establishes SSL connection. ------------------------------------------------

// AT+CIPSTART="SSL","iot.espressif.cn",8443
esp8266Status_t esp8266EstablishSSLConnectionSend( 
   int linkID, // ID of network connection (0~4), used for multiple connections
               // (-1 = single conection = ESP8266_SINGLE_CONNECTION).
   char* remoteIP,   // String parameter indicating the remote IP address.
   int remotePort,   // The remote port number.
   int TCP_keepAlive // Optional. Detection time interval when TCP is kept
                     // alive; this function is disabled by default.
                     // - 0: disable TCP keep-alive.
                     // - 1~7200: detection time interval; unit: second (s)
)
{
    /*
    "AT+CIPSTART=" + intToString(linkID) + "," + "\"SSL\",\"" + 
    remoteIP + "\"," + intToString(remotePort) + 
    intToString(TCP_keepAlive) + "\r\n"
    */
    // Return:
    //     ESP8266_OK
    //     ESP8266_ERR
    //     ESP8266_ALREADY_CONNECT

    return ESP8266_AT_RESPONSED; // TODO: Falta implementar
}

esp8266Status_t esp8266EstablishSSLConnectionResponse()
{
    return ESP8266_AT_RESPONSED; // TODO: Falta implementar
}


//=====[Implementations of private functions]==================================

esp8266Status_t esp8266ATCommandUpdate() 
{/*
    char receivedChar = '\0';
    switch( esp8266SendingATStatus ) {

        // Module waiting to send AT command
        case ESP8266_AT_IDLE:
        break;

        // Waiting module response 
        case ESP8266_AT_PENDING_RESPONSE:
            //esp8266UartByteRead( &receivedChar );
            //esp8266SendingATStatus = esp8266CheckOkResponse( receivedChar );
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
    }*/
    return ESP8266_AT_RESPONSED;
}

esp8266Status_t esp8266ReceiveDataUpdate() 
{/*
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
    }*/
    return ESP8266_AT_RESPONSED;
}

static esp8266Status_t esp8266SendCommandWithOkResponse( char const* cmd )
{
    if( esp8266State == ESP8266_IDLE ){
        parserInit( &parser, "OK\r\n", strlen("OK\r\n"), 50 );
        parserStart( &parser );
        esp8266UartStringWrite( cmd );
        esp8266State = ESP8266_PROCESSING_AT_COMMAND; //ESP8266_PENDING_RESPONSE;
        return ESP8266_AT_SENDED;
    } else {        
        return ESP8266_AT_NOT_SENDED;   
    }
}

// Check response for previously sended commands that only response "OK\r\n"
static esp8266Status_t esp8266CheckOkResponse()
{
    // Receive from UART connected to ESP8266 if is data available
    char receivedChar = '\0';
    esp8266UartByteRead( &receivedChar );
    
    // Update parser status
    parserStatus = parserPatternMatchOrTimeout( &parser, receivedChar );

    // Chech parser response
    switch( parserStatus ) {
        case PARSER_TIMEOUT:
            esp8266State = ESP8266_IDLE;
            return ESP8266_AT_TIMEOUT_WAITING_RESPONSE;
        break;
        case PARSER_PATTERN_MATCH:
            esp8266State = ESP8266_IDLE;
            return ESP8266_AT_RESPONSED;
        break;
        default:
            return ESP8266_AT_PENDING_RESPONSE;
        break;
    }
}

// Check response for previously sended commands that response parameter(s) and "\r\nOK\r\n"
static esp8266Status_t esp8266CheckParametersAndOkResponse()
{
    return ESP8266_AT_RESPONSED; // TODO: Implement
}
