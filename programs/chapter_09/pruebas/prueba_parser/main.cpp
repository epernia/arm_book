//=====[Libraries]=============================================================

#include <arm_book_lib.h>
#include <mbed.h>

#include "sapi.h"     // <= sAPI header


/*==================[macros and definitions]=================================*/

#define PC_SERIAL_COM_BAUD_RATE   115200

/*==================[internal data definition]===============================*/

Serial uartUsb( USBTX, USBRX );

//=====[Main function, the program entry point after power on or reset]========

int main()
{
    char miTexto[] = "todo bien";

    parser_t parser;
    parserStatus_t parserStatus;

    tickInit(1); // 1ms tick
    uartUsb.baud(115200);

    uartUsb.printf( "Se espera a que el usuario escriba \"todo bien\",\r\n" );
    uartUsb.printf( "o sale por timeout (10 segundos) y vuelve a esperar\r\n" );
    uartUsb.printf( "a que se escriba el mensaje.\r\n" );

    parserInit( &parser,         // Instance 
                //&uartUsb,        // UART
                miTexto,         // String
                strlen(miTexto), // String lenght
                10000 );         // Timeout

    parserStart( &parser ); 
    uartUsb.printf( "\r\nParser running\r\n" );

    int count = 0;

    while( true ) {

        parserStatus = parserUpdate( &parser );

        // Si no lo recibe indica que salio de la funcion
        // waitForReceiveStringOrTimeoutBlocking  por timeout.
        if( parserStatus == PARSER_TIMEOUT ) {
            uartUsb.printf( "\r\nSalio por timeout\r\n" );
            parserStart( &parser ); // Relanzo el parser
        }

        // Si recibe el string almacenado en miTexto indica que llego el
        // mensaje esperado.
        if( parserStatus == PARSER_RECEIVED_OK ) {
            uartUsb.printf( "\r\nLlego el mensaje esperado\r\n" );
            parserStart( &parser ); // Relanzo el parser
            if( count >= 2 ){
                parserStop( &parser );
               uartUsb.printf( "\r\nParser stop\r\n" );
            }
            count++;
        }

   }

}