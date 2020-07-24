//=====[Libraries]=============================================================

#include "mbed.h"
#include "arm_book_lib.h"
#include <fsm_matrix_keypad.h>

//=====[Defines]===============================================================

#define NUMBER_OF_KEYS                           4
#define STRING_MAX_LENGTH                       30
#define BLINKING_TIME_GAS_ALARM               1000
#define BLINKING_TIME_OVER_TEMP_ALARM          500
#define BLINKING_TIME_GAS_AND_OVER_TEMP_ALARM  100
#define LM35_SAMPLE_TIME                       100
#define NUMBER_OF_AVG_SAMPLES                   10
#define OVER_TEMP_LEVEL                         50
#define DEBOUNCE_BUTTON_TIME_MS                 40
#define MAX_NUMBER_OF_EVENTS                   100
#define MAX_NUMBER_OF_CHARACTERS                15

//=====[Declaration of public data types]======================================

typedef enum {
    BUTTON_UP,
    BUTTON_DOWN,
    BUTTON_FALLING,
    BUTTON_RISING
} buttonState_t;

typedef struct systemEvent {
    time_t seconds;
    char typeOfEvent[MAX_NUMBER_OF_CHARACTERS];
} systemEvent_t;

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

AnalogIn potentiometer(A0);
AnalogIn lm35(A1);

//=====[Declaration and intitalization of public global variables]=============

char codeSequence[NUMBER_OF_KEYS]   = { '1', '8', '0', '5' };
char buttonsPressed[NUMBER_OF_KEYS] = { '0', '0', '0', '0' };
int matrixKeypadCodeIndex = 0;

bool alarmState       = OFF;
bool incorrectCode    = false;
bool overTempDetector = OFF;

int numberOfIncorrectCodes            = 0;
int keyBeingCompared                  = 0;
int accumulatedTimeAlarm              = 0;
int accumulatedTimeLm35               = 0;
int lm35SampleIndex                   = 0;
int eventsIndex                       = 0;

char receivedChar = '\0';
char buffer[32];

bool alarmLastState = OFF;
bool gasLastState   = OFF;
bool tempLastState  = OFF;
bool ICLastState    = OFF;
bool SBLastState    = OFF;
bool gasDetectorState          = OFF;
bool overTempDetectorState     = OFF;

float potentiometerReading      = 0.0;
float lm35ReadingsMovingAverage = 0.0;
float lm35AvgReadingsArray[NUMBER_OF_AVG_SAMPLES];
float lm35TempC                 = 0.0;

int accumulatedDebounceButtonTime     = 0;
int numberOfEnterButtonReleasedEvents = 0;
buttonState_t enterButtonState;

struct tm RTCTime;
time_t timeAux;

systemEvent_t arrayOfStoredEvents[MAX_NUMBER_OF_EVENTS];

time_t seconds;

//=====[Declarations (prototypes) of public functions]=========================

void inputsInit();
void outputsInit();

void alarmActivationUpdate();
void alarmDeactivationUpdate();

void uartTask();
void availableCommands();
bool areEqual();
void checkStateChangeTask();

void checkStateChange( bool lastState,
        bool currentState,
        const char* elementName );

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
        checkStateChangeTask();
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

        case 's':
        case 'S':
            int RTCTimeAux;
            uartUsb.printf("Enter the current year (YYYY): ");
            uartUsb.scanf("%d",&RTCTimeAux);
            RTCTime.tm_year = RTCTimeAux - 1900;
            uartUsb.printf("%d\r\n",RTCTimeAux);

            uartUsb.printf("Enter the current month (1-12): ");
            uartUsb.scanf("%d",&RTCTimeAux);
            RTCTime.tm_mon = RTCTimeAux - 1;
            uartUsb.printf("%d\r\n",RTCTimeAux);

            uartUsb.printf("Enter the current day (1-31): ");
            uartUsb.scanf("%d",&RTCTime.tm_mday);
            uartUsb.printf("%d\r\n",RTCTime.tm_mday);

            uartUsb.printf("Enter the current hour (0-24): ");
            uartUsb.scanf("%d",&RTCTime.tm_hour);
            uartUsb.printf("%d\r\n",RTCTime.tm_hour);

            uartUsb.printf("Enter the current minutes (0-59): ");
            uartUsb.scanf("%d",&RTCTime.tm_min);
            uartUsb.printf("%d\r\n",RTCTime.tm_min);

            uartUsb.printf("Enter the current seconds (0-59): ");
            uartUsb.scanf("%d",&RTCTime.tm_sec);
            uartUsb.printf("%d\r\n",RTCTime.tm_sec);

            while ( uartUsb.readable() ) {
                uartUsb.getc();
            }

            RTCTime.tm_isdst = -1;

            set_time( mktime( &RTCTime ) );
            break;

        case 't':
        case 'T':
            timeAux = time(NULL);
            uartUsb.printf("Date and Time = %s", ctime(&timeAux));
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
    uartUsb.printf( "Press 'P' or 'p' to get potentiometer reading\r\n" );
    uartUsb.printf( "Press 'f' or 'F' to get lm35 reading in Fahrenheit\r\n" );
    uartUsb.printf( "Press 'c' or 'C' to get lm35 reading in Celsius\r\n" );
    uartUsb.printf( "Press 's' or 'S' to set the time\r\n" );
    uartUsb.printf( "Press 't' or 'T' to get the time\r\n" );
    uartUsb.printf( "Press 'e' or 'E' to get the stored events\r\n\r\n" );
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

void checkStateChangeTask()
{
    checkStateChange( alarmLastState,alarmState, "ALARM" );
    alarmLastState = alarmState;

    checkStateChange( gasLastState,gasDetector, "GAS_DET" );
    gasLastState = gasDetector;

    checkStateChange( tempLastState,overTempDetector, "OVER_TEMP" );
    tempLastState = overTempDetector;

    checkStateChange( ICLastState,incorrectCodeLed, "LED_IC" );
    ICLastState = incorrectCodeLed;

    checkStateChange( SBLastState,systemBlockedLed, "LED_SB" );
    SBLastState = systemBlockedLed;
}                                                                              

void checkStateChange( bool lastState,
        bool currentState,
        const char* elementName )
{
    char typeOfEventAux[MAX_NUMBER_OF_CHARACTERS];

    if ( lastState != currentState ) {
        typeOfEventAux[0] = 0;
        strncat( typeOfEventAux, elementName, strlen( elementName ) );
        if ( currentState ) {
            strncat( typeOfEventAux, "_ON", strlen("_ON") );
        } else {
            strncat( typeOfEventAux, "_OFF", strlen("_OFF") );
        }

        arrayOfStoredEvents[eventsIndex].seconds = time(NULL);
        strcpy( arrayOfStoredEvents[eventsIndex].typeOfEvent,typeOfEventAux );

        if ( eventsIndex < MAX_NUMBER_OF_EVENTS ) {
            eventsIndex++;
        } else {
            eventsIndex = 0;
        }
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
    switch( enterButtonState ) {

    case BUTTON_UP:
        if( enterButton ) {
            enterButtonState = BUTTON_FALLING;
            accumulatedDebounceButtonTime = 0;
        }
        break;

    case BUTTON_FALLING:
        if( accumulatedDebounceButtonTime >= DEBOUNCE_BUTTON_TIME_MS ) {
            if( enterButton ) {
                enterButtonState = BUTTON_DOWN;
            } else {
                enterButtonState = BUTTON_UP;
            }
        }
        accumulatedDebounceButtonTime = accumulatedDebounceButtonTime +
                                        TIME_INCREMENT_MS;
        break;

    case BUTTON_DOWN:
        if( !enterButton ) {
            enterButtonState = BUTTON_RISING;
            accumulatedDebounceButtonTime = 0;
        }
        break;

    case BUTTON_RISING:
        if( accumulatedDebounceButtonTime >= DEBOUNCE_BUTTON_TIME_MS ) {
            if( !enterButton ) {
                enterButtonState = BUTTON_UP;
                enterButtonReleasedEvent = true;
            } else {
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
