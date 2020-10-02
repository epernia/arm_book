//=====[#include guards - begin]===============================================

#ifndef _ESP8266_AT_H_
#define _ESP8266_AT_H_

//=====[Libraries]=============================================================

//=====[Declaration of public defines]=========================================

#define ESP8266_BAUD_RATE                  115200
#define ESP8266_BUFFER_SIZE              2048

#define ESP8266_SINGLE_CONNECTION        -1     // "AT+CIPSTART="
#define ESP8266_CLOSE_ALL_CONNECTIONS    5      // "AT+CLOSE=<linkID>"
#define ESP8266_SERVER_DEFAULT_PORT      333    // AT+CIPSERVER=<mode>[,<port>]

//=====[Declaration of public data types]======================================

// Module status
typedef enum{
    ESP8266_AT_SENT,                     // Module already send the command.  
    ESP8266_AT_NOT_SENT,                 // Not sended.
    ESP8266_AT_INVALID_PARAMETER,        // Invalid parameter(s).

    ESP8266_AT_RESPONSE_PENDING,         // Waiting module response.  
    ESP8266_AT_RESPONDED,                // Module already response.
    ESP8266_AT_TIMEOUT,                  // Time-out waiting for a response.

    ESP8266_RECEIVING,                   // Module is receiving data. 
}esp8266Status_t;

// Responses after sending an AT command to ESP8266
typedef enum{
    ESP8266_OK, 
    ESP8266_ERR,
    ESP8266_ALREADY_CONNECT,
    ESP8266_BUSY,
}esp8266ATResponse_t;

// "AT+CWMODE?\r\n"
// "AT+CWMODE=<mode>\r\n"
typedef enum{
    ESP8266_WIFI_MODE_STATION     = 1, // Station mode
    ESP8266_WIFI_MODE_SOFT_AP     = 1, // SoftAP mode
    ESP8266_WIFI_MODE_SOFT_AP_STA = 3, // SoftAP+Station mode
}esp8266WiFiMode_t;

// "AT+CIPMUX?\r\n"
typedef enum{
    ESP8266_SIGNLE_CONNECTION    = 0,
    ESP8266_MULTIPLE_CONNECTIONS = 1,
}esp8266ConnectionsMode_t;

// "AT+CIPSTART" UDP Mode
typedef enum{
    ESP8266_UDP_MODE_FIXED      = 0, // Fixed Remote IP and Port.
    ESP8266_UDP_MODE_CHANGEABLE = 2, // Changeable Remote IP and Port.
}esp8266UDP_mode_t;

// "AT+CIPSTATUS\r\n"
typedef struct{
    int status; // status of the ESP32 Station interface:
        //   2: The ESP32 Station is connected to an AP and its IP is obtained.
        //   3: The ESP32 Station has created a TCP or UDP transmission.
        //   4: The TCP or UDP transmission of ESP32 Station is disconnected.
        //   5: The ESP32 Station does NOT connect to an AP.
    int linkID; // ID of the connection (0~4), used for multiple connections.
                // (-1 = single conection = ESP8266_SINGLE_CONNECTION).
    char type[4];        // String parameter, "TCP" or "UDP".
    char remoteIP[16];   // String parameter indicating the remote IP address.
    char remotePort[10]; // The remote port number.
    char localPort[10];  // ESP32 local port number.
    int tetype; // 0: ESP32 runs as a client. 1: ESP32 runs as a server.
}esp8266ConnectionStatus_t;

//=====[Declarations (prototypes) of public functions]=========================

// UART used for the ESP8266 --------------------------------------------------

void esp8266UartInit( int baudRate );

bool esp8266UartDataIsReceived();
char esp8266UartDataRead();
void esp8266UartReceptorFlush();

bool esp8266UartByteRead( char* receivedByte );
void esp8266UartByteWrite( char sentByte );
void esp8266UartStringWrite( char const* str );

// FSM Initialization and Update ----------------------------------------------

void esp8266Init();

esp8266Status_t esp8266Update();

esp8266Status_t esp8266StatusGet();

// Tests AT startup. ----------------------------------------------------------

// "AT\r\n"
esp8266Status_t esp8266TestATSend();
esp8266Status_t esp8266TestATResponse();

// Restarts the ESP8266 module. -----------------------------------------------

// "AT+RST\r\n"
esp8266Status_t esp8266ResetSend();
esp8266Status_t esp8266ResetResponse();

// Sets the Wi-Fi mode of ESP32 (Station/AP/Station+AP). ----------------------

// "AT+CWMODE=3\r\n"
esp8266Status_t esp8266WiFiModeSetSend( esp8266WiFiMode_t mode ); 

esp8266Status_t esp8266WiFiModeSetResponse();

// Query the current Wi-Fi mode of ESP32 (Station/AP/Station+AP). -------------

// "AT+CWMODE?"
esp8266Status_t esp826SendCmd6WiFiModeGetSend();

esp8266Status_t esp826SendCmd6WiFiModeGetResponse(
    esp8266WiFiMode_t* responseMode );

// Lists available APs. -------------------------------------------------------
 
//TODO: It can be improbed
// "AT+CWLAP\r\n"
esp8266Status_t esp8266ListAPsSend();

esp8266Status_t esp8266ListAPsResponse( char* listOfAPs, 
                                        int listOfAPsMaxLen );

// Disconnects from the AP. ---------------------------------------------------

// "AT+CWQAP\r\n"
esp8266Status_t esp8266DisconnectFromAPSend();
esp8266Status_t esp8266DisconnectFromAPResponse();

// Connects to an AP. ---------------------------------------------------------

// Set the AP to which the ESP32 Station needs to be connected.
// AT+CWJAP=<ssid>,<pwd>
esp8266Status_t esp8266ConnectToAPSend( char const* ssid, char const* pwd );

esp8266Status_t esp8266ConnectToAPResponse();

// AT+CWJAP=<ssid>,<pwd>[,<bssid>]
esp8266Status_t esp8266ConnectToAPWithMACSend( char const* ssid, 
                                               char const* pwd , 
                                               char const* bssid );

esp8266Status_t esp8266ConnectToAPWithMACResponse();

// Query the AP to which the ESP32 Station is already connected. --------------

// AT+CWJAP?
esp8266Status_t esp8266WhichAPIsConnectedSend();

esp8266Status_t esp8266WhichAPIsConnectedResponse( char* response,
                                                   int responseMaxLen );

// Configures the multiple connections mode. ----------------------------------

// "AT+CIPMUX?\r\n"
esp8266Status_t esp8266ConnectionsModeGetSend();

esp8266Status_t esp8266ConnectionsModeGetResponse(
    esp8266ConnectionsMode_t* respose );

// "AT+CIPMUX=1\r\n"
esp8266Status_t esp8266ConnectionsModeSetSend( esp8266ConnectionsMode_t mode );

esp8266Status_t esp8266ConnectionsModeSetResponse();
    // - The default mode is single connection mode.
    // - This mode can only be changed after all connections are disconnected, use:
    //   "AT+CIPCLOSE=5\r\n" // ID = 5 all connections will be closed
    // - If the TCP server is running, it must be deleted (AT+CIPSERVER=0)
    //   before the single connection mode is activated.
    // - Multiple connections can only be set when transparent transmission
    //   is disabled (AT+CIPMODE=0).

// Deletes/Creates TCP server. ------------------------------------------------

// "AT+CIPSERVER=1,80\r\n"
esp8266Status_t esp8266CreateTCPServerSend( int port );

esp8266Status_t esp8266CreateTCPServerResponse();
    // - A TCP server can only be created when multiple connections are
    //   activated (AT+CIPMUX=1).
    // - A server monitor will automatically be created when the TCP server is
    //   created.
    // - When a client is connected to the server, it will take up one
    //   connection and be assigned an ID.
    // - Use ESP8266_SERVER_DEFAULT_PORT for port number = 333

// "AT+CIPSERVER=0\r\n"
esp8266Status_t esp8266DeleteTCPServerSend();

esp8266Status_t esp8266DeleteTCPServerResponse(); 

// Gets the local IP address. -------------------------------------------------

// "AT+CIFSR\r\n"
esp8266Status_t esp8266LocalIPAddressGetSend();
                                          
esp8266Status_t esp8266LocalIPAddressGetResponse( char* softAP_IPaddress,
                                                  char* station_IPaddress ); 

// Gets the connection status. ------------------------------------------------

// "AT+CIPSTATUS\r\n"
esp8266Status_t esp8266ConnectionStatusGetSend();

esp8266Status_t esp8266ConnectionStatusGetResponse(
    esp8266ConnectionStatus_t* result );

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
);

esp8266Status_t esp8266SendTCPOrSSLDataResponse();

// "AT+CIPSEND=[<link ID>,]<length>[,<remote IP>,<remote port>]\r\n"
esp8266Status_t esp8266SendUDPDataSend(
    int linkID, // ID of the connection (0~4), for multiple connections.
                // (-1 = single conection = ESP8266_SINGLE_CONNECTION).
    int length,          // Data length, MAX: 2048 bytes.
    char remoteIP[16],   // Remote IP can be set in UDP transmission.
    char remotePort[10], // Remote port can be set in UDP transmission.
    char* data           // Data to send (String NULL terminated).
);

esp8266Status_t esp8266SendUDPDataResponse();

// Closes TCP/UDP/SSL connection. ---------------------------------------------

// "AT+CIPCLOSE=" + linkID + "\r\n"
esp8266Status_t esp8266CloseConnectionSend( int linkID ); 
    // linkID: ID number of connections to be closed;
    // when ID = 5, all connections will be closed.
    // Remember to use ESP8266_CLOSE_ALL_CONNECTIONS for linkID=5

esp8266Status_t esp8266CloseConnectionResponse(); 

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
);

esp8266Status_t esp8266EstablishTCPConnectionResponse();

// Establishes UDP transmission -----------------------------------------------

// AT+CIPSTART="UDP","192.168.101.110",1000,1002,2
esp8266Status_t esp8266EstablishUDPTransmissionSend( 
   int linkID, // ID of network connection (0~4), used for multiple connections.
               // (-1 = single conection = ESP8266_SINGLE_CONNECTION).
   char* remoteIP,    // String parameter indicating the remote IP address.
   int remotePort,    // The remote port number.
   int UDP_localPort, // The local UDP port number.
   esp8266UDP_mode_t UDP_mode // Fixed or Changeable
);

esp8266Status_t esp8266EstablishUDPTransmissionResponse();

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
);

esp8266Status_t esp8266EstablishSSLConnectionResponse(); 

//=====[#include guards - end]=================================================

#endif // _ESP8266_H_
