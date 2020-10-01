//=====[#include guards - begin]===============================================

#ifndef _ESP8266_H_
#define _ESP8266_H_

//=====[Libraries]=============================================================

//=====[Declaration of public defines]=========================================

#define ESP8266_BAUDRATE                 115200

//=====[Declaration of public data types]======================================

// Global status
typedef enum{   
    ESP8266_IDLE,                        // Waiting for interactions
    ESP8266_PROCESSING_AT_COMMAND,       // Sending AT command to ESP8266
}esp8266State_t;

// Module status
typedef enum{
    ESP8266_AT_SENDED,                   // Module already send the command.  
    ESP8266_AT_NOT_SENDED,               // Not sended.  

    ESP8266_AT_PENDING_RESPONSE,         // Waiting module response.  
    ESP8266_AT_RESPONSED,                // Module already response.
    ESP8266_AT_TIMEOUT_WAITING_RESPONSE, // Time-out waiting for a response.
}esp8266Status_t;

//=====[Declarations (prototypes) of public functions]=========================

// UART used for the ESP8266 --------------------------------------------------

void esp8266UartInit( int baudrate );
bool esp8266UartByteRead( char* receivedByte );
void esp8266UartByteWrite( char sendedByte );
void esp8266UartStringWrite( char const* str );

// FSM Initialization and Update ----------------------------------------------

void esp8266Init();

// Tests AT startup. ----------------------------------------------------------

// "AT\r\n"
esp8266Status_t esp8266TestATSend();
esp8266Status_t esp8266TestATResponse();

//=====[#include guards - end]=================================================

#endif // _ESP8266_H_