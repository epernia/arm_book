// Example 5-3 SIN MODIFICAR LA PARTE DE BLUETOOTH


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
#define KEYPAD_NUMBER_OF_ROWS                    4
#define KEYPAD_NUMBER_OF_COLS                    4

//=====[Declaration of public data types]======================================

typedef enum{
   BUTTON_UP,
   BUTTON_DOWN,
   BUTTON_FALLING,
   BUTTON_RISING
} buttonState_t;

typedef enum{
    MATRIX_KEYPAD_SCANNING,
    MATRIX_KEYPAD_DEBOUNCE,
    MATRIX_KEYPAD_KEY_HOLD_PRESSED
} matrixKeypadState_t;

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

DigitalOut keypadRowPins[KEYPAD_NUMBER_OF_ROWS] = {D23, D22, D21, D20};
DigitalIn keypadColPins[KEYPAD_NUMBER_OF_COLS]  = {D19, D18, D17, D16};

//=====[Declaration and intitalization of public global variables]=============

bool alarmState       = OFF;
bool incorrectCode    = false;
bool overTempDetector = OFF;

int numberOfIncorrectCodes            = 0;
int buttonBeingCompared               = 0;
int accumulatedTimeAlarm              = 0;
int accumulatedTimeLm35               = 0;
int lm35SampleIndex                   = 0;

char receivedChar = '\0';
char bleReceivedString[STRING_MAX_LENGTH];
char codeSequence[NUMBER_OF_KEYS]   = { '1', '8', '0', '5' };
char buttonsPressed[NUMBER_OF_KEYS] = { '0', '0', '0', '0' };

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

int accumulatedDebounceButtonTime     = 0;
int numberOfEnterButtonReleasedEvents = 0;
buttonState_t enterButtonState;

int accumulatedDebounceMatrixKeypadTime = 0;
int codeMatrixKeypadIndex = 0;
char matrixKeypadLastKeyPressed = '\0';
char matrixKeypadIndexToCharArray[] = {
    '1', '2', '3', 'A',
    '4', '5', '6', 'B',
    '7', '8', '9', 'C',
    '*', '0', '#', 'D',
};
matrixKeypadState_t matrixKeypadState;

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
bool debounceButtonUpdate();

void matrixKeypadInit();
char matrixKeypadScan();
char matrixKeypadUpdate();

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
        delay(TIME_INCREMENT_MS);
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
    matrixKeypadInit();
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
        char keyReleased = matrixKeypadUpdate();        
        if( keyReleased != '\0' && keyReleased != '#' ) {
            buttonsPressed[codeMatrixKeypadIndex] = keyReleased;
            if( codeMatrixKeypadIndex >= NUMBER_OF_KEYS ) {
                codeMatrixKeypadIndex = 0;
            } else {
                codeMatrixKeypadIndex++;
            }
        }
        if( keyReleased == '#' ) {
            if( incorrectCodeLed ) {
                numberOfEnterButtonReleasedEvents++;
                if( numberOfEnterButtonReleasedEvents >= 2 ) {
                    incorrectCodeLed = OFF;
                    numberOfEnterButtonReleasedEvents = 0;
                    codeMatrixKeypadIndex = 0;
                }
            } else {
                if ( alarmState ) {
                    if ( areEqual() ) {
                        alarmState = OFF;
                        numberOfIncorrectCodes = 0;
                        codeMatrixKeypadIndex = 0;
                    } else {
                        incorrectCodeLed = ON;
                        numberOfIncorrectCodes++;
                    }
                }
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
            uartUsb.printf( "Please enter the 4 digits numeric code " );
            uartUsb.printf( "sequence to deactivate the alarm.\r\n" );

            incorrectCode = false;

            for ( buttonBeingCompared = 0; 
                  buttonBeingCompared < NUMBER_OF_KEYS; 
                  buttonBeingCompared++) {
                receivedChar = uartUsb.getc();
                uartUsb.printf( "*" );
                if ( codeSequence[buttonBeingCompared] != receivedChar ) {
                    incorrectCode = true;
                }
            }

            if ( incorrectCode == false ) {
                uartUsb.printf( "\r\nThe code is correct\r\n\r\n" );
                alarmState = OFF;
                incorrectCodeLed = OFF;
                numberOfIncorrectCodes = 0;
            } else {
                uartUsb.printf( "\r\nThe code is incorrect\r\n\r\n" );
                incorrectCodeLed = ON;
                numberOfIncorrectCodes = numberOfIncorrectCodes + 1;
            }                
            break;

        case '5':
            uartUsb.printf( "Please enter the 4 digits numeric new code " );
            uartUsb.printf( "sequence.\r\n" );

            for ( buttonBeingCompared = 0; 
                  buttonBeingCompared < NUMBER_OF_KEYS; 
                  buttonBeingCompared++) {
                codeSequence[buttonBeingCompared] = uartUsb.getc();
                uartUsb.printf( "*" );
            }

            uartUsb.printf( "\r\nNew code generated\r\n\r\n" );
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

bool debounceButtonUpdate()
{
    bool enterButtonReleasedEvent = false;
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
                    enterButtonReleasedEvent = true;
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
    return enterButtonReleasedEvent;
}

void matrixKeypadInit()
{
    matrixKeypadState = MATRIX_KEYPAD_SCANNING;
    int pinIndex = 0;
    for( pinIndex=0; pinIndex<KEYPAD_NUMBER_OF_COLS; pinIndex++ ) {
        (keypadColPins[pinIndex]).mode(PullUp);
    }
}

char matrixKeypadScan()
{
    int r = 0;
    int c = 0;
    int i = 0;

    for( r=0; r<KEYPAD_NUMBER_OF_ROWS; r++ ) {
   
        for( i=0; i<KEYPAD_NUMBER_OF_ROWS; i++ ) {
            keypadRowPins[i] = ON;
        }

        keypadRowPins[r] = OFF;
       
        for( c=0; c<KEYPAD_NUMBER_OF_COLS; c++ ) {                   
            if( keypadColPins[c] == OFF ) {
                return matrixKeypadIndexToCharArray[r*KEYPAD_NUMBER_OF_ROWS + c];
            }
        }
    }
    return '\0';
}

char matrixKeypadUpdate()
{
    char keyDetected = '\0';
    char keyReleased = '\0';

    switch( matrixKeypadState ){

        case MATRIX_KEYPAD_SCANNING:
            keyDetected = matrixKeypadScan();
            if( keyDetected != '\0' ) {
                matrixKeypadLastKeyPressed = keyDetected;
                accumulatedDebounceMatrixKeypadTime = 0;
                matrixKeypadState = MATRIX_KEYPAD_DEBOUNCE;
            }
        break;

        case MATRIX_KEYPAD_DEBOUNCE:
            if( accumulatedDebounceMatrixKeypadTime >= 
                DEBOUNCE_BUTTON_TIME_MS ) {
                keyDetected = matrixKeypadScan();
                if( keyDetected == matrixKeypadLastKeyPressed ) {
                    matrixKeypadState = MATRIX_KEYPAD_KEY_HOLD_PRESSED;
                } else{
                    matrixKeypadState = MATRIX_KEYPAD_SCANNING;
                }
            }
            accumulatedDebounceMatrixKeypadTime = 
                accumulatedDebounceMatrixKeypadTime + TIME_INCREMENT_MS;
        break;

        case MATRIX_KEYPAD_KEY_HOLD_PRESSED:
            keyDetected = matrixKeypadScan();
            if( keyDetected != matrixKeypadLastKeyPressed ) {             
                if( keyDetected == '\0' ) {
                    keyReleased = matrixKeypadLastKeyPressed;
                }
                matrixKeypadState = MATRIX_KEYPAD_SCANNING;   
            }
        break;

        default:
            matrixKeypadInit();
        break;
    }
    return keyReleased;
}
