#include "mbed.h"
#include "arm_book_lib.h"

//=====[Defines]===============================================================

#define NUMBER_OF_KEYS 4

//=====[Declaration and initialization of public global objects]===============

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

Serial uartUsb(USBTX, USBRX);

//=====[Declaration and initialization of public global variables]=============

bool alarmState    = OFF;
bool incorrectCode = false;

int numberOfIncorrectCodes = 0;
int buttonBeingCompared    = 0;
int codeSequence[NUMBER_OF_KEYS] = { 1, 1, 0, 0 };

char receivedChar = '\0';

//=====[Declarations (prototypes) of public functions]=========================

void inputsInit();
void outputsInit();

void alarmActivationUpdate();
void alarmDeactivationUpdate();

void uartTask();
void availableCommands();

//=====[Main function, the program entry point after power on or reset]========

int main()
{
    inputsInit();
    outputsInit();
    while (true) {
        alarmActivationUpdate();
        alarmDeactivationUpdate();
        uartTask();
    }
}         

//=====[Implementations of public functions]===================================

void inputsInit()
{
    gasDetector.mode(PullDown);
    overTempDetector.mode(PullDown);
    aButton.mode(PullDown);
    bButton.mode(PullDown);
    cButton.mode(PullDown);
    dButton.mode(PullDown);
}

void outputsInit()
{
    alarmLed = OFF;
    incorrectCodeLed = OFF;
    systemBlockedLed = OFF;
}

void alarmActivationUpdate()
{
    if ( gasDetector || overTempDetector ) {
        alarmState = ON;
    }
    alarmLed = alarmState;
}

void alarmDeactivationUpdate()
{
    if ( numberOfIncorrectCodes < 5 ) {
        if ( aButton && bButton && cButton && dButton && !enterButton ) {
            incorrectCodeLed = OFF;
        }
        if ( enterButton && !incorrectCodeLed && alarmState ) {
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

void uartTask()
{
    if( uartUsb.readable() ) {
        char receivedChar = uartUsb.getc();
        switch (receivedChar) {
        case '1':
            if ( alarmState ) {
                uartUsb.printf( "The alarm is activated\r\n");
            } else {
                uartUsb.printf( "The alarm is not activated\r\n");
            }
            break;

        case '2':
            if ( gasDetector ) {
                uartUsb.printf( "Gas is being detected\r\n");
            } else {
                uartUsb.printf( "Gas is not being detected\r\n");
            }
            break;

        case '3':
            if ( overTempDetector ) {
                uartUsb.printf( "Temperature is above the maximum level\r\n");
            } else {
                uartUsb.printf( "Temperature is below the maximum level\r\n");
            }
            break;
            
        case '4':
            uartUsb.printf( "Please enter the code sequence.\r\n" );
            uartUsb.printf( "First enter 'A', then 'B', then 'C', and " ); 
            uartUsb.printf( "finally 'D' button\r\n" );
            uartUsb.printf( "In each case type 1 for pressed or 0 for " );
            uartUsb.printf( "not pressed\r\n" );
            uartUsb.printf( "For example, for 'A' = pressed, " );
            uartUsb.printf( "'B' = pressed, 'C' = not pressed, ");
            uartUsb.printf( "'D' = not pressed, enter '1', then '1', " );
            uartUsb.printf( "then '0', and finally '0'\r\n\r\n" );

            incorrectCode = false;

            for ( buttonBeingCompared = 0; 
                  buttonBeingCompared < NUMBER_OF_KEYS; 
                  buttonBeingCompared++) {

                receivedChar = uartUsb.getc();

                if ( receivedChar == '1' ) {
                    if ( codeSequence[buttonBeingCompared] != 1 ) {
                        incorrectCode = true;
                    }
                } else if ( receivedChar == '0' ) {
                    if ( codeSequence[buttonBeingCompared] != 0 ) {
                        incorrectCode = true;
                    }
                }
            }

            if ( incorrectCode == false ) {
                uartUsb.printf( "The code is correct\r\n\r\n" );
                alarmState = OFF;
                incorrectCodeLed = OFF;
                numberOfIncorrectCodes = 0;
            } else {
                uartUsb.printf( "The code is incorrect\r\n\r\n" );
                incorrectCodeLed = ON;
                numberOfIncorrectCodes = numberOfIncorrectCodes + 1;
            }                
            break;

        default:
            availableCommands();
            break;

        }
    }
}

void availableCommands()
{
    uartUsb.printf( "Available commands:\r\n" );
    uartUsb.printf( "Press '1' to get the alarm state\r\n" );
    uartUsb.printf( "Press '2' for gas detector state\r\n" );
    uartUsb.printf( "Press '3' for over temperature detector state\r\n" );
    uartUsb.printf( "Press '4' to enter the code sequence\r\n\r\n" );
}