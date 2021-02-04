#include <arm_book_lib.h>
#include <mbed.h>

#define ESP8266_BAUD_RATE      9600
#define UART_USB_BAUD_RATE     9600

Serial esp8266Uart( PD_5, PD_6 );
Serial uartUsb(USBTX, USBRX);

int main()
{
    char receivedCharUsb;
    char receivedCharEsp;
	
    esp8266Uart.baud(ESP8266_BAUD_RATE);
    uartUsb.baud(UART_USB_BAUD_RATE);

    while( true ) {

        if( uartUsb.readable() ) {
            receivedCharUsb = uartUsb.getc();	
			esp8266Uart.putc( receivedCharUsb );
        }

        if ( esp8266Uart.readable() ) {
            receivedCharEsp = esp8266Uart.getc();
            uartUsb.putc( receivedCharEsp );
		}
    }
}