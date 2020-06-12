//=====[Libraries]=============================================================

#include "mbed.h"
#include "arm_book_lib.h"

//=====[Defines]===============================================================

#define NUMBER_OF_KEYS     4
#define STRING_MAX_LENGTH 30

//=====[Declaration and intitalization of public global objects]===============

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
Serial uartBle(D1, D0);

//=====[Declaration and intitalization of public global variables]=============

bool alarmState    = OFF;
bool incorrectCode = false;

int numberOfIncorrectCodes = 0;
int buttonBeingCompared    = 0;
int codeSequence[NUMBER_OF_KEYS]   = { 1, 1, 0, 0 };
int buttonsPressed[NUMBER_OF_KEYS] = { 0, 0, 0, 0 };

char receivedChar = '\0';
char bleReceivedString[STRING_MAX_LENGTH];

bool alarmLastTransmittedState = OFF;
bool gasLastTransmittedState   = OFF;
bool tempLastTransmittedState  = OFF;
bool ICLastTransmittedState    = OFF;
bool SBLastTransmittedState    = OFF;

bool aButtonLastTransmittedState = OFF;
bool bButtonLastTransmittedState = OFF;
bool cButtonLastTransmittedState = OFF;
bool dButtonLastTransmittedState = OFF;

//=====[Declarations (prototypes) of public functions]=========================

void inputsInit();
void outputsInit();

void alarmActivationUpdate();
void alarmDeactivationUpdate();

void uartTask();
void availableCommands();
bool areEqual();
void bleTask();

void bleSendElementStateToTheSmartphone( bool lastTransmittedState, 
                                         bool currentState, 
                                         char* elementName );

void bleSendButtonStateToTheSmartphone( bool lastTransmittedState, 
                                        bool currentState, 
                                        char* buttonName );

//=====[Main function, the program entry point after power on or reset]========

int main()
{
    inputsInit();
    outputsInit();
    while (true) {
        alarmActivationUpdate();
        alarmDeactivationUpdate();
        uartTask();
        bleTask();
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
            buttonsPressed[0] = aButton;
            buttonsPressed[1] = bButton;
            buttonsPressed[2] = cButton;
            buttonsPressed[3] = dButton;
            if ( areEqual() ) {
                alarmState = OFF;
                numberOfIncorrectCodes = 0;
            } else {
                incorrectCodeLed = ON;
                numberOfIncorrectCodes++;
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
                uartUsb.printf( "The alarmLed is activated\r\n");
            } else {
                uartUsb.printf( "The alarmLed is not activated\r\n");
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

        case '5':
            uartUsb.printf( "Please enter new code sequence\r\n" );
            uartUsb.printf( "First enter 'A', then 'B', then 'C', and " );
            uartUsb.printf( "finally 'D' button\r\n" );
            uartUsb.printf( "In each case type 1 for pressed or 0 for not " );
            uartUsb.printf( "pressed\r\n" );
            uartUsb.printf( "For example, for 'A' = pressed, 'B' = pressed," );
            uartUsb.printf( " 'C' = not pressed," );
            uartUsb.printf( "'D' = not pressed, enter '1', then '1', " );
            uartUsb.printf( "then '0', and finally '0'\r\n\r\n" );

            for ( buttonBeingCompared = 0; 
                  buttonBeingCompared < NUMBER_OF_KEYS; 
                  buttonBeingCompared++) {

                receivedChar = uartUsb.getc();

                if ( receivedChar == '1' ) {
                    codeSequence[buttonBeingCompared] = 1;
                } else if ( receivedChar == '0' ) {
                    codeSequence[buttonBeingCompared] = 0;
                }
            }

            uartUsb.printf( "New code generated\r\n\r\n" );
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
    uartUsb.printf( "Press '4' to enter the code sequence\r\n" );
    uartUsb.printf( "Press '5' to enter a new code\r\n\r\n" );
}

bool areEqual()
{
    int i;

    for (i = 0; i < NUMBER_OF_KEYS; i++) {
        if (codeSequence[i] != buttonsPressed[i]) {
            return false;
        }
    }

    return true;
}

void bleTask()
{
    bleSendElementStateToTheSmartphone( alarmLastTransmittedState, 
                                        alarmState, "ALARM" );
    alarmLastTransmittedState = alarmState;

    bleSendElementStateToTheSmartphone( gasLastTransmittedState, 
                                        gasDetector, "GAS_DET" );
    gasLastTransmittedState = gasDetector;

    bleSendElementStateToTheSmartphone( tempLastTransmittedState, 
                                        overTempDetector, "OVER_TEMP" );
    tempLastTransmittedState = overTempDetector;

    bleSendElementStateToTheSmartphone( ICLastTransmittedState, 
                                        incorrectCodeLed, "LED_IC" );
    ICLastTransmittedState = incorrectCodeLed;

    bleSendElementStateToTheSmartphone( SBLastTransmittedState, 
                                        systemBlockedLed, "LED_SB" );
    SBLastTransmittedState = systemBlockedLed;


    bleSendButtonStateToTheSmartphone( aButtonLastTransmittedState, 
                                       aButton, "A" );
    aButtonLastTransmittedState = aButton;

    bleSendButtonStateToTheSmartphone( bButtonLastTransmittedState, 
                                       bButton, "B" );
    bButtonLastTransmittedState = bButton;

    bleSendButtonStateToTheSmartphone( cButtonLastTransmittedState, 
                                       cButton, "C" );
    cButtonLastTransmittedState = cButton;

    bleSendButtonStateToTheSmartphone( dButtonLastTransmittedState, 
                                       dButton, "D" );
    dButtonLastTransmittedState = dButton;


    if( uartBle.readable() ) {
        uartBle.scanf("%s", bleReceivedString);

        if ( strcmp(bleReceivedString, "A_PRESSED") == 0)  {
            buttonsPressed[0] = 1;
            uartUsb.printf("Button 'A' has been pressed ");
            uartUsb.printf("in the smartphone application\r\n");
        }

        if ( strcmp(bleReceivedString, "B_PRESSED") == 0 ) {
            buttonsPressed[1] = 1;
            uartUsb.printf("Button 'B' has been pressed ");
            uartUsb.printf("in the smartphone application\r\n");
        }

        if ( strcmp(bleReceivedString, "C_PRESSED") == 0 ) {
            buttonsPressed[2] = 1;
            uartUsb.printf("Button 'C' has been pressed ");
            uartUsb.printf("in the smartphone application\r\n");
        }

        if ( strcmp(bleReceivedString, "D_PRESSED") == 0 ) {
            buttonsPressed[3] = 1;
            uartUsb.printf("Button 'D' has been pressed ");
            uartUsb.printf("in the smartphone application\r\n");
        }

        if ( strcmp(bleReceivedString, "ENTER_PRESSED") == 0 ) {
            uartUsb.printf("Button 'Enter' has been pressed ");
            uartUsb.printf("in the smartphone application\r\n");
        }

        if ( strcmp(bleReceivedString, "ENTER_RELEASED") == 0 ) {
            uartUsb.printf("Button 'Enter' has been released ");
            uartUsb.printf("in the smartphone application\r\n");
        }

        while ( uartBle.readable() ) {
            uartBle.getc();
        }
    } 
    
}

void bleSendElementStateToTheSmartphone( bool lastTransmittedState, 
                                         bool currentState, 
                                         char* elementName )
{
    if ( lastTransmittedState != currentState ) {
        uartBle.printf(elementName);
        if ( currentState ) {
            uartBle.printf("_ON\r\n");
        } else {
            uartBle.printf("_OFF\r\n");
        }
    }
}

void bleSendButtonStateToTheSmartphone( bool lastTransmittedState, 
                                        bool currentState, 
                                        char* buttonName )
{
    if ( lastTransmittedState != currentState ) {
        uartBle.printf(buttonName);
        if ( currentState ) {
            uartBle.printf("_PRESSED\r\n");
        } else {
            uartBle.printf("_RELEASED\r\n");
        }
    }
}