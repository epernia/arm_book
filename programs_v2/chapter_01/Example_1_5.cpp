#include "mbed.h"
#include "arm_book_lib.h"

int main()
{
    DigitalIn enterButton(BUTTON1);
    DigitalIn gasDetector(D2);
    DigitalIn overTempDetector(D3);
    DigitalIn aButton(D4);
    DigitalIn bButton(D5);
    DigitalIn cButton(D6);
    DigitalIn dButton(D7);

    DigitalOut alarmLed(LED1);
    DigitalOut incorrectCodeLed(LED3);
    DigitalOut systemBlockedLed(LED2);

    gasDetector.mode(PullDown);
    overTempDetector.mode(PullDown);
    aButton.mode(PullDown);
    bButton.mode(PullDown);
    cButton.mode(PullDown);
    dButton.mode(PullDown);

    alarmLed = OFF;
    incorrectCodeLed = OFF;
    systemBlockedLed = OFF;

    bool alarmState = OFF;
    int numberOfIncorrectCodes = 0;

    while (true) {

        if ( gasDetector || overTempDetector ) {
            alarmState = ON;
        }

        alarmLed = alarmState;

        if ( numberOfIncorrectCodes < 5 ) {
            
            if ( aButton && bButton && cButton && dButton && !enterButton ) {
                incorrectCodeLed = OFF;
            }

            if ( enterButton && !incorrectCodeLed && alarmState) {
                if ( aButton && bButton && !cButton && !dButton ) {
                    alarmState = OFF;
                    numberOfIncorrectCodes = 0;
                } else {
                    incorrectCodeLed = ON;
                    numberOfIncorrectCodes = numberOfIncorrectCodes + 1;
                }
            }
        } else {
            systemBlockedLed = ON;
        }
    }
}