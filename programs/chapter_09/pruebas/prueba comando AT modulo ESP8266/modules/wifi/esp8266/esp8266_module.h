//=====[#include guards - begin]===============================================

#ifndef _ESP8266_H_
#define _ESP8266_H_

//=====[Libraries]=============================================================

#include <arm_book_lib.h>
#include <mbed.h>

#include "sapi_delay.h"

#include <cstring>

//=====[Declaration of public defines]=========================================

#define ESP8266_BAUDRATE            115200
#define ESP8266_SINGLE_CONNECTION   -1 // "AT+CIPSTART="

//=====[Declaration of public data types]======================================

// Global status
typedef enum{    
    ESP8266_PENDING_RESPONSE = 0,   // Waiting module response.
    ESP8266_RESPONSE_TIMEOUT = -1,  // Time-out waiting for a response.

    ESP8266_OK               = 1,   // AT command valid response.
    ESP8266_ERR              = -10, // AT command valid response.
    ESP8266_ALREADY_CONNECT  = -11, // AT command valid response.
    ESP8266_BUSY             = -12, // AT command valid response.
}esp8266Status_t;

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

// Initialization -------------------------------------------------------------

esp8266Status_t esp8266Init();

// Tests AT startup. ----------------------------------------------------------

// "AT\r\n"
esp8266Status_t esp8266TestAT(); 

// Restarts the ESP8266 module. -----------------------------------------------

// "AT+RST\r\n"
esp8266Status_t esp8266Reset();

// Sets the Wi-Fi mode of ESP32 (Station/AP/Station+AP). ----------------------

// "AT+CWMODE=3\r\n"
esp8266Status_t esp8266WiFiModeSet( esp8266WiFiMode_t mode ); 

// Query the current Wi-Fi mode of ESP32 (Station/AP/Station+AP). -------------

// "AT+CWMODE?"
esp8266Status_t esp8266WiFiModeGet( esp8266WiFiMode_t* responseMode );

// Lists available APs. -------------------------------------------------------
 
//TODO: It can be improbed
// "AT+CWLAP\r\n"
esp8266Status_t esp8266ListAPs( char* listOfAPs, int listOfAPsMaxLen );

// Disconnects from the AP. ---------------------------------------------------

// "AT+CWQAP\r\n"
esp8266Status_t esp8266DisconnectFromAP();

// Connects to an AP. ---------------------------------------------------------

// Set the AP to which the ESP32 Station needs to be connected.
// AT+CWJAP=<ssid>,<pwd>
esp8266Status_t esp8266ConnectToAP( char const* ssid, 
                                    char const* pwd );

// AT+CWJAP=<ssid>,<pwd>[,<bssid>]                        
esp8266Status_t esp8266ConnectToAPWithMAC( char const* ssid, 
                                           char const* pwd , 
                                           char const* bssid );

// Query the AP to which the ESP32 Station is already connected. --------------

// AT+CWJAP?
esp8266Status_t esp8266WhichAPIsConnected( char* response, 
                                           int responseMaxLen );

// Configures the multiple connections mode. ----------------------------------

// "AT+CIPMUX?\r\n"
esp8266Status_t esp8266ConnectionsModeGet( esp8266ConnectionsMode_t* respose );

// "AT+CIPMUX=1\r\n"
esp8266Status_t esp8266ConnectionsModeSet( esp8266ConnectionsMode_t mode );

// Deletes/Creates TCP server. ------------------------------------------------

// "AT+CIPSERVER=1,80\r\n"
esp8266Status_t esp8266CreateTCPServer( int port );

// "AT+CIPSERVER=0\r\n"
esp8266Status_t esp8266DeleteTCPServer( int port ); 

// Gets the local IP address. -------------------------------------------------

// "AT+CIFSR\r\n"
esp8266Status_t esp8266GetLocalIPAddress( char* softAP_IPaddress,
                                          char* station_IPaddress ); 

// Gets the connection status. ------------------------------------------------

// "AT+CIPSTATUS\r\n"
esp8266Status_t esp8266GetConnectionStatus(esp8266ConnectionStatus_t* result);

// Sends data. ----------------------------------------------------------------

// "AT+CIPSEND=[<link ID>,]<length>[,<remote IP>,<remote port>]\r\n"
esp8266Status_t esp8266SendTCPOrSSLData(
    int linkID, // ID of the connection (0~4), for multiple connections.
                // (-1 = single conection = ESP8266_SINGLE_CONNECTION).
    int length, // Data length, MAX: 2048 bytes.
    char* data  // Data to send (String NULL terminated).
);

// "AT+CIPSEND=[<link ID>,]<length>[,<remote IP>,<remote port>]\r\n"
esp8266Status_t esp8266SendUDPData(
    int linkID, // ID of the connection (0~4), for multiple connections.
                // (-1 = single conection = ESP8266_SINGLE_CONNECTION).
    int length,          // Data length, MAX: 2048 bytes.
    char remoteIP[16],   // Remote IP can be set in UDP transmission.
    char remotePort[10], // Remote port can be set in UDP transmission.
    char* data           // Data to send (String NULL terminated).
);

// Closes TCP/UDP/SSL connection. ---------------------------------------------

// "AT+CIPCLOSE=" + linkID + "\r\n"
esp8266Status_t esp8266CloseConnection( int linkID ); 
    // linkID: ID number of connections to be closed;
    // when ID = 5, all connections will be closed.

// Establishes TCP connection -------------------------------------------------

// AT+CIPSTART="TCP","iot.espressif.cn",8000
// AT+CIPSTART="TCP","192.168.101.110",1000
esp8266Status_t esp8266EstablishTCPConnection( 
   int linkID, // ID of network connection (0~4), used for multiple connections.
               // (-1 = single conection = ESP8266_SINGLE_CONNECTION).
   char* remoteIP, // String parameter indicating the remote IP address.
   int remotePort, // The remote port number.
   int TCP_keepAlive // Optional. Detection time interval when TCP is kept
                     // alive; this function is disabled by default.
                     // - 0: disable TCP keep-alive.
                     // - 1~7200: detection time interval; unit: second (s)
);

// Establishes UDP transmission -----------------------------------------------

// AT+CIPSTART="UDP","192.168.101.110",1000,1002,2
esp8266Status_t esp8266EstablishUDPTransmission( 
   int linkID, // ID of network connection (0~4), used for multiple connections.
               // (-1 = single conection = ESP8266_SINGLE_CONNECTION).
   char* remoteIP,    // String parameter indicating the remote IP address.
   int remotePort,    // The remote port number.
   int UDP_localPort, // The local UDP port number.
   esp8266UDP_mode_t UDP_mode // Fixed or Changeable
);

// Establishes SSL connection. ------------------------------------------------

// AT+CIPSTART="SSL","iot.espressif.cn",8443
esp8266Status_t esp8266EstablishSSLConnection( 
   int linkID, // ID of network connection (0~4), used for multiple connections
               // (-1 = single conection = ESP8266_SINGLE_CONNECTION).
   char* remoteIP,   // String parameter indicating the remote IP address.
   int remotePort,   // The remote port number.
   int TCP_keepAlive // Optional. Detection time interval when TCP is kept
                     // alive; this function is disabled by default.
                     // - 0: disable TCP keep-alive.
                     // - 1~7200: detection time interval; unit: second (s)
);

//=====[#include guards - end]=================================================

#endif // _ESP8266_H_