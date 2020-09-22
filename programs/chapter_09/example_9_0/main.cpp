#include <arm_book_lib.h>
#include <mbed.h>

#define ESP8266_BAUD_RATE      115200
#define UART_USB_BAUD_RATE     115200

Serial uartEsp8266( D42, D41 );
Serial uartUsb(USBTX, USBRX);

int main()
{
    char receivedCharUsb;
    char receivedCharEsp;

    uartEsp8266.baud(ESP8266_BAUD_RATE);
    uartUsb.baud(UART_USB_BAUD_RATE);

    uartUsb.printf("AT COMMANDS test\r\n");

    while( true ) {

        if( uartUsb.readable() ) {
            receivedCharUsb = uartUsb.getc();
            if ( receivedCharUsb == '\r' ) {
                uartEsp8266.printf("\r\n");
            } else {
                uartEsp8266.putc( receivedCharUsb );
            }
        }

        if ( uartEsp8266.readable() ) {
            receivedCharEsp = uartEsp8266.getc();
            uartUsb.putc( receivedCharEsp );
        }
    }
}