#include "mbed.h"
#include "arm_book_lib.h"

int main()
{
    DigitalIn gasDetector(D2);

    DigitalOut alarmLed(LED1);

    gasDetector.mode(PullDown);

    alarmLed = OFF;

    while (true) {

        if ( gasDetector == ON ) {
            alarmLed = ON;
        }
        if ( gasDetector == OFF ) {
            alarmLed = OFF;
        }
    }
}