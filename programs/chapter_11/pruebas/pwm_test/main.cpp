//=====[Libraries]=============================================================

#include "mbed.h"
#include "arm_book_lib.h"

//=====[Defines]===============================================================

//=====[Declaration and intitalization of public global objects]===============

DigitalOut led1(PF_7);
PwmOut led2(PF_9);
DigitalOut led3(PG_1);

Ticker tickerMain;
tick_t tickRateMSMain = 1;

Serial uartUsb(USBTX, USBRX);

//=====[Declaration and intitalization of public global variables]=============

delay_t signalTime1;

int onTime = 2;
int offTime = 8;

volatile tick_t tickCounter;

//=====[Declarations (prototypes) of public functions]=========================

void tickerCallbackMain( void );

//=====[Main function, the program entry point after power on or reset]========

int main()
{

    tickInit(1);

    delayInit( &signalTime1, onTime );

    tickerMain.attach( tickerCallbackMain, ((float) tickRateMSMain) / 1000.0 );

    led2.period(0.01f);
    led2.pulsewidth(0.002f);

    while (true) {

        if( delayRead( &signalTime1 ) ) {
            if (led1.read() == ON) {
                delayWrite( &signalTime1, offTime );
                led1 = OFF;
            } else {
                delayWrite( &signalTime1, onTime );
                led1 = ON;
            }
        }

    }
}

//=====[Implementations of public functions]===================================

void tickerCallbackMain( void )   // Before SysTick_Handler
{
    tickCounter++;
    if (led3.read() == ON) {
        if( tickCounter > onTime ) {
            tickCounter = 1;
            led3 = OFF;
        }
    } else {
        if( tickCounter > offTime ) { 
            tickCounter = 1;
            led3 = ON;
        }
    }
}