/*
Modules:
--------

code
date_and_time
event_log
fire_alarm
gas_sensor
matrix_keypad
pc_serial_com
siren
smart_home_system
smartphone_ble_com
temperature_sensor
user_interface
*/

//=====[Libraries]=============================================================

#include "mbed.h"
#include "arm_book_lib.h"

//=====[Constants]=============================================================

// Module: code ---------------------------------------

#define CODE_NUMBER_OF_KEYS   4

// Module: event_log ----------------------------------

#define EVENT_LOG_MAX_STORAGE       100
#define EVENT_LOG_NAME_MAX_LENGTH    15

// Module: fire_alarm ---------------------------------

#define TEMPERATURE_C_LIMIT_ALARM       50.0
#define GAS_CONCENTRATION_LIMIT_ALARM   0.0 // TODO: Ver que valor de umbral poner

// Module: matrix_keypad ------------------------------

#define MATRIX_KEYPAD_NUMBER_OF_ROWS    4
#define MATRIX_KEYPAD_NUMBER_OF_COLS    4
#define DEBOUNCE_BUTTON_TIME_MS        40

// Module: siren --------------------------------------

#define SIREN_BLINKING_TIME_GAS               1000
#define SIREN_BLINKING_TIME_OVER_TEMP          500
#define SIREN_BLINKING_TIME_GAS_AND_OVER_TEMP  100

// Module: smart_home_system --------------------------

#define SYSTEM_TIME_INCREMENT_MS   10

// Module: temperature_sensor -------------------------

#define LM35_SAMPLE_TIME             100
#define LM35_NUMBER_OF_AVG_SAMPLES    10

//=====[Declaration of public data types]======================================

// Module: code ---------------------------------------

typedef enum{
    CODE_KEYPAD,
    CODE_PC_SERIAL,
} codeOrigin_t;

// Module: event_log ----------------------------------

typedef struct systemEvent {
    time_t seconds;
    char typeOfEvent[EVENT_LOG_NAME_MAX_LENGTH];
} systemEvent_t;

// Module: matrix_keypad ------------------------------

typedef enum {
    MATRIX_KEYPAD_SCANNING,
    MATRIX_KEYPAD_DEBOUNCE,
    MATRIX_KEYPAD_KEY_HOLD_PRESSED
} matrixKeypadState_t;

// Module: pc_serial_com ------------------------------

typedef enum{
    PC_SERIAL_COMMANDS,
    PC_SERIAL_SAVE_CODE,
    PC_SERIAL_SAVE_NEW_CODE,
} pcSerialComMode_t;

//=====[Declaration and intitalization of public global objects]===============

// Module: gas_sensor ---------------------------------

DigitalIn gasDetector(D2);

// Module: matrix_keypad ------------------------------

DigitalOut keypadRowPins[MATRIX_KEYPAD_NUMBER_OF_ROWS] = {D23, D22, D21, D20};
DigitalIn keypadColPins[MATRIX_KEYPAD_NUMBER_OF_COLS]  = {D19, D18, D17, D16};

// Module: pc_serial_com ------------------------------

Serial uartUsb(USBTX, USBRX);

// Module: siren --------------------------------------

DigitalOut alarmLed(LED1);

// Module: smartphone_ble_com -------------------------

Serial uartBle(D1, D0);

// Module: temperature_sensor -------------------------

AnalogIn lm35(A1);

// Module: user_interface -----------------------------

DigitalOut incorrectCodeLed(LED3);
DigitalOut systemBlockedLed(LED2);

//=====[Declaration and intitalization of public global variables]=============

// Module: code ---------------------------------------

int numberOfIncorrectCodes = 0;
char codeSequence[CODE_NUMBER_OF_KEYS] = { '1', '8', '0', '5' };

// Module: event_log ----------------------------------

bool sirenLastState = OFF;
bool gasLastState   = OFF;
bool tempLastState  = OFF;
bool ICLastState    = OFF;
bool SBLastState    = OFF;
int eventsIndex     = 0;
systemEvent_t arrayOfStoredEvents[EVENT_LOG_MAX_STORAGE];

// Module: fire_alarm ---------------------------------

bool gasDetected           = OFF;
bool overTempDetected      = OFF;
bool gasDetectorState      = OFF;
bool overTempDetectorState = OFF;

// Module: matrix_keypad ------------------------------

matrixKeypadState_t matrixKeypadState;
int timeIncrement_ms = 0;

// Module: pc_serial_com ------------------------------

char codeSequenceFromPcSerialCom[CODE_NUMBER_OF_KEYS];
pcSerialComMode_t pcSerialComMode = PC_SERIAL_COMMANDS;
bool codeComplete = false;
int numberOfCodeChars = 0;

// Module: siren --------------------------------------

bool sirenState = OFF;

// Module: temperature_sensor -------------------------

float lm35TempC = 0.0;
float lm35AvgReadingsArray[LM35_NUMBER_OF_AVG_SAMPLES];

// Module: user_interface -----------------------------

char codeSequenceFromUserInterface[CODE_NUMBER_OF_KEYS];
bool incorrectCodeState = OFF;
bool systemBlockedState = OFF;
bool codeComplete = false;
int numberOfCodeChars = 0;

//=====[Declarations (prototypes) of public functions]=========================

// Module: code ---------------------------------------

void codeWrite( char* newCodeSequence );
bool codeMatch( char* codeToCompare );
bool codeMatchFrom( codeOrigin_t codeOrigin );
void codeDeactivate();

// Module: date_and_time ------------------------------

char* dateAndTimeReadString();
void dateAndTimeWriteIndividualValues( int year, int month, int day, 
                                       int hour, int minute, int second );

// Module: event_log ----------------------------------

void eventLogUpdate();
int eventLogNumberOfStoredEvents();
void eventLogReadStriangAtIndex( int index, char* str );
void eventLogWrite( bool currentState, const char* elementName );
void eventLogElementStateUpdate( bool lastState,
                                 bool currentState,
                                 const char* elementName );

// Module: fire_alarm ---------------------------------

void fireAlarmInit();
void fireAlarmUpdate();
bool gasDetectorStateRead();
bool overTempDetectorStateRead();
bool gasDetectedRead();
bool overTempDetectedRead();
void fireAlarmActivationUpdate();
void fireAlarmDeactivationUpdate();
void fireAlarmDeactivate();

// Module: gas_sensor ---------------------------------

void gasSensorInit( void );
void gasSensorUpdate( void );
float gasSensorRead();

// Module: matrix_keypad ------------------------------

void matrixKeypadInit( int updateTime_ms );
char matrixKeypadUpdate();
char matrixKeypadScan();
void matrixKeypadReset();

// Module: pc_serial_com ------------------------------

char pcSerialComReadChar();
void pcSerialComWrite( const char* str );
void pcSerialComUpdate();
bool pcSerialComCodeCompleteRead();
void pcSerialComCodeCompleteWrite( bool state );
void pcSerialComSaveCodeUpdate( char receivedChar );
void pcSerialComSaveNewCodeUpdate( char receivedChar );
void pcSerialComCommandUpdate( char receivedChar );
void availableCommands();
void commandShowCurrentSirenState();
void commandShowCurrentGasDetectorState();
void commandShowCurrentOverTempDetectorState();
void commandEnterCodeSequence();
void commandEnterNewCode();
void commandShowCurrentTemperatureInCelsius();
void commandShowCurrentTemperatureInFahrenheit();
void commandSetDateAndTime();
void commandShowDateAndTime();
void commandShowStoredEvents();

// Module: siren --------------------------------------

void sirenInit();
bool sirenStateRead();
void sirenStateWrite( bool state );
void sirenIndicatorUpdate();

// Module: smart_home_system --------------------------

void smartHomeSystemInit();
void smartHomeSystemUpdate();

// Module: smartphone_ble_com -------------------------

void smartphoneBleComWrite( const char* str );

// Module: temperature_sensor -------------------------

void temperatureSensorInit( void );
void temperatureSensorUpdate( void );
float temperatureSensorReadCelsius( void );
float temperatureSensorReadFahrenheit( void );
float celsiusToFahrenheit( float tempInCelsiusDegrees );
float analogReadingScaledWithTheLM35Formula( float analogReading );
void shiftLm35AvgReadingsArray();

// Module: user_interface -----------------------------

void userInterfaceInit();
void userInterfaceUpdate();
bool userInterfaceCodeCompleteRead();
void userInterfaceCodeCompleteWrite( bool state );
bool incorrectCodeStateRead();
void incorrectCodeStateWrite( bool state );
void incorrectCodeIndicatorUpdate();
bool systemBlockedStateRead();
void systemBlockedStateWrite( bool state );
void systemBlockedIndicatorUpdate();
void userInterfaceMatrixKeypadUpdate();

//=====[Main function, the program entry point after power on or reset]========

int main()
{
    smartHomeSystemInit();
    while (true) {
        smartHomeSystemUpdate();
    }
}

//=====[Implementations of public functions]===================================

// Module: code ---------------------------------------

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

bool codeMatchFrom( codeOrigin_t codeOrigin )
{
    bool codeIsCorrect = false;
    switch (codeOrigin) {
        case CODE_KEYPAD:
            if( userInterfaceCodeCompleteRead() ) {
                codeIsCorrect = codeMatch(codeSequenceFromUserInterface);
                userInterfaceCodeCompleteWrite(false);
                if ( codeIsCorrect ) {
                    codeDeactivate();
                } else {
                    incorrectCodeStateWrite(ON);
                    numberOfIncorrectCodes++;
                }
            }


        break;
        case CODE_PC_SERIAL:
            if( pcSerialComCodeCompleteRead() ) {
                codeIsCorrect = codeMatch(codeSequenceFromPcSerialCom);
                pcSerialComCodeCompleteWrite(false);
                if ( codeIsCorrect ) {
                    codeDeactivate();
                    pcSerialComWrite( "\r\nThe code is correct\r\n\r\n" );
                } else {
                    incorrectCodeStateWrite(ON);
                    numberOfIncorrectCodes++;
                    pcSerialComWrite( "\r\nThe code is incorrect\r\n\r\n" );
                }
            }

        break;
        default:
        break;
    }

    if ( numberOfIncorrectCodes >= 5 ) {
        systemBlockedStateWrite(ON);
    }

    return codeIsCorrect;
}

void codeDeactivate()
{
    systemBlockedStateWrite(OFF);
    incorrectCodeStateWrite(OFF);
    numberOfIncorrectCodes = 0;
}

// Module: date_and_time ------------------------------

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

// Module: event_log ----------------------------------

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
 
    smartphoneBleComWrite(eventAndStateStr);
    smartphoneBleComWrite("\r\n");
}

void eventLogElementStateUpdate( bool lastState,
                                 bool currentState,
                                 const char* elementName )
{
    if ( lastState != currentState ) {        
        eventLogWrite( currentState, elementName );       
    }
}

// Module: fire_alarm ---------------------------------

void fireAlarmInit()
{
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

bool gasDetectedRead()
{
    return gasDetected;
}

bool overTempDetectedRead()
{
    return overTempDetected;
}

void fireAlarmActivationUpdate()
{
    temperatureSensorUpdate();
    gasSensorUpdate();

    overTempDetectorState = temperatureSensorReadCelsius() > 
                            TEMPERATURE_C_LIMIT_ALARM;

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

void fireAlarmDeactivationUpdate()
{
    if ( sirenStateRead() ) {
        if ( codeMatchFrom(CODE_KEYPAD) ||
             codeMatchFrom(CODE_PC_SERIAL) ) {
            fireAlarmDeactivate();
        }
    }
}

void fireAlarmDeactivate()
{
	sirenStateWrite(OFF);
    overTempDetected = OFF;
    gasDetected      = OFF;    
}

// Module: gas_sensor ---------------------------------

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

// Module: matrix_keypad ------------------------------

void matrixKeypadInit( int updateTime_ms )
{
    timeIncrement_ms = updateTime_ms;
    matrixKeypadState = MATRIX_KEYPAD_SCANNING;
    int pinIndex = 0;
    for( pinIndex=0; pinIndex<MATRIX_KEYPAD_NUMBER_OF_COLS; pinIndex++ ) {
        (keypadColPins[pinIndex]).mode(PullUp);
    }
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
            accumulatedDebounceMatrixKeypadTime + timeIncrement_ms;
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
        matrixKeypadReset();
        break;
    }
    return keyReleased;
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

void matrixKeypadReset()
{
    matrixKeypadState = MATRIX_KEYPAD_SCANNING;
}

// Module: pc_serial_com ------------------------------

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

void pcSerialComUpdate()
{
    char receivedChar = pcSerialComReadChar();
    if( receivedChar != '\0' ) {
        switch ( pcSerialComMode ) {
            case PC_SERIAL_COMMANDS:
                pcSerialComCommandUpdate( receivedChar );
            break;

            case PC_SERIAL_SAVE_CODE:
                pcSerialComSaveCodeUpdate( receivedChar );
            break;

            case PC_SERIAL_SAVE_NEW_CODE:
                pcSerialComSaveNewCodeUpdate( receivedChar );
            break;
            default:
                pcSerialComMode = PC_SERIAL_COMMANDS;
            break;
        }
    }    
}

bool pcSerialComCodeCompleteRead()
{
    return codeComplete;
}

void pcSerialComCodeCompleteWrite( bool state )
{
    codeComplete = state;
}

void pcSerialComSaveCodeUpdate( char receivedChar )
{
    codeSequenceFromPcSerialCom[numberOfCodeChars] = receivedChar;
    uartUsb.printf( "*" );
    numberOfCodeChars++;
   if ( numberOfCodeChars >= CODE_NUMBER_OF_KEYS ) {
        pcSerialComMode = PC_SERIAL_COMMANDS;
        codeComplete = true;
        numberOfCodeChars = 0;
    } 
}

void pcSerialComSaveNewCodeUpdate( char receivedChar )
{
    static char newCodeSequence[CODE_NUMBER_OF_KEYS];

    newCodeSequence[numberOfCodeChars] = receivedChar;
    uartUsb.printf( "*" );
    numberOfCodeChars++;
    if ( numberOfCodeChars >= CODE_NUMBER_OF_KEYS ) {
        pcSerialComMode = PC_SERIAL_COMMANDS;
        numberOfCodeChars = 0;
        codeWrite( newCodeSequence );
        uartUsb.printf( "\r\nNew code configurated\r\n\r\n" );
    } 
}

void pcSerialComCommandUpdate( char receivedChar )
{
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

void availableCommands()
{
    uartUsb.printf( "Available commands:\r\n" );
    uartUsb.printf( "Press '1' to get the alarm state\r\n" );
    uartUsb.printf( "Press '2' for gas detector state\r\n" );
    uartUsb.printf( "Press '3' for over temperature detector state\r\n" );
    uartUsb.printf( "Press '4' to enter the code sequence to deactivate the alarm\r\n" );
    uartUsb.printf( "Press '5' to enter a new code to deactivate the alarm\r\n" );
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
        uartUsb.printf( "Gas is being detected\r\n"); // Ver de poner la concentracion de gas no este superando el umbral
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
    if( sirenStateRead() ) {
        uartUsb.printf( "Please enter the four digits numeric code " );
        uartUsb.printf( "to deactivate the alarm.\r\n" );
        pcSerialComMode = PC_SERIAL_SAVE_CODE;
        codeComplete = false;
        numberOfCodeChars = 0;
    } else {
        uartUsb.printf( "Alarm is not activated.\r\n" );
    }
}

void commandEnterNewCode()
{
    uartUsb.printf( "Please enter the new four digits numeric code " );
    uartUsb.printf( "to deactivate the alarm.\r\n" );
    numberOfCodeChars = 0;
    pcSerialComMode = PC_SERIAL_SAVE_NEW_CODE;

}

void commandShowCurrentTemperatureInCelsius()
{
    uartUsb.printf( "Temperature: %.2f °C\r\n",
                    temperatureSensorReadCelsius() );    
}

void commandShowCurrentTemperatureInFahrenheit()
{
    uartUsb.printf( "Temperature: %.2f °F\r\n", 
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

    uartUsb.printf("Enter the current hour (0-23): ");
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

// Module: siren --------------------------------------

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
        if( gasDetectedRead() && overTempDetectedRead() ) {
            if( accumulatedTimeAlarm >= SIREN_BLINKING_TIME_GAS_AND_OVER_TEMP ) {
                accumulatedTimeAlarm = 0;
                alarmLed = !alarmLed;
            }
        } else if( gasDetectedRead() ) {
            if( accumulatedTimeAlarm >= SIREN_BLINKING_TIME_GAS ) {
                accumulatedTimeAlarm = 0;
                alarmLed = !alarmLed;
            }
        } else if ( overTempDetectedRead() ) {
            if( accumulatedTimeAlarm >= SIREN_BLINKING_TIME_OVER_TEMP  ) {
                accumulatedTimeAlarm = 0;
                alarmLed = !alarmLed;
            }
        }
    } else {
        alarmLed = OFF;
    }
}

// Module: smart_home_system --------------------------

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
    pcSerialComUpdate();
    eventLogUpdate();
    delay(SYSTEM_TIME_INCREMENT_MS);
}

// Module: smartphone_ble_com -------------------------

void smartphoneBleComWrite( const char* str )
{
    uartBle.printf( "%s", str );
}

// Module: temperature_sensor -------------------------

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

float celsiusToFahrenheit( float tempInCelsiusDegrees )
{
    return ( tempInCelsiusDegrees * 9.0 / 5.0 + 32.0 );
}

float analogReadingScaledWithTheLM35Formula( float analogReading )
{
    return ( analogReading * 3.3 / 0.01 );
}

void shiftLm35AvgReadingsArray()
{
    int i = 0;
    for( i=1; i<LM35_NUMBER_OF_AVG_SAMPLES; i++ ) {
        lm35AvgReadingsArray[i-1] = lm35AvgReadingsArray[i];
    }
    lm35AvgReadingsArray[LM35_NUMBER_OF_AVG_SAMPLES-1] = 0.0;
}

// Module: user_interface -----------------------------

void userInterfaceInit()
{
    incorrectCodeLed = OFF;
    systemBlockedLed = OFF;
    matrixKeypadInit( SYSTEM_TIME_INCREMENT_MS );
}

void userInterfaceUpdate()
{
	userInterfaceMatrixKeypadUpdate();
    incorrectCodeIndicatorUpdate();
    systemBlockedIndicatorUpdate();
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

bool userInterfaceCodeCompleteRead()
{
    return codeComplete;
}

void userInterfaceCodeCompleteWrite( bool state )
{
    codeComplete = state;
}

void userInterfaceMatrixKeypadUpdate()
{
    static int numberOfHaskKeyReleased = 0;
    char keyReleased = matrixKeypadUpdate();

    if( keyReleased != '\0' ) {

        if( sirenStateRead() && !systemBlockedStateRead() ) {
            if( !incorrectCodeStateRead() ) {
                codeSequenceFromUserInterface[numberOfCodeChars] = keyReleased;
                numberOfCodeChars++;
                if ( numberOfCodeChars >= CODE_NUMBER_OF_KEYS ) {
                    codeComplete = true;
                    numberOfCodeChars = 0;
                }
            } else {
                if( keyReleased == '#' ) {
                    numberOfHaskKeyReleased++;
                    if( numberOfHaskKeyReleased >= 2 ) {
                        numberOfHaskKeyReleased = 0;
                        numberOfCodeChars = 0;
                        codeComplete = false;
                        incorrectCodeState = OFF;
                    }
                }
            }
        }
    }
}