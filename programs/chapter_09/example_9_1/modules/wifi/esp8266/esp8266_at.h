//=====[#include guards - begin]===============================================

#ifndef _ESP8266_AT_H_
#define _ESP8266_AT_H_

//=====[Libraries]=============================================================

//=====[Declaration of public defines]=========================================

#define ESP8266_BAUD_RATE                  115200

//=====[Declaration of public data types]======================================

typedef enum{
    ESP8266_AT_SENT,
    ESP8266_AT_NOT_SENT,

    ESP8266_AT_RESPONSE_PENDING,
    ESP8266_AT_RESPONDED,
    ESP8266_AT_TIMEOUT,
}esp8266RequestResult_t;

//=====[Declarations (prototypes) of public functions]=========================

void esp8266UartInit( int baudRate );
bool esp8266UartByteRead( char* receivedByte );
void esp8266UartByteWrite( char sentByte );
void esp8266UartStringWrite( char const* str );

void esp8266Init();

esp8266RequestResult_t esp8266TestATSend();
esp8266RequestResult_t esp8266TestATResponse();

//=====[#include guards - end]=================================================

#endif // _ESP8266_H_
