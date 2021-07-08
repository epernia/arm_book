#include "mbed.h"
#include "arm_book_lib.h"

int main()
{
    DigitalIn b1(BUTTON1);

    b1.mode(PullDown);
   
    Serial uartUsb(USBTX, USBRX);

    bool pressed = false;
    
    uartUsb.printf( "Hello\r\n");

    while (true) {
        if (b1 && !pressed) {
            uartUsb.printf( "Button pressed\r\n");
            pressed = true;
        }
        
        if (!b1 && pressed) {
            uartUsb.printf( "Button released\r\n");
            pressed = false;
        }
        
        delay(50);
    }
}