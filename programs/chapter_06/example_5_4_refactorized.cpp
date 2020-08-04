//=====[Libraries]=============================================================

#include "mbed.h"
#include "arm_book_lib.h"

//=====[Constants]=============================================================

#define CODE_NUMBER_OF_KEYS                      4
#define TEMPERATURE_C_LIMIT_ALARM             50.0
#define GAS_CONCENTRATION_LIMIT_ALARM          0.0 // TODO: Ver que valor de umbral poner

#define SIREN_BLINKING_TIME_GAS               1000
#define SIREN_BLINKING_TIME_OVER_TEMP          500
#define SIREN_BLINKING_TIME_GAS_AND_OVER_TEMP  100

#define LM35_SAMPLE_TIME                       100
#define LM35_NUMBER_OF_AVG_SAMPLES              10

#define SYSTEM_TIME_INCREMENT_MS                10

#define MATRIX_KEYPAD_NUMBER_OF_ROWS             4
#define MATRIX_KEYPAD_NUMBER_OF_COLS             4
#define DEBOUNCE_BUTTON_TIME_MS                 40

#define EVENT_LOG_MAX_STORAGE                  100
#define EVENT_LOG_NAME_MAX_LENGTH               15

//=====[Declaration of public data types]======================================

typedef enum {
    MATRIX_KEYPAD_SCANNING,
    MATRIX_KEYPAD_DEBOUNCE,
    MATRIX_KEYPAD_KEY_HOLD_PRESSED
} matrixKeypadState_t;

typedef struct systemEvent {
    time_t seconds;
    char typeOfEvent[EVENT_LOG_NAME_MAX_LENGTH];
} systemEvent_t;

//=====[Declaration and intitalization of public global objects]===============

DigitalIn gasDetector(D2);

DigitalOut alarmLed(LED1);
DigitalOut incorrectCodeLed(LED3);
DigitalOut systemBlockedLed(LED2);

Serial uartUsb(USBTX, USBRX);

Serial uartBle(D1, D0);

AnalogIn lm35(A1);

DigitalOut keypadRowPins[MATRIX_KEYPAD_NUMBER_OF_ROWS] = {D23, D22, D21, D20};
DigitalIn keypadColPins[MATRIX_KEYPAD_NUMBER_OF_COLS]  = {D19, D18, D17, D16};

//=====[Declaration and intitalization of public global variables]=============

bool gasDetected           = OFF;
bool overTempDetected      = OFF;

bool gasDetectorState      = OFF;
bool overTempDetectorState = OFF;

bool sirenState            = OFF;
bool incorrectCodeState    = OFF;
bool systemBlockedState    = OFF;

int numberOfIncorrectCodes = 0;
char codeSequence[CODE_NUMBER_OF_KEYS]   = { '1', '8', '0', '5' };

bool matrixKeypadCodeCompleteSaved = false;
int matrixKeypadCodeIndex          = 0;
char alarmCodeFromMatrixKeypad[CODE_NUMBER_OF_KEYS] = {'0','0','0','0'};

int pcSerialComCodeIndex = 0;
char alarmCodeFromPcSerialCom[CODE_NUMBER_OF_KEYS] = {'0','0','0','0'};

float lm35TempC = 0.0;
float lm35AvgReadingsArray[LM35_NUMBER_OF_AVG_SAMPLES];

matrixKeypadState_t matrixKeypadState;

bool sirenLastState = OFF;
bool gasLastState   = OFF;
bool tempLastState  = OFF;
bool ICLastState    = OFF;
bool SBLastState    = OFF;
int eventsIndex     = 0;
systemEvent_t arrayOfStoredEvents[EVENT_LOG_MAX_STORAGE];

//=====[Declarations (prototypes) of public functions]=========================

void smartHomeSystemInit();
void smartHomeSystemUpdate();

void fireAlarmInit();
void fireAlarmUpdate();
void fireAlarmActivationUpdate();
void fireAlarmDeactivationUpdate();
void fireAlarmDeactivate();
bool gasDetectorStateRead();
bool overTempDetectorStateRead();

void sirenInit();
bool sirenStateRead();
void sirenStateWrite( bool state );
void sirenIndicatorUpdate();

void userInterfaceInit();
void userInterfaceUpdate();
void userInterfaceMatrixKeypadUpdate();

void codeWrite( char* newCodeSequence );
bool codeMatch( char* codeToCompare );
bool codeFromMatrixKeypadMatch();
bool codeFromPcSerialComMatch();

bool incorrectCodeStateRead();
void incorrectCodeStateWrite( bool state );
void incorrectCodeIndicatorUpdate();

bool systemBlockedStateRead();
void systemBlockedStateWrite( bool state );
void systemBlockedIndicatorUpdate();


void smarphoneBleComWrite( const char* str );

char pcSerialComReadChar();
void pcSerialComWrite( const char* str );
void pcSerialComCommandUpdate();
bool pcSerialComCodeMatch();
void availableCommands();
void commandShowCurrentSirenState();
void commandShowCurrentGasDetectorState();
void commandShowCurrentOverTempDetectorState();
void commandEnterNewCode();
void commandShowCurrentTemperatureInCelsius();
void commandShowCurrentTemperatureInFahrenheit();
void commandSetDateAndTime();
void commandShowDateAndTime();
void commandShowStoredEvents();
void commandEnterCodeSequence();

void eventLogUpdate();
int eventLogNumberOfStoredEvents();
void eventLogReadStriangAtIndex( int index, char* str );
void eventLogWrite( bool currentState, const char* elementName );
void eventLogElementStateUpdate( bool lastState,
                                 bool currentState,
                                 const char* elementName );

char* dateAndTimeReadString();
void dateAndTimeWriteIndividualValues( int year, int month, int day, 
                                       int hour, int minute, int second );

void gasSensorInit( void );
void gasSensorUpdate( void );
float gasSensorRead();

void temperatureSensorInit( void );
void temperatureSensorUpdate( void );
float temperatureSensorReadCelsius( void );
float temperatureSensorReadFahrenheit( void );
float celsiusToFahrenheit( float tempInCelsiusDegrees );
float analogReadingScaledWithTheLM35Formula( float analogReading );
void shiftLm35AvgReadingsArray();

void matrixKeypadInit();
char matrixKeypadScan();
char matrixKeypadUpdate();

//=====[Main function, the program entry point after power on or reset]========

int main()
{
    smartHomeSystemInit();
    while (true) {
        smartHomeSystemUpdate();
    }
}

//=====[Implementations of public functions]===================================

void smartHomeSystemInit()
{
	sirenInit();
	userInterfaceInit();
    fireAlarmInit();
}

void smartHomeSystemUpdate()
{
    fireAlarmUpdate();	
    sirenIndicatorUpdate();
    userInterfaceUpdate();
    pcSerialComCommandUpdate();
    eventLogUpdate();
    delay(SYSTEM_TIME_INCREMENT_MS);
}

void userInterfaceInit()
{
    incorrectCodeLed = OFF;
    systemBlockedLed = OFF;
    matrixKeypadInit();
}

void fireAlarmInit()
{
    gasSensorInit();
    gasSensorInit();
    temperatureSensorInit();
}

void fireAlarmUpdate()
{
    fireAlarmActivationUpdate();
    fireAlarmDeactivationUpdate();
}

bool gasDetectorStateRead()
{
    return gasDetectorState;
}

bool overTempDetectorStateRead()
{
    return overTempDetectorState;
}



void sirenInit()
{
    alarmLed = OFF;
}

bool sirenStateRead()
{
    return sirenState;
}

void sirenStateWrite( bool state )
{
    sirenState = state;
}

void sirenIndicatorUpdate()
{
    static int accumulatedTimeAlarm = 0;
    accumulatedTimeAlarm = accumulatedTimeAlarm + SYSTEM_TIME_INCREMENT_MS;
    
    if( sirenState ) {
        if( gasDetected && overTempDetected ) {
            if( accumulatedTimeAlarm >= SIREN_BLINKING_TIME_GAS_AND_OVER_TEMP ) {
                accumulatedTimeAlarm = 0;
                alarmLed = !alarmLed;
            }
        } else if( gasDetected ) {
            if( accumulatedTimeAlarm >= SIREN_BLINKING_TIME_GAS ) {
                accumulatedTimeAlarm = 0;
                alarmLed = !alarmLed;
            }
        } else if ( overTempDetected ) {
            if( accumulatedTimeAlarm >= SIREN_BLINKING_TIME_OVER_TEMP  ) {
                accumulatedTimeAlarm = 0;
                alarmLed = !alarmLed;
            }
        }
    } else {
        alarmLed = OFF;
    }
}

bool incorrectCodeStateRead()
{
    return incorrectCodeState;
}

void incorrectCodeStateWrite( bool state )
{
    incorrectCodeState = state;
}

void incorrectCodeIndicatorUpdate()
{
    incorrectCodeLed = incorrectCodeStateRead();
}

bool systemBlockedStateRead()
{
    return systemBlockedState;
}

void systemBlockedStateWrite( bool state )
{
    systemBlockedState = state;
}

void systemBlockedIndicatorUpdate()
{
    systemBlockedLed = systemBlockedState;
}

void codeWrite( char* newCodeSequence )
{
    for ( int i = 0; i < CODE_NUMBER_OF_KEYS; i++) {
        codeSequence[i] = newCodeSequence[i];
    }
}

bool codeMatch( char* codeToCompare )
{
    int i;
    for (i = 0; i < CODE_NUMBER_OF_KEYS; i++) {
        if ( codeSequence[i] != codeToCompare[i] ) {
            return false;
        }
    }
    return true;
}

void fireAlarmDeactivate()
{
	sirenStateWrite(ON);
	
    overTempDetected       = OFF;
    gasDetected            = OFF;
    systemBlockedState     = OFF;
    incorrectCodeState     = OFF;
    numberOfIncorrectCodes = 0;
    matrixKeypadCodeIndex  = 0;
}

void fireAlarmActivationUpdate()
{
    temperatureSensorUpdate();
    gasSensorUpdate();

    //overTempDetectorState = temperatureSensorReadCelsius() > 
    //                        TEMPERATURE_C_LIMIT_ALARM;

    if ( overTempDetectorState ) {
        overTempDetected = ON;
        sirenStateWrite(ON);
    }

    gasDetectorState = gasSensorRead() > GAS_CONCENTRATION_LIMIT_ALARM;

    if ( gasDetectorState ) {
        gasDetected = ON;
        sirenStateWrite(ON);
    }
}

bool codeFromMatrixKeypadMatch()
{
    bool codeIsCorrect = false;    
    if( matrixKeypadCodeCompleteSaved ) {
        matrixKeypadCodeCompleteSaved = false;
        codeIsCorrect = codeMatch(alarmCodeFromMatrixKeypad);
        if( codeIsCorrect ) {
            uartUsb.printf( "Code is correct!!\r\n" );
        } else {
            uartUsb.printf( "Code is incorrect.\r\n" );
            incorrectCodeState = ON;
            numberOfIncorrectCodes++;
        }
    }
    return codeIsCorrect;
}

void fireAlarmDeactivationUpdate()
{
    if ( sirenStateRead() ) {
        if ( numberOfIncorrectCodes >= 5 ) {
            systemBlockedState = ON;
        }
        if ( codeFromMatrixKeypadMatch() ||
            codeFromPcSerialComMatch() ) {
            fireAlarmDeactivate();
        }
    }
}

void userInterfaceUpdate()
{
	userInterfaceMatrixKeypadUpdate();
    incorrectCodeIndicatorUpdate();
    systemBlockedIndicatorUpdate();
}

void userInterfaceMatrixKeypadUpdate()
{
    static int numberOfHaskKeyReleased = 0;
    char keyReleased = matrixKeypadUpdate();
    
    // Se me ocurrio que sea una sola funcion esta y la de uart que segun 
    // un parametro revise caracteres de una o la otra

    if( keyReleased != '\0' ) {
        uartUsb.printf( "%c\r\n", keyReleased );

        if( sirenStateRead() ) {

            if( keyReleased == '#' ) {
                numberOfHaskKeyReleased++;
                if( numberOfHaskKeyReleased >= 2 ) {
                    uartUsb.printf( "Double Press Hash: keypad code reset\r\n" );
                    numberOfHaskKeyReleased = 0;
                    matrixKeypadCodeIndex = 0;
                    incorrectCodeState = OFF;
                }
                return;
            }

            if( matrixKeypadCodeIndex < CODE_NUMBER_OF_KEYS ) {
                uartUsb.printf( "Start save matrix keypad code\r\n" ); 
                alarmCodeFromMatrixKeypad[matrixKeypadCodeIndex] = keyReleased;
                uartUsb.printf( "  index: %d\r\n", matrixKeypadCodeIndex );
                matrixKeypadCodeIndex++;      
            } else {
                if( !incorrectCodeState ) {
                    matrixKeypadCodeCompleteSaved = true;                    
                }
            }
        }
    }
}

bool codeFromPcSerialComMatch()
{
    return pcSerialComCodeMatch();
}

char* dateAndTimeReadString()
{
    time_t epochSeconds;
    epochSeconds = time(NULL);
    return ctime(&epochSeconds);    
}

void dateAndTimeWriteIndividualValues( int year, int month, int day, 
                                       int hour, int minute, int second )
{
    struct tm rtcTime;

    rtcTime.tm_year = year - 1900;
    rtcTime.tm_mon  = month - 1;
    rtcTime.tm_mday = day;
    rtcTime.tm_hour = hour;
    rtcTime.tm_min  = minute;
    rtcTime.tm_sec  = second;

    rtcTime.tm_isdst = -1;

    set_time( mktime( &rtcTime ) );
}

void gasSensorInit( void )
{
    gasDetector.mode(PullDown);
}

void gasSensorUpdate( void )
{
    return;
}

float gasSensorRead()
{
    return (float)gasDetector;
}

void temperatureSensorInit( void )
{
    return;
}

void temperatureSensorUpdate( void )
{
    static int accumulatedTimeLm35 = 0;
    static int lm35SampleIndex     = 0;
    static float lm35ReadingsMovingAverage = 0.0;

    accumulatedTimeLm35 = accumulatedTimeLm35 + SYSTEM_TIME_INCREMENT_MS;

    if ( accumulatedTimeLm35 >= LM35_SAMPLE_TIME ) {
        if ( lm35SampleIndex < LM35_NUMBER_OF_AVG_SAMPLES ) {
            lm35AvgReadingsArray[lm35SampleIndex] = lm35.read() / LM35_NUMBER_OF_AVG_SAMPLES;
            lm35ReadingsMovingAverage = lm35ReadingsMovingAverage +
                                        lm35AvgReadingsArray[lm35SampleIndex];
            lm35SampleIndex++;
        } else {
            lm35ReadingsMovingAverage = lm35ReadingsMovingAverage -
                                        lm35AvgReadingsArray[0];

            shiftLm35AvgReadingsArray();

            lm35AvgReadingsArray[LM35_NUMBER_OF_AVG_SAMPLES-1] =
                lm35.read() / LM35_NUMBER_OF_AVG_SAMPLES;

            lm35ReadingsMovingAverage =
                lm35ReadingsMovingAverage +
                lm35AvgReadingsArray[LM35_NUMBER_OF_AVG_SAMPLES-1];

            lm35TempC = analogReadingScaledWithTheLM35Formula(
                            lm35ReadingsMovingAverage );
        }
        accumulatedTimeLm35 = 0;
    }
}

float temperatureSensorReadCelsius( void )
{
    return lm35TempC;
}

float temperatureSensorReadFahrenheit( void )
{
    return celsiusToFahrenheit( lm35TempC );
}

void smarphoneBleComWrite( const char* str )
{
    uartBle.printf( "%s", str );
}

char pcSerialComReadChar()
{
	char receivedChar = '\0';
    if( uartUsb.readable() ) {
        receivedChar = uartUsb.getc();
    }
	return receivedChar;
}

void pcSerialComWrite( const char* str )
{
    uartUsb.printf( "%s", str );
}

void pcSerialComCommandUpdate()
{
	char receivedChar = pcSerialComReadChar();
    if( receivedChar != '\0' ) {
        switch (receivedChar) {
            case '1': commandShowCurrentSirenState(); break;
            case '2': commandShowCurrentGasDetectorState(); break;
            case '3': commandShowCurrentOverTempDetectorState(); break;
            case '4': commandEnterCodeSequence(); break;
            case '5': commandEnterNewCode(); break;
            case 'c': case 'C': commandShowCurrentTemperatureInCelsius(); break;
            case 'f': case 'F': commandShowCurrentTemperatureInFahrenheit(); break;
            case 's': case 'S': commandSetDateAndTime(); break;
            case 't': case 'T': commandShowDateAndTime(); break;
            case 'e': case 'E': commandShowStoredEvents(); break;
            default: availableCommands(); break;
        }
    }
}

bool pcSerialComCodeMatch()
{
    return false;
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

void commandShowCurrentSirenState()
{
    if ( sirenStateRead() ) {
        uartUsb.printf( "The alarmLed is activated\r\n");
    } else {
        uartUsb.printf( "The alarmLed is not activated\r\n");
    }
}

void commandShowCurrentGasDetectorState()
{
    if ( gasDetectorStateRead() ) {
        uartUsb.printf( "Gas is being detected\r\n");
    } else {
        uartUsb.printf( "Gas is not being detected\r\n");
    }    
}

void commandShowCurrentOverTempDetectorState()
{
    if ( overTempDetectorStateRead() ) {
        uartUsb.printf( "Temperature is above the maximum level\r\n");
    } else {
        uartUsb.printf( "Temperature is below the maximum level\r\n");
    }
}

void commandEnterCodeSequence()
{
    char receivedCodeSequence[CODE_NUMBER_OF_KEYS];
    
    uartUsb.printf( "Please enter the new four digits numeric code " );
    uartUsb.printf( "to deactivate the alarm.\r\n" );

    for ( int i = 0; i < CODE_NUMBER_OF_KEYS; i++) {
        receivedCodeSequence[i] = uartUsb.getc();
        uartUsb.printf( "*" );
    }

    if ( codeMatch(receivedCodeSequence) ) {
        uartUsb.printf( "\r\nThe code is correct\r\n\r\n" );
    } else {
        uartUsb.printf( "\r\nThe code is incorrect\r\n\r\n" );
    }    
}

void commandEnterNewCode()
{
    char newCodeSequence[CODE_NUMBER_OF_KEYS];

    uartUsb.printf( "Please enter the new four digits numeric code\r\n" );

    for ( int i = 0; i < CODE_NUMBER_OF_KEYS; i++) {
        newCodeSequence[i] = uartUsb.getc();
        uartUsb.printf( "*" );
    }

    codeWrite( newCodeSequence );

    uartUsb.printf( "\r\nNew code configurated\r\n\r\n" );
}

void commandShowCurrentTemperatureInCelsius()
{
    uartUsb.printf( "Temperature: %.2f �C\r\n",
                    temperatureSensorReadCelsius() );    
}

void commandShowCurrentTemperatureInFahrenheit()
{
    uartUsb.printf( "Temperature: %.2f �F\r\n", 
                    temperatureSensorReadFahrenheit() );    
}

void commandSetDateAndTime()
{
    int year   = 0;
    int month  = 0;
    int day    = 0;
    int hour   = 0;
    int minute = 0;
    int second = 0;
    
    uartUsb.printf("Enter the current year (YYYY): ");
    uartUsb.scanf("%d", &year);
    uartUsb.printf("%d\r\n", year);

    uartUsb.printf("Enter the current month (1-12): ");
    uartUsb.scanf("%d", &month);
    uartUsb.printf("%d\r\n", month);

    uartUsb.printf("Enter the current day (1-31): ");
    uartUsb.scanf("%d", &day);
    uartUsb.printf("%d\r\n", day);

    uartUsb.printf("Enter the current hour (0-24): ");
    uartUsb.scanf("%d", &hour);
    uartUsb.printf("%d\r\n",hour);

    uartUsb.printf("Enter the current minute (0-59): ");
    uartUsb.scanf("%d", &minute);
    uartUsb.printf("%d\r\n", minute);

    uartUsb.printf("Enter the current second (0-59): ");
    uartUsb.scanf("%d", &second);
    uartUsb.printf("%d\r\n", second);

    while ( uartUsb.readable() ) {
        uartUsb.getc();
    }

    dateAndTimeWriteIndividualValues( year, month, day, 
                                      hour, minute, second );
}

void commandShowDateAndTime()
{
    uartUsb.printf("Date and Time = %s", dateAndTimeReadString());
}

void commandShowStoredEvents()
{
    char str[100];
    for (int i = 0; i < eventLogNumberOfStoredEvents(); i++) {
        eventLogReadStriangAtIndex( i, str );
        uartUsb.printf( "%s\r\n", str );                       
    }
}

void eventLogUpdate()
{
    bool currentState = sirenStateRead();
    eventLogElementStateUpdate( sirenLastState, currentState, "ALARM" );
    sirenLastState = currentState;

    currentState = gasDetectorStateRead();
    eventLogElementStateUpdate( gasLastState, currentState, "GAS_DET" );
    gasLastState = currentState;

    currentState = overTempDetectorStateRead();
    eventLogElementStateUpdate( tempLastState, currentState, "OVER_TEMP" );
    tempLastState = currentState;

    currentState = incorrectCodeStateRead();
    eventLogElementStateUpdate( ICLastState, currentState, "LED_IC" );
    ICLastState = currentState;

    currentState = systemBlockedStateRead();
    eventLogElementStateUpdate( SBLastState ,currentState, "LED_SB" );
    SBLastState = currentState;
}

int eventLogNumberOfStoredEvents()
{
    return eventsIndex;
}

void eventLogReadStriangAtIndex( int index, char* str )
{
    str[0] = 0;

    strncat( str, "Event = ", strlen("Event = ") );

    strncat( str, arrayOfStoredEvents[index].typeOfEvent,
             strlen(arrayOfStoredEvents[index].typeOfEvent) );

    strncat( str, "\r\nDate and Time = ", strlen("\r\nDate and Time = ") );

    strncat( str, ctime(&arrayOfStoredEvents[index].seconds),
             strlen(ctime(&arrayOfStoredEvents[index].seconds)) );

    strncat( str, "\r\n", strlen("\r\n") );
}

void eventLogWrite( bool currentState, const char* elementName )
{
    char eventAndStateStr[EVENT_LOG_NAME_MAX_LENGTH];
    eventAndStateStr[0] = 0;
    strncat( eventAndStateStr, elementName, strlen(elementName) );
    if ( currentState ) {
        strncat( eventAndStateStr, "_ON", strlen("_ON") );
    } else {
        strncat( eventAndStateStr, "_OFF", strlen("_OFF") );
    }

    arrayOfStoredEvents[eventsIndex].seconds = time(NULL);
    strcpy( arrayOfStoredEvents[eventsIndex].typeOfEvent, eventAndStateStr );
    if ( eventsIndex < EVENT_LOG_MAX_STORAGE ) {
        eventsIndex++;
    } else {
        eventsIndex = 0;
    }

    pcSerialComWrite(eventAndStateStr);
    pcSerialComWrite("\r\n");
 
    smarphoneBleComWrite(eventAndStateStr);
    smarphoneBleComWrite("\r\n");
}

void eventLogElementStateUpdate( bool lastState,
                                 bool currentState,
                                 const char* elementName )
{
    if ( lastState != currentState ) {        
        eventLogWrite( currentState, elementName );       
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
    for( i=1; i<LM35_NUMBER_OF_AVG_SAMPLES; i++ ) {
        lm35AvgReadingsArray[i-1] = lm35AvgReadingsArray[i];
    }
    lm35AvgReadingsArray[LM35_NUMBER_OF_AVG_SAMPLES-1] = 0.0;
}

void matrixKeypadInit()
{
    matrixKeypadState = MATRIX_KEYPAD_SCANNING;
    int pinIndex = 0;
    for( pinIndex=0; pinIndex<MATRIX_KEYPAD_NUMBER_OF_COLS; pinIndex++ ) {
        (keypadColPins[pinIndex]).mode(PullUp);
    }
}

char matrixKeypadScan()
{
    int r = 0;
    int c = 0;
    int i = 0; 

    char matrixKeypadIndexToCharArray[] = {
        '1', '2', '3', 'A',
        '4', '5', '6', 'B',
        '7', '8', '9', 'C',
        '*', '0', '#', 'D',
    };

    for( r=0; r<MATRIX_KEYPAD_NUMBER_OF_ROWS; r++ ) {

        for( i=0; i<MATRIX_KEYPAD_NUMBER_OF_ROWS; i++ ) {
            keypadRowPins[i] = ON;
        }

        keypadRowPins[r] = OFF;

        for( c=0; c<MATRIX_KEYPAD_NUMBER_OF_COLS; c++ ) {
            if( keypadColPins[c] == OFF ) {
                return matrixKeypadIndexToCharArray[r*MATRIX_KEYPAD_NUMBER_OF_ROWS + c];
            }
        }
    }
    return '\0';
}

char matrixKeypadUpdate()
{
    static int accumulatedDebounceMatrixKeypadTime = 0;
    static char matrixKeypadLastKeyPressed = '\0';

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
            accumulatedDebounceMatrixKeypadTime + SYSTEM_TIME_INCREMENT_MS;
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