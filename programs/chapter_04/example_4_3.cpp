//=====[Libraries]=============================================================

#include "mbed.h"
#include "arm_book_lib.h"

//=====[Defines]===============================================================

#define NUMBER_OF_KEYS                           4
#define STRING_MAX_LENGTH                       30
#define BLINKING_TIME_GAS_ALARM               1000
#define BLINKING_TIME_OVER_TEMP_ALARM          500
#define BLINKING_TIME_GAS_AND_OVER_TEMP_ALARM  100
#define POTENTIOMETER_OVER_TEMP_LEVEL            0.5

//=====[Declaration and intitalization of public global objects]===============

DigitalIn enterButton(BUTTON1);
DigitalIn gasDetector(D2);
DigitalIn aButton(D4);
DigitalIn bButton(D5);
DigitalIn cButton(D6);
DigitalIn dButton(D7);

DigitalOut alarmLed(LED1);
DigitalOut incorrectCodeLed(LED3);
DigitalOut systemBlockedLed(LED2);

Serial uartUsb(USBTX, USBRX);
Serial uartBle(D1, D0);

AnalogIn potentiometer(A0);

//=====[Declaration and intitalization of public global variables]=============

bool alarmState    = OFF;
bool incorrectCode = false;
bool overTempDetector = OFF;

int numberOfIncorrectCodes = 0;
int buttonBeingCompared    = 0;
int codeSequence[NUMBER_OF_KEYS]   = { 1, 1, 0, 0 };
int buttonsPressed[NUMBER_OF_KEYS] = { 0, 0, 0, 0 };
int accumulatedTimeAlarm = 0;

char receivedChar = '\0';
char bleReceivedString[STRING_MAX_LENGTH];

bool alarmLastTransmittedState = OFF;
bool gasLastTransmittedState   = OFF;
bool tempLastTransmittedState  = OFF;
bool ICLastTransmittedState    = OFF;
bool SBLastTransmittedState    = OFF;
bool gasDetectorState          = OFF;
bool overTempDetectorState     = OFF;

float potentiometerReading;

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

void bleGetTheSmartphoneButtonsState( char* buttonName, int index );

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
    potentiometerReading = potentiometer.read();

    if ( potentiometerReading > POTENTIOMETER_OVER_TEMP_LEVEL ) {
        overTempDetector = ON;
    } else {
        overTempDetector = OFF;
    }

    if( gasDetector) {
        gasDetectorState = ON;
        alarmState = ON;
    }
    if( overTempDetector ) {
        overTempDetectorState = ON;
        alarmState = ON;
    }
    if( alarmState ) { 
        delay(10);
        accumulatedTimeAlarm = accumulatedTimeAlarm + 10;
	
        if( gasDetectorState && overTempDetectorState ) {
            if( accumulatedTimeAlarm >= BLINKING_TIME_GAS_AND_OVER_TEMP_ALARM ) {
                accumulatedTimeAlarm = 0;
                alarmLed = !alarmLed;
            }
        } else if( gasDetectorState ) {
            if( accumulatedTimeAlarm >= BLINKING_TIME_GAS_ALARM ) {
                accumulatedTimeAlarm = 0;
                alarmLed = !alarmLed;
            }
        } else if ( overTempDetectorState ) {
            if( accumulatedTimeAlarm >= BLINKING_TIME_OVER_TEMP_ALARM  ) {
                accumulatedTimeAlarm = 0;
                alarmLed = !alarmLed;
            }
        }
    } else{
        alarmLed = OFF;
        gasDetectorState = OFF;
        overTempDetectorState = OFF;
    }
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

        case 'p':
        case 'P':
            potentiometerReading = potentiometer.read();
            uartUsb.printf( "Potentiometer: %.2f\r\n", potentiometerReading );
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
    uartUsb.printf( "Press '5' to enter a new code\r\n" );
    uartUsb.printf( "Press 'P' or 'p' to get potentiometer reading\r\n\r\n" );
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

    if( uartBle.readable() ) {
        uartBle.scanf("%s", bleReceivedString);

        bleGetTheSmartphoneButtonsState( "A", 0 );
        bleGetTheSmartphoneButtonsState( "B", 1 );
        bleGetTheSmartphoneButtonsState( "C", 2 );
        bleGetTheSmartphoneButtonsState( "D", 3 );

        bleGetTheSmartphoneButtonsState( "ENTER", -1 );

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

void bleGetTheSmartphoneButtonsState( char* buttonName, int index )
{
    char str[30];

    str[0] = 0;
    strncat( str, buttonName, strlen(buttonName) );
    strncat( str, "_PRESSED", strlen("_PRESSED") );

    if ( strcmp(bleReceivedString, str) == 0 )  {
        uartUsb.printf("Button '%s' has been pressed", buttonName );
        uartUsb.printf("in the smartphone application\r\n\r\n");
        if ( index >= 0 ) {
            buttonsPressed[index] = 1;
        }
    }

    str[0] = 0;
    strncat( str, buttonName, strlen(buttonName) );
    strncat( str, "_RELEASED", strlen("_RELEASED") );

    if ( strcmp(bleReceivedString, str) == 0 )  {
        if ( index >= 0 ) {
            buttonsPressed[index] = 0;
        }
        uartUsb.printf("Button '%s' has been released", buttonName );
        uartUsb.printf("in the smartphone application\r\n\r\n");
    }
}
