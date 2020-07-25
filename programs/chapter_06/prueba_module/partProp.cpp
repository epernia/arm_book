#include "partProp.h"

Serial uartUsb(USBTX, USBRX);

void uartTask()
{
    if( uartUsb.readable() ) {
        char receivedChar = uartUsb.getc();
        if ( receivedChar == '1') {
                uartUsb.printf( "The alarm is OFF\r\n");
        } else {
            availableCommands();
        }
    }
}

void availableCommands()
{
    uartUsb.printf( "Available command: 1\r\n" );
    uartUsb.printf( "Press '1' to get the alarm state\r\n\r\n" );
}