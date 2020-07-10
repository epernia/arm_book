//=====[Libraries]=============================================================

#include "mbed.h"
#include "arm_book_lib.h"

//=====[Defines]===============================================================

#define NUMBER_OF_KEYS                           4
#define STRING_MAX_LENGTH                       30
#define BLINKING_TIME_GAS_ALARM               1000
#define BLINKING_TIME_OVER_TEMP_ALARM          500
#define BLINKING_TIME_GAS_AND_OVER_TEMP_ALARM  100
#define LM35_SAMPLE_TIME                       100
#define NUMBER_OF_AVG_SAMPLES                   10
#define OVER_TEMP_LEVEL                         50
#define TIME_INCREMENT_MS                       10
#define DEBOUNCE_BUTTON_TIME_MS                 40

//=====[Declaration of public data types]======================================

typedef enum{
   BUTTON_UP,
   BUTTON_DOWN,
   BUTTON_FALLING,
   BUTTON_RISING
} buttonState_t;

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
AnalogIn lm35(A1);

//=====[Declaration and intitalization of public global variables]=============

bool alarmState       = OFF;
bool incorrectCode    = false;
bool overTempDetector = OFF;

int numberOfIncorrectCodes            = 0;
int buttonBeingCompared               = 0;
int codeSequence[NUMBER_OF_KEYS]      = { 1, 1, 0, 0 };
int buttonsPressed[NUMBER_OF_KEYS]    = { 0, 0, 0, 0 };
int accumulatedTimeAlarm              = 0;
int accumulatedTimeLm35               = 0;
int lm35SampleIndex                   = 0;
int accumulatedDebounceButtonTime        = 0;
int numberOfenterButtonReleasedEvents = 0;

char receivedChar = '\0';
char bleReceivedString[STRING_MAX_LENGTH];

bool alarmLastTransmittedState = OFF;
bool gasLastTransmittedState   = OFF;
bool tempLastTransmittedState  = OFF;
bool ICLastTransmittedState    = OFF;
bool SBLastTransmittedState    = OFF;
bool gasDetectorState          = OFF;
bool overTempDetectorState     = OFF;

float potentiometerReading      = 0.0;
float lm35ReadingsMovingAverage = 0.0;
float lm35AvgReadingsArray[NUMBER_OF_AVG_SAMPLES];
float lm35TempC                 = 0.0;

buttonState_t enterButtonState;

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
                                         const char* elementName );

void bleGetTheSmartphoneButtonsState( const char* buttonName, int index );

float celsiusToFahrenheit( float tempInCelsiusDegrees );
float analogReadingScaledWithTheLM35Formula( float analogReading );

void shiftLm35AvgReadingsArray();

void debounceButtonInit();
void debounceButtonUpdate();
void enterButtonReleasedEvent();

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
    debounceButtonInit();
}

void outputsInit()
{
    alarmLed = OFF;
    incorrectCodeLed = OFF;
    systemBlockedLed = OFF;
}

void alarmActivationUpdate()
{
    int i = 0;

    delay(TIME_INCREMENT_MS);

    accumulatedTimeLm35 = accumulatedTimeLm35 + TIME_INCREMENT_MS;

    if ( accumulatedTimeLm35 >= LM35_SAMPLE_TIME ) {
        if ( lm35SampleIndex < NUMBER_OF_AVG_SAMPLES ) {
            lm35AvgReadingsArray[lm35SampleIndex] = lm35.read() / NUMBER_OF_AVG_SAMPLES;
            lm35ReadingsMovingAverage = lm35ReadingsMovingAverage + 
                                        lm35AvgReadingsArray[lm35SampleIndex];
            lm35SampleIndex++;
        } else {
            lm35ReadingsMovingAverage = lm35ReadingsMovingAverage - 
                                        lm35AvgReadingsArray[0];

            shiftLm35AvgReadingsArray();

            lm35AvgReadingsArray[NUMBER_OF_AVG_SAMPLES-1] = 
                lm35.read() / NUMBER_OF_AVG_SAMPLES;

            lm35ReadingsMovingAverage =             
                lm35ReadingsMovingAverage +
                lm35AvgReadingsArray[NUMBER_OF_AVG_SAMPLES-1];
                
            lm35TempC = analogReadingScaledWithTheLM35Formula(
                            lm35ReadingsMovingAverage );
        }
        accumulatedTimeLm35 = 0;
    }

    if ( lm35TempC > OVER_TEMP_LEVEL ) {
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
        accumulatedTimeAlarm = accumulatedTimeAlarm + TIME_INCREMENT_MS;
	
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
        debounceButtonUpdate();
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

        case 'c':
        case 'C':
            uartUsb.printf( "Temperature: %.2f °C\r\n", lm35TempC );
            break;

        case 'f':
        case 'F':
            uartUsb.printf( "Temperature: %.2f °F\r\n", 
				celsiusToFahrenheit( lm35TempC ) );
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
    uartUsb.printf( "Press 'P' or 'p' to get potentiometer reading\r\n" );
	uartUsb.printf( "Press 'f' or 'F' to get lm35 reading in Fahrenheit\r\n" );
	uartUsb.printf( "Press 'c' or 'C' to get lm35 reading in Celsius\r\n\r\n" );
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
                                         const char* elementName )
{
    if ( lastTransmittedState != currentState ) {
        uartBle.printf("%s", elementName);
        if ( currentState ) {
            uartBle.printf("_ON\r\n");
        } else {
            uartBle.printf("_OFF\r\n");
        }
    }
}

void bleGetTheSmartphoneButtonsState( const char* buttonName, int index )
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

float analogReadingScaledWithTheLM35Formula( float analogReading )
{
    return ( analogReading * 3.3 / 0.01 );
}

float celsiusToFahrenheit( float tempInCelsiusDegrees )
{
    return ( tempInCelsiusDegrees * 9.0 / 5.0 + 32.0 );
}

void shiftLm35AvgReadingsArray()
{
    int i = 0;
    for( i=1; i<NUMBER_OF_AVG_SAMPLES; i++ ) {
        lm35AvgReadingsArray[i-1] = lm35AvgReadingsArray[i];
    }
    lm35AvgReadingsArray[NUMBER_OF_AVG_SAMPLES-1] = 0.0;
}

void debounceButtonInit()
{
    if( enterButton ) {
        enterButtonState = BUTTON_DOWN;        
    } else {
        enterButtonState = BUTTON_UP;
    }
}

void debounceButtonUpdate()
{
    switch( enterButtonState ){

        case BUTTON_UP:
            if( enterButton ){
                enterButtonState = BUTTON_FALLING;
                accumulatedDebounceButtonTime = 0;
            }
        break;

        case BUTTON_FALLING:
            if( accumulatedDebounceButtonTime >= DEBOUNCE_BUTTON_TIME_MS ) {
                if( enterButton ){
                    enterButtonState = BUTTON_DOWN;
                } else{
                    enterButtonState = BUTTON_UP;
                }
            }
            accumulatedDebounceButtonTime = accumulatedDebounceButtonTime + 
                                         TIME_INCREMENT_MS;
        break;

        case BUTTON_DOWN:
            if( !enterButton ){
                enterButtonState = BUTTON_RISING;
                accumulatedDebounceButtonTime = 0;
            }
        break;

        case BUTTON_RISING:
            if( accumulatedDebounceButtonTime >= DEBOUNCE_BUTTON_TIME_MS ) {
                if( !enterButton ){
                    enterButtonState = BUTTON_UP;
                    enterButtonReleasedEvent();
                } else{
                    enterButtonState = BUTTON_DOWN;
                }
            }
            accumulatedDebounceButtonTime = accumulatedDebounceButtonTime + 
                                         TIME_INCREMENT_MS;
        break;

        default:
            debounceButtonInit();
        break;
    }
}

void enterButtonReleasedEvent()
{
    if( incorrectCodeLed ) {
        numberOfenterButtonReleasedEvents++;
        if( numberOfenterButtonReleasedEvents >= 2 ) {
            incorrectCodeLed = OFF;
            numberOfenterButtonReleasedEvents = 0;
        }
    } else {
        if ( alarmState ) {
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
    }
}
