//=====[Libraries]=============================================================

#include "mbed.h"
#include "arm_book_lib.h"
#include "sapi.h"

//=====[Defines]===============================================================

//=====[Declaration and intitalization of public global objects]===============

DigitalIn period1Button(PF_9);
DigitalIn period2Button(PF_8);
DigitalIn period3Button(PF_7);

DigitalOut led1(LED1);
DigitalOut led2(LED2);
DigitalOut led3(LED3);

Serial uartUsb(USBTX, USBRX);

//=====[Declaration and intitalization of public global variables]=============

int onTime1 = 200;
int offTime1 = 800;

int onTime2 = 20;
int offTime2 = 80;

int onTime3 = 2;
int offTime3 = 8;

delay_t signalTime1;
delay_t signalTime2;
delay_t signalTime3;

//=====[Declarations (prototypes) of public functions]=========================

//=====[Main function, the program entry point after power on or reset]========

int main()
{
    tickInit(1);

    delayInit( &signalTime1, onTime );
    delayInit( &signalTime2, 20 );
    delayInit( &signalTime3, 2 );

    led1 = ON;
    delay(1000);
    led1 = OFF;
    delay(1000);
    led1 = ON;

    period1Button.mode(PullUp);
    period2Button.mode(PullUp);
    period3Button.mode(PullUp);

    while (true) {

        if( delayRead( &signalTime1 ) ) {
            if (led1.read() == ON) {
                delayWrite( &signalTime1, offTime1 );
                led1 = OFF;
            } else {
                delayWrite( &signalTime1, onTime1 );
                led1 = ON;
            }
        }

        if( delayRead( &signalTime2 ) ) {
            if (led2.read() == ON) {
                delayWrite( &signalTime2, offTime2 );
                led2 = OFF;
            } else {
                delayWrite( &signalTime2, onTime2 );
                led2 = ON;
            }
        }

        if( delayRead( &signalTime3 ) ) {
            if (led3.read() == ON) {
                delayWrite( &signalTime3, offTime3 );
                led3 = OFF;
            } else {
                delayWrite( &signalTime3, onTime3 );
                led3 = ON;
            }
        }

        if ( !period1Button ) {
            onTime = 200;
            offTime = 800;
        }

        if ( !period2Button ) {
            onTime = 20;
            offTime = 80;
        }

        if ( !period3Button ) {
            onTime = 2;
            offTime = 8;
        }
    }
}

//=====[Implementations of public functions]===================================