//=====[Libraries]=============================================================

#include <arm_book_lib.h>
#include <mbed.h>

#include "sapi.h"     // <= sAPI header
#include "string.h"

/*==================[macros and definitions]=================================*/

#define PC_SERIAL_COM_BAUD_RATE   115200

/*==================[internal data definition]===============================*/

Serial uartUsb( USBTX, USBRX );

//=====[Main function, the program entry point after power on or reset]========

int main()
{
    uartUsb.baud(115200);

    tickInit(1); // 1ms tick
    parserUartInit(115200);

    parser_t parser;
    parserStatus_t parserStatus;
    parserInit( &parser, "OK\r\n", strlen("OK\r\n"), 100 );
    parserStart( &parser ); 

    uartUsb.printf( "\r\nParser running\r\n" );

    // Envío AT para que me conteste el modulo y parsear el resultado
    parserUartStringWrite( "AT\r\n" );

    int count = 0;

    while( true ) {
        parserStatus = parserPatternMatchOrTimeout( &parser );

        // Si no lo recibe indica que salio de la funcion
        // waitForReceiveStringOrTimeoutBlocking  por timeout.
        if( parserStatus == PARSER_TIMEOUT ) {
            uartUsb.printf( "\r\nSalio por timeout.\r\n" );

            delay(1000);
            parserStart( &parser ); // Relanzo el parser
            parserUartStringWrite( "AT\r\n" );
        }

        // Si recibe el string almacenado en miTexto indica que llego el
        // mensaje esperado.
        if( parserStatus == PARSER_PATTERN_MATCH ) {
            uartUsb.printf( "\r\nLlego: OK\r\n" );

            delay(1000);
            parserStart( &parser ); // Relanzo el parser
            parserUartStringWrite( "AT\r\n" );

            if( count >= 2 ){
                parserStop( &parser );
                uartUsb.printf( "\r\nParser stop\r\n" );
            }
            count++;
        }
   }
}