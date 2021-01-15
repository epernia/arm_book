#include "mbed.h"
#include "arm_book_lib.h"

int main()
{
    DigitalIn b1(BUTTON1);
    DigitalIn d2(D2);
    DigitalIn d3(D3);
    DigitalIn d4(D4);
    DigitalIn d5(D5);
    DigitalIn d6(D6);
    DigitalIn d7(D7);
    DigitalOut led1(LED1);
    DigitalOut led2(LED2);
    DigitalOut led3(LED3);

    b1.mode(PullDown);
    d2.mode(PullDown);
    d3.mode(PullDown);
    d4.mode(PullDown);
    d5.mode(PullDown);
    d6.mode(PullDown);
    d7.mode(PullDown);
    led1 = OFF;
    led2 = OFF;
    led3 = OFF;

    while (true) {
        led1 = b1 || d2 || d3;
        led2 = b1 || d4 || d5;
        led3 = b1 || d6 || d7;
    }
}