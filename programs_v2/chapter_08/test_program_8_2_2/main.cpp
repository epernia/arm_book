//=====[Libraries]=============================================================

#include "mbed.h"
#include "arm_book_lib.h"

//=====[Defines]===============================================================

//=====[Declaration and intitalization of public global objects]===============

DigitalIn period1Button(PF_9);
DigitalIn period2Button(PF_8);
DigitalIn period3Button(PF_7);

PwmOut led1(LED1);
PwmOut led2(LED2);
PwmOut led3(LED3);

//=====[Declaration and intitalization of public global variables]=============

float period1 = 1.0;
float onTime1 = 0.2;

float period2 = 0.10;
float onTime2 = 0.02;

float period3 = 0.010;
float onTime3 = 0.002;

//=====[Declarations (prototypes) of public functions]=========================

//=====[Main function, the program entry point after power on or reset]========

int main()
{
    period1Button.mode(PullUp);
    period2Button.mode(PullUp);
    period3Button.mode(PullUp);

    led1.period(period1);
    led1.pulsewidth(onTime1);

    led2.period(period1);
    led2.pulsewidth(onTime1);

    led3.period(period1);
    led3.pulsewidth(onTime1);

    while (true) {

        if ( !period1Button ) {
            period1 = 1.0;
            onTime1 = 0.2;
            
            led1.period(period1);
            led1.pulsewidth(onTime1);
            
            led2.period(period1);
            led2.pulsewidth(onTime1);

            led3.period(period1);
            led3.pulsewidth(onTime1);
        }

        if ( !period2Button ) {
            period1 = 0.10;
            onTime1 = 0.02;
            
            led1.period(period1);
            led1.pulsewidth(onTime1);
            
            led2.period(period1);
            led2.pulsewidth(onTime1);

            led3.period(period1);
            led3.pulsewidth(onTime1);
        }

        if ( !period3Button ) {
            period1 = 0.010;
            onTime1 = 0.002;
            
            led1.period(period1);
            led1.pulsewidth(onTime1);
            
            led2.period(period1);
            led2.pulsewidth(onTime1);

            led3.period(period1);
            led3.pulsewidth(onTime1);
        }
    }
}

//=====[Implementations of public functions]===================================