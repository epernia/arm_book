//=====[Libraries]=============================================================

#include <arm_book_lib.h>
#include <mbed.h>

#include "sapi.h"     // <= sAPI header

#include "esp8266_module.h"

/*==================[macros and definitions]=================================*/

#define PC_SERIAL_COM_BAUD_RATE   115200

#define COMMAND_TO_TEST   esp8266CloseConnection(ESP8266_CLOSE_ALL_CONNECTIONS)

// Tested Ok:
//   esp8266TestAT()
//   esp8266Reset()
//   esp8266WiFiModeSet(ESP8266_WIFI_MODE_STATION)
//   esp8266WiFiModeSet(ESP8266_WIFI_MODE_SOFT_AP)
//   esp8266WiFiModeSet(ESP8266_WIFI_MODE_SOFT_AP_STA)
//   esp8266DisconnectFromAP()
//   esp8266ConnectionsModeSet(ESP8266_SIGNLE_CONNECTION)
//   esp8266ConnectionsModeSet(ESP8266_MULTIPLE_CONNECTIONS)
//   esp8266CreateTCPServer(ESP8266_SERVER_DEFAULT_PORT)
//   esp8266CreateTCPServer(80)


// Test fail sometimes:
//   esp8266CreateTCPServer(ESP8266_SERVER_DEFAULT_PORT)
//   esp8266CreateTCPServer(80)
//      TODO: Parece que andan estos 2 solo cuando antes llamo a
//            esp8266ConnectionsModeSet(ESP8266_MULTIPLE_CONNECTIONS)
//            Ver que devuelve antes porque me da que sale por timeout
//   esp8266CloseConnection(ESP8266_CLOSE_ALL_CONNECTIONS) 
//      TODO: Parece que anda solo cuando el server esta creado
//            Ver que devuelve antes porque me da que sale por timeout

/*==================[internal data definition]===============================*/

Serial uartUsb( USBTX, USBRX );

//=====[Main function, the program entry point after power on or reset]========

int main()
{
    uartUsb.baud(115200);    
    esp8266Init();

    esp8266Status_t esp8266Status;

    uartUsb.printf( "Prueba de comando que devuelve solo OK\r\n" );

    while( true ) {
        esp8266Status = COMMAND_TO_TEST;

        // Si no lo recibe indica que salio de la funcion
        // waitForReceiveStringOrTimeoutBlocking  por timeout.
        if( esp8266Status == ESP8266_RESPONSE_TIMEOUT ) {
            uartUsb.printf( "   Salio por timeout.\r\n" );

            delay(1000);
            esp8266Status = COMMAND_TO_TEST;
        }

        // Si recibe el string almacenado en miTexto indica que llego el
        // mensaje esperado.
        if( esp8266Status == ESP8266_OK ) {
            uartUsb.printf( "Llego: OK\r\n" );

            delay(1000);
            esp8266Status = COMMAND_TO_TEST;
        }
   }
}

