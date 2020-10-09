//=====[Libraries]=============================================================

#include "mbed.h"
#include "arm_book_lib.h"

#include "esp8266_module.h"

#include "sapi.h"

//=====[Declaration of private defines]========================================

//=====[Declaration of private data types]=====================================

//=====[Declaration and initialization of public global objects]===============

// D42 = PE_8 = UART7_TX
// D41 = PE_7 = UART7_RX
//Serial esp8266Uart( D42, D41 );

//=====[Declaration of external public global variables]=======================

//=====[Declaration and initialization of public global variables]=============

//=====[Declaration and initialization of private global variables]============

static bool flagStartParser = true;
static parser_t parser;
static parserStatus_t parserStatus;

//=====[Declarations (prototypes) of private functions]========================

/*
static void esp8266UartInit();
static bool esp8266UartCharRead( char* receivedChar );
static void esp8266UartCharWrite( char c );
static void esp8266UartStringWrite( char const* str );
*/

// Send commands that response "OK\r\n"
static void esp8266SendCommandWithOkResponse( char const* cmd );

// Check response for previously sended commands that only response "OK\r\n"
static esp8266Status_t esp8266CheckOkResponse();

// Check response for previously sended commands that response parameter(s) and "\r\nOK\r\n"
static esp8266Status_t esp8266CheckParametersAndOkResponse();

//=====[Implementations of public functions]===================================

// Initialization -------------------------------------------------------------

esp8266Status_t esp8266Init() /* uartMap_t uartConnectedToEsp, 
                             uartMap_t uartForConsoleInfo,
                             int baudRateForBothUarts );*/
{
    tickInit( 1 ); // 1ms tick (for non blocking delay works)
    parserUartInit( ESP8266_BAUDRATE );
    return ESP8266_OK;
}

// Tests AT startup. ----------------------------------------------------------

// "AT\r\n"
esp8266Status_t esp8266TestAT()
{
    esp8266SendCommandWithOkResponse( "AT\r\n" );
    return esp8266CheckOkResponse();
}

// Restarts the ESP8266 module. -----------------------------------------------

// "AT+RST\r\n"
esp8266Status_t esp8266Reset()
{
    esp8266SendCommandWithOkResponse( "AT+RST\r\n" );
    return esp8266CheckOkResponse();
}

// Sets the Wi-Fi mode of ESP32 (Station/AP/Station+AP). ----------------------

// "AT+CWMODE=3\r\n"
esp8266Status_t esp8266WiFiModeSet( esp8266WiFiMode_t mode )
{
    // "AT+CWMODE=" + mode +  "\r\n"  (debo concatenar estos 3 strings)
    esp8266SendCommandWithOkResponse( "AT+CWMODE=3\r\n" ); // FIXME: Hacer comando con mode variable
    return esp8266CheckOkResponse();
}

// Query the current Wi-Fi mode of ESP32 (Station/AP/Station+AP). -------------

// "AT+CWMODE?"
esp8266Status_t esp8266WiFiModeGet( esp8266WiFiMode_t* responseMode )
{
    return ESP8266_OK; // TODO: Falta implementar
}

// Lists available APs. -------------------------------------------------------
 
// "AT+CWLAP\r\n"
esp8266Status_t esp8266ListAPs( char* listOfAPs, int listOfAPsMaxLen )
{
    // FIXME: It can be improved
    return ESP8266_OK; // TODO: Falta implementar
}

// Disconnects from the AP. ---------------------------------------------------

// "AT+CWQAP\r\n"
esp8266Status_t esp8266DisconnectFromAP()
{
    esp8266SendCommandWithOkResponse( "AT+CWQAP\r\n" );
    return esp8266CheckOkResponse();
}

// Connects to an AP. ---------------------------------------------------------

// AT+CWJAP=<ssid>,<pwd>
esp8266Status_t esp8266ConnectToAP( char const* ssid, 
                                    char const* pwd )
{
    return ESP8266_OK; // TODO: Falta implementar
}

// AT+CWJAP=<ssid>,<pwd>[,<bssid>]                        
esp8266Status_t esp8266ConnectToAPWithMAC( char const* ssid, 
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
    return ESP8266_OK; // TODO: Falta implementar
}

// Query the AP to which the ESP32 Station is already connected. --------------

// AT+CWJAP?
esp8266Status_t esp8266WhichAPIsConnected( char* response, 
                                           int responseMaxLen )
{
    return ESP8266_OK; // TODO: Falta implementar
}

// Configures the multiple connections mode. ----------------------------------

// "AT+CIPMUX?\r\n"
esp8266Status_t esp8266ConnectionsModeGet( esp8266ConnectionsMode_t* respose )
{
    return ESP8266_OK; // TODO: Falta implementar
}

// "AT+CIPMUX=1\r\n"
esp8266Status_t esp8266ConnectionsModeSet( esp8266ConnectionsMode_t mode )
{
    // - The default mode is single connection mode.
    // - This mode can only be changed after all connections are disconnected, use:
    //   "AT+CIPCLOSE=5\r\n" // ID = 5 all connections will be closed
    // - If the TCP server is running, it must be deleted (AT+CIPSERVER=0)
    //   before the single connection mode is activated.
    // - Multiple connections can only be set when transparent transmission
    //   is disabled (AT+CIPMODE=0).
    if( mode == ESP8266_SIGNLE_CONNECTION ) {
        esp8266SendCommandWithOkResponse( "AT+CIPMUX=0\r\n" );
        return esp8266CheckOkResponse();
    } else if( mode == ESP8266_MULTIPLE_CONNECTIONS ) {
        esp8266SendCommandWithOkResponse( "AT+CIPMUX=1\r\n" );
        return esp8266CheckOkResponse();
    } else{
        return ESP8266_ERR;
    }
}

// Deletes/Creates TCP server. ------------------------------------------------

// "AT+CIPSERVER=1,80\r\n"
esp8266Status_t esp8266CreateTCPServer( int port )
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
        esp8266SendCommandWithOkResponse( "AT+CIPSERVER=1\r\n" );
        return esp8266CheckOkResponse();
    } else {
        esp8266SendCommandWithOkResponse( "AT+CIPSERVER=1,80\r\n" ); // FIXME: allow to select a port
        return esp8266CheckOkResponse();
    }
}

// "AT+CIPSERVER=0\r\n"
esp8266Status_t esp8266DeleteTCPServer()
{
    return ESP8266_OK; // TODO: Falta implementar
}

// Gets the local IP address. -------------------------------------------------

// "AT+CIFSR\r\n"
esp8266Status_t esp8266GetLocalIPAddress( char* softAP_IPaddress,
                                          char* station_IPaddress )
{    
    return ESP8266_OK; // TODO: Falta implementar
}

// Gets the connection status. ------------------------------------------------

// "AT+CIPSTATUS\r\n"
esp8266Status_t esp8266GetConnectionStatus( esp8266ConnectionStatus_t* result )
{
    return ESP8266_OK; // TODO: Falta implementar
}

// Sends data. ----------------------------------------------------------------

// ---------------> Implementada en ejemplo!

// "AT+CIPSEND=[<link ID>,]<length>[,<remote IP>,<remote port>]\r\n"
esp8266Status_t esp8266SendTCPOrSSLData(
    int linkID,  // ID of the connection (0~4), for multiple connections.
                 // (-1 = single conection = ESP8266_SINGLE_CONNECTION).
    int length,  // Data length, MAX: 2048 bytes.
    char* data ) // Data to send (String NULL terminated).
{
    return ESP8266_OK; // TODO: Falta implementar
}

// "AT+CIPSEND=[<link ID>,]<length>[,<remote IP>,<remote port>]\r\n"
esp8266Status_t esp8266SendUDPData(
    int linkID, // ID of the connection (0~4), for multiple connections.
                // (-1 = single conection = ESP8266_SINGLE_CONNECTION).
    int length,          // Data length, MAX: 2048 bytes.
    char remoteIP[16],   // Remote IP can be set in UDP transmission.
    char remotePort[10], // Remote port can be set in UDP transmission.
    char* data )         // Data to send (String NULL terminated).
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
    return ESP8266_OK; // TODO: Falta implementar
}

// Closes TCP/UDP/SSL connection. ---------------------------------------------

// "AT+CIPCLOSE=" + linkID + "\r\n"
esp8266Status_t esp8266CloseConnection( int linkID )
    // linkID: ID number of connections to be closed;
    // when ID = 5, all connections will be closed.
    // Remember to use ESP8266_CLOSE_ALL_CONNECTIONS for linkID=5
{
    esp8266SendCommandWithOkResponse( "AT+CIPCLOSE=5\r\n" ); // FIXME: Lo hice fijo para cerrar todas las conexiones, hacer variable
    return esp8266CheckOkResponse();
}

// Establishes TCP connection -------------------------------------------------

// AT+CIPSTART="TCP","iot.espressif.cn",8000
// AT+CIPSTART="TCP","192.168.101.110",1000
esp8266Status_t esp8266EstablishTCPConnection( 
   int linkID, // ID of network connection (0~4), used for multiple connections.
               // (-1 = single conection = ESP8266_SINGLE_CONNECTION).
   char* remoteIP, // String parameter indicating the remote IP address.
   int remotePort, // The remote port number.
   int TCP_keepAlive ) // Optional. Detection time interval when TCP is kept
                       // alive; this function is disabled by default.
                       // - 0: disable TCP keep-alive.
                       // - 1~7200: detection time interval; unit: second (s)
{
    /*
    "AT+CIPSTART=" + intToString(linkID) + "," + "\"TCP\",\"" + 
    remoteIP + "\"," + intToString(remotePort) + 
    intToString(TCP_keepAlive) + "\r\n"
    */

    // Return:
    //     ESP8266_OK
    //     ESP8266_ERR
    
    return ESP8266_OK; // TODO: Falta implementar
}

// Establishes UDP transmission -----------------------------------------------

// AT+CIPSTART="UDP","192.168.101.110",1000,1002,2
esp8266Status_t esp8266EstablishUDPTransmission( 
   int linkID, // ID of network connection (0~4), used for multiple connections.
               // (-1 = single conection = ESP8266_SINGLE_CONNECTION).
   char* remoteIP,    // String parameter indicating the remote IP address.
   int remotePort,    // The remote port number.
   int UDP_localPort, // The local UDP port number.
   esp8266UDP_mode_t UDP_mode ) // Fixed or Changeable
{
    /*
    "AT+CIPSTART=" + intToString(linkID) + "," + "\"UDP\",\"" + 
    remoteIP + "\"," + intToString(remotePort) + 
    intToString(UDP_localPort) + intToString(UDP_mode) + "\r\n"
    */

    // Return:
    //     ESP8266_OK
    //     ESP8266_ERR
    //     ESP8266_ALREADY_CONNECT
    
    return ESP8266_OK; // TODO: Falta implementar
}

// Establishes SSL connection. ------------------------------------------------

// AT+CIPSTART="SSL","iot.espressif.cn",8443
esp8266Status_t esp8266EstablishSSLConnection( 
   int linkID, // ID of network connection (0~4), used for multiple connections
               // (-1 = single conection = ESP8266_SINGLE_CONNECTION).
   char* remoteIP,   // String parameter indicating the remote IP address.
   int remotePort,   // The remote port number.
   int TCP_keepAlive ) // Optional. Detection time interval when TCP is kept
                       // alive; this function is disabled by default.
                       // - 0: disable TCP keep-alive.
                       // - 1~7200: detection time interval; unit: second (s)
{
    // ---------------> Implementada en ejemplo, a medias!
    /*
    "AT+CIPSTART=" + intToString(linkID) + "," + "\"SSL\",\"" + 
    remoteIP + "\"," + intToString(remotePort) + 
    intToString(TCP_keepAlive) + "\r\n"
    */
    // Return:
    //     ESP8266_OK
    //     ESP8266_ERR
    //     ESP8266_ALREADY_CONNECT

    return ESP8266_OK; // TODO: Falta implementar
}

//=====[Implementations of private functions]==================================

/*
static void esp8266UartInit()
{
    esp8266Uart.baud(ESP8266_BAUDRATE);
}

static bool esp8266UartCharRead( char* receivedChar )
{
    if( esp8266Uart.readable() ) {
        *receivedChar = esp8266Uart.getc();
        return true;
    }
    return false;
}

static void esp8266UartCharWrite( char c )
{
    esp8266Uart.putc(c);
}

static void esp8266UartStringWrite( char const* str )
{
    esp8266Uart.printf( "%s", str );
}
*/

// Send commands that response "OK\r\n"
static void esp8266SendCommandWithOkResponse( char const* cmd )
{
    // Send command once.
    // only alows to send again after receive a final response.
    if( flagStartParser ){
        flagStartParser = false;
        parserInit( &parser, "OK\r\n", strlen("OK\r\n"), 50 );
        parserStart( &parser ); 
        parserUartStringWrite( cmd );
    }
}

// Check response for previously sended commands that only response "OK\r\n"
static esp8266Status_t esp8266CheckOkResponse()
{
    // Update parser status
    parserStatus = parserUpdate( &parser );

    // Chech parser response
    switch( parserStatus ) {
        case PARSER_TIMEOUT:
            flagStartParser = true;
            return ESP8266_RESPONSE_TIMEOUT;
        break;
        case PARSER_PATTERN_MATCH:
            flagStartParser = true;
            return ESP8266_OK;
        break;
        default:
            flagStartParser = false;
            return ESP8266_PENDING_RESPONSE;
        break;
    }
}

// Check response for previously sended commands that response parameter(s) and "\r\nOK\r\n"
static esp8266Status_t esp8266CheckParametersAndOkResponse()
{
    return ESP8266_OK; // TODO: Implement
}