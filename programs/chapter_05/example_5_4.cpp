//=====[Libraries]=============================================================

#include "mbed.h"
#include "arm_book_lib.h"

//=====[Defines]===============================================================

#define NUMBER_OF_KEYS                           4
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
#define EVENT_MAX_STORAGE                      100
#define EVENT_NAME_MAX_LENGTH                   15

//=====[Declaration of public data types]======================================

typedef enum {
    MATRIX_KEYPAD_SCANNING,
    MATRIX_KEYPAD_DEBOUNCE,
    MATRIX_KEYPAD_KEY_HOLD_PRESSED
} matrixKeypadState_t;

typedef struct systemEvent {
    time_t seconds;
    char typeOfEvent[EVENT_NAME_MAX_LENGTH];
} systemEvent_t;

//=====[Declaration and intitalization of public global objects]===============

DigitalIn enterButton(BUTTON1);
DigitalIn gasDetector(D2);

DigitalOut alarmLed(LED1);
DigitalOut incorrectCodeLed(LED3);
DigitalOut systemBlockedLed(LED2);

Serial uartUsb(USBTX, USBRX);
Serial uartBle(D1, D0);

AnalogIn lm35(A1);

DigitalOut keypadRowPins[KEYPAD_NUMBER_OF_ROWS] = {D23, D22, D21, D20};
DigitalIn keypadColPins[KEYPAD_NUMBER_OF_COLS]  = {D19, D18, D17, D16};

//=====[Declaration and intitalization of public global variables]=============

bool alarmState       = OFF;
bool incorrectCode    = false;
bool overTempDetector = OFF;

int numberOfIncorrectCodes = 0;
int keyBeingCompared       = 0;
int accumulatedTimeAlarm   = 0;
int accumulatedTimeLm35    = 0;
int lm35SampleIndex        = 0;

char receivedChar = '\0';
char codeSequence[NUMBER_OF_KEYS]   = { '1', '8', '0', '5' };
char buttonsPressed[NUMBER_OF_KEYS] = { '0', '0', '0', '0' };

bool alarmLastState        = OFF;
bool gasLastState          = OFF;
bool tempLastState         = OFF;
bool ICLastState           = OFF;
bool SBLastState           = OFF;
bool gasDetectorState      = OFF;
bool overTempDetectorState = OFF;

float potentiometerReading      = 0.0;
float lm35ReadingsMovingAverage = 0.0;
float lm35AvgReadingsArray[NUMBER_OF_AVG_SAMPLES];
float lm35TempC                 = 0.0;

int numberOfEnterButtonReleasedEvents = 0;
buttonState_t enterButtonState;

int accumulatedDebounceMatrixKeypadTime = 0;
int matrixKeypadCodeIndex = 0;
char matrixKeypadLastKeyPressed = '\0';
char matrixKeypadIndexToCharArray[] = {
    '1', '2', '3', 'A',
    '4', '5', '6', 'B',
    '7', '8', '9', 'C',
    '*', '0', '#', 'D',
};
matrixKeypadState_t matrixKeypadState;

int eventsIndex            = 0;
systemEvent_t arrayOfStoredEvents[EVENT_MAX_STORAGE];

//=====[Declarations (prototypes) of public functions]=========================

void inputsInit();
void outputsInit();

void alarmActivationUpdate();
void alarmDeactivationUpdate();

void uartTask();
void availableCommands();

bool areEqual();

void eventLogUpdate();
void systemElementStateUpdate( bool lastState,
                               bool currentState,
                               const char* elementName );

float celsiusToFahrenheit( float tempInCelsiusDegrees );
float analogReadingScaledWithTheLM35Formula( float analogReading );
void shiftLm35AvgReadingsArray();

void matrixKeypadInit();
char matrixKeypadScan();
char matrixKeypadUpdate();

//=====[Main function, the program entry point after power on or reset]========

int main()
{
    inputsInit();
    outputsInit();
    matrixKeypadInit();
    while (true) {
        alarmActivationUpdate();
        alarmDeactivationUpdate();
        uartTask();
        eventLogUpdate();
        delay(TIME_INCREMENT_MS);
    }
}

//=====[Implementations of public functions]===================================

void inputsInit()
{
    gasDetector.mode(PullDown);
}

void outputsInit()
{
    alarmLed = OFF;
    incorrectCodeLed = OFF;
    systemBlockedLed = OFF;
}

void alarmActivationUpdate()
{
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
    } else {
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
            buttonsPressed[matrixKeypadCodeIndex] = keyReleased;
            if( matrixKeypadCodeIndex >= NUMBER_OF_KEYS ) {
                matrixKeypadCodeIndex = 0;
            } else {
                matrixKeypadCodeIndex++;
            }
        }
        if( keyReleased == '#' ) {
            if( incorrectCodeLed ) {
                numberOfEnterButtonReleasedEvents++;
                if( numberOfEnterButtonReleasedEvents >= 2 ) {
                    incorrectCodeLed = OFF;
                    numberOfEnterButtonReleasedEvents = 0;
                    matrixKeypadCodeIndex = 0;
                }
            } else {
                if ( alarmState ) {
                    if ( areEqual() ) {
                        alarmState = OFF;
                        numberOfIncorrectCodes = 0;
                        matrixKeypadCodeIndex = 0;
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
            uartUsb.printf( "Please enter the new four digits numeric code " );
            uartUsb.printf( "to deactivate the alarm.\r\n" );

            incorrectCode = false;

            for ( keyBeingCompared = 0;
                  keyBeingCompared < NUMBER_OF_KEYS;
                  keyBeingCompared++) {
                receivedChar = uartUsb.getc();
                uartUsb.printf( "*" );
                if ( codeSequence[keyBeingCompared] != receivedChar ) {
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
            uartUsb.printf( "Please enter the new four digits numeric code" );

            for ( keyBeingCompared = 0;
                  keyBeingCompared < NUMBER_OF_KEYS;
                  keyBeingCompared++) {
                codeSequence[keyBeingCompared] = uartUsb.getc();
                uartUsb.printf( "*" );
            }

            uartUsb.printf( "\r\nNew code configurated\r\n\r\n" );
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

        case 's':
        case 'S':
            struct tm rtcTime;
            int uartReceivedYear;
            uartUsb.printf("Enter the current year (YYYY): ");
            uartUsb.scanf("%d",&uartReceivedYear);
            rtcTime.tm_year = uartReceivedYear - 1900;
            uartUsb.printf("%d\r\n",uartReceivedYear);

            int uartReceivedMonth;
            uartUsb.printf("Enter the current month (1-12): ");
            uartUsb.scanf("%d",&uartReceivedMonth);
            rtcTime.tm_mon = uartReceivedMonth - 1;
            uartUsb.printf("%d\r\n",uartReceivedMonth);

            uartUsb.printf("Enter the current day (1-31): ");
            uartUsb.scanf("%d",&rtcTime.tm_mday);
            uartUsb.printf("%d\r\n",rtcTime.tm_mday);

            uartUsb.printf("Enter the current hour (0-24): ");
            uartUsb.scanf("%d",&rtcTime.tm_hour);
            uartUsb.printf("%d\r\n",rtcTime.tm_hour);

            uartUsb.printf("Enter the current minutes (0-59): ");
            uartUsb.scanf("%d",&rtcTime.tm_min);
            uartUsb.printf("%d\r\n",rtcTime.tm_min);

            uartUsb.printf("Enter the current seconds (0-59): ");
            uartUsb.scanf("%d",&rtcTime.tm_sec);
            uartUsb.printf("%d\r\n",rtcTime.tm_sec);

            while ( uartUsb.readable() ) {
                uartUsb.getc();
            }

            rtcTime.tm_isdst = -1;

            set_time( mktime( &rtcTime ) );
            break;

        case 't':
        case 'T':
            time_t epochSeconds;
            epochSeconds = time(NULL);
            uartUsb.printf("Date and Time = %s", ctime(&epochSeconds));
            break;

        case 'e':
        case 'E':
            for (int i = 0; i < eventsIndex; i++) {
                uartUsb.printf("Event = %s\r\n", arrayOfStoredEvents[i].typeOfEvent);
                uartUsb.printf("Date and Time = %s\r\n",
                               ctime(&arrayOfStoredEvents[i].seconds));
                uartUsb.printf("\r\n");
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
    uartUsb.printf( "Press '4' to enter the code sequence\r\n" );
    uartUsb.printf( "Press '5' to enter a new code\r\n" );
    uartUsb.printf( "Press 'f' or 'F' to get lm35 reading in Fahrenheit\r\n" );
    uartUsb.printf( "Press 'c' or 'C' to get lm35 reading in Celsius\r\n" );
    uartUsb.printf( "Press 's' or 'S' to set the date and time\r\n" );
    uartUsb.printf( "Press 't' or 'T' to get the date and time\r\n" );
    uartUsb.printf( "Press 'e' or 'E' to get the stored events\r\n" );
    uartUsb.printf( "\r\n" );
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

void eventLogUpdate()
{
    systemElementStateUpdate( alarmLastState,alarmState, "ALARM" );
    alarmLastState = alarmState;

    systemElementStateUpdate( gasLastState,gasDetector, "GAS_DET" );
    gasLastState = gasDetector;

    systemElementStateUpdate( tempLastState,overTempDetector, "OVER_TEMP" );
    tempLastState = overTempDetector;

    systemElementStateUpdate( ICLastState,incorrectCodeLed, "LED_IC" );
    ICLastState = incorrectCodeLed;

    systemElementStateUpdate( SBLastState,systemBlockedLed, "LED_SB" );
    SBLastState = systemBlockedLed;
}

void systemElementStateUpdate( bool lastState,
                               bool currentState,
                               const char* elementName )
{
    if ( lastState != currentState ) {
        
        char eventAndStateStr[EVENT_NAME_MAX_LENGTH];
        eventAndStateStr[0] = 0;
        strncat( eventAndStateStr, elementName, strlen(elementName) );
        if ( currentState ) {
            strncat( eventAndStateStr, "_ON", strlen("_ON") );
        } else {
            strncat( eventAndStateStr, "_OFF", strlen("_OFF") );
        }

        arrayOfStoredEvents[eventsIndex].seconds = time(NULL);
        strcpy( arrayOfStoredEvents[eventsIndex].typeOfEvent,eventAndStateStr );
        if ( eventsIndex < EVENT_MAX_STORAGE ) {
            eventsIndex++;
        } else {
            eventsIndex = 0;
        }

        uartUsb.printf("%s",eventAndStateStr);
        uartUsb.printf("\r\n");

        uartBle.printf("%s",eventAndStateStr);
        uartBle.printf("\r\n");
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

    switch( matrixKeypadState ) {

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
            } else {
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