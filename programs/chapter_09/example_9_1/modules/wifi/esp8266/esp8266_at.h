//=====[#include guards - begin]===============================================

#ifndef _ESP8266_H_
#define _ESP8266_H_

//=====[Libraries]=============================================================

//=====[Declaration of public defines]=========================================

#define ESP8266_BAUDRATE                 115200

//=====[Declaration of public data types]======================================

typedef enum{   
    ESP8266_IDLE,
    ESP8266_PROCESSING_AT_COMMAND,
}esp8266State_t;

typedef enum{
    ESP8266_AT_SENDED,
    ESP8266_AT_NOT_SENDED,

    ESP8266_AT_PENDING_RESPONSE,
    ESP8266_AT_RESPONSED,
    ESP8266_AT_TIMEOUT_WAITING_RESPONSE,
}esp8266Status_t;

//=====[Declarations (prototypes) of public functions]=========================

void esp8266UartInit( int baudrate );
bool esp8266UartByteRead( char* receivedByte );
void esp8266UartByteWrite( char sendedByte );
void esp8266UartStringWrite( char const* str );

void esp8266Init();

esp8266Status_t esp8266TestATSend();
esp8266Status_t esp8266TestATResponse();

//=====[#include guards - end]=================================================

#endif // _ESP8266_H_
