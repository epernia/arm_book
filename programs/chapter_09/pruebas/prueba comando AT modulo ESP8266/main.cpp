//=====[Libraries]=============================================================

#include <arm_book_lib.h>
#include <mbed.h>

#include "sapi.h"     // <= sAPI header

#include "esp8266_module.h"

/*==================[macros and definitions]=================================*/

#define PC_SERIAL_COM_BAUD_RATE   115200

/*==================[internal data definition]===============================*/

Serial uartUsb( USBTX, USBRX );

//=====[Main function, the program entry point after power on or reset]========

int main()
{
    uartUsb.baud(115200);

    uartUsb.printf( "Prueba de enviar AT\r\n" );
    tickInit(1); // 1ms tick
    esp8266Init();

    esp8266Status_t esp8266Status;

    // Envio AT para que me conteste el modulo y parsear el resultado
    uartUsb.printf( " Envio AT para que me conteste el modulo y parsear el resultado\r\n" );

    while( true ) {
        esp8266Status = esp8266TestAT();

        // Si no lo recibe indica que salio de la funcion
        // waitForReceiveStringOrTimeoutBlocking  por timeout.
        if( esp8266Status == ESP8266_RESPONSE_TIMEOUT ) {
            uartUsb.printf( "\r\nSalio por timeout.\r\n" );

            delay(1000);
            esp8266Status = esp8266TestAT();
        }

        // Si recibe el string almacenado en miTexto indica que llego el
        // mensaje esperado.
        if( esp8266Status == ESP8266_OK ) {
            uartUsb.printf( "\r\nLlego: OK\r\n" );

            delay(1000);
            esp8266Status = esp8266TestAT();
        }
   }
}