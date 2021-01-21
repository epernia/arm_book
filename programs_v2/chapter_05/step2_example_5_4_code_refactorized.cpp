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

//=====[Defines]=============================================================

// Module: code ---------------------------------------

#define CODE_NUMBER_OF_KEYS   4

// Module: event_log ----------------------------------

#define EVENT_LOG_MAX_STORAGE       100
#define EVENT_LOG_NAME_MAX_LENGTH    15
#define EVENT_HEAD_STR_LENGTH         8
#define NEW_LINE_STR_LENGTH           2
#define DATE_AND_TIME_STR_LENGTH     15
#define CTIME_STR_LENGTH             25
#define EVENT_STR_LENGTH              EVENT_HEAD_STR_LENGTH + \
                                      EVENT_LOG_NAME_MAX_LENGTH + \
                                      NEW_LINE_STR_LENGTH  + \
                                      DATE_AND_TIME_STR_LENGTH + \
                                      CTIME_STR_LENGTH

// Module: fire_alarm ---------------------------------

#define TEMPERATURE_C_LIMIT_ALARM        50.0
#define STROBE_TIME_GAS                1000
#define STROBE_TIME_OVER_TEMP           500
#define STROBE_TIME_GAS_AND_OVER_TEMP   100

// Module: matrix_keypad ------------------------------

#define MATRIX_KEYPAD_NUMBER_OF_ROWS    4
#define MATRIX_KEYPAD_NUMBER_OF_COLS    4
#define DEBOUNCE_KEY_TIME_MS           40

// Module: smart_home_system --------------------------

#define SYSTEM_TIME_INCREMENT_MS   10

// Module: temperature_sensor -------------------------

#define LM35_NUMBER_OF_AVG_SAMPLES    100

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
    PC_SERIAL_GET_CODE,
    PC_SERIAL_SAVE_NEW_CODE,
} pcSerialComMode_t;

//=====[Declaration and initialization of public global objects]===============

// Module: fire_alarm ---------------------------------

DigitalIn alarmTestButton(BUTTON1); 

// Module: gas_sensor ---------------------------------

DigitalIn mq2(PE_12);

// Module: matrix_keypad ------------------------------

DigitalOut keypadRowPins[MATRIX_KEYPAD_NUMBER_OF_ROWS] = {PB_3, PB_5, PC_7, PA_15};
DigitalIn keypadColPins[MATRIX_KEYPAD_NUMBER_OF_COLS]  = {PB_12, PB_13, PB_15, PC_6};

// Module: pc_serial_com ------------------------------

Serial uartUsb(USBTX, USBRX);

// Module: siren --------------------------------------

DigitalOut sirenPin(PE_10);

// Module: strobe light -------------------------------

DigitalOut strobeLight(LED1);

// Module: temperature_sensor -------------------------

AnalogIn lm35(A1);

// Module: user_interface -----------------------------

DigitalOut incorrectCodeLed(LED3);
DigitalOut systemBlockedLed(LED2);

//=====[Declaration and initialization of public global variables]=============

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

bool gasDetected                  = OFF;
bool overTemperatureDetected      = OFF;
bool gasDetectorState             = OFF;
bool overTemperatureDetectorState = OFF;

// Module: matrix_keypad ------------------------------

matrixKeypadState_t matrixKeypadState;
int timeIncrement_ms = 0;
int accumulatedDebounceMatrixKeypadTime = 0;
char matrixKeypadLastKeyPressed = '\0';

// Module: pc_serial_com ------------------------------

char codeSequenceFromPcSerialCom[CODE_NUMBER_OF_KEYS];
pcSerialComMode_t pcSerialComMode = PC_SERIAL_COMMANDS;
bool codeCompleteFromPcSerialCom = false;
int numberOfCodeCharsFromPcSerialCom = 0;
char newCodeSequence[CODE_NUMBER_OF_KEYS];

// Module: siren --------------------------------------

bool sirenState = OFF;
int accumulatedTimeAlarm = 0;

// Module: strobe_light --------------------------------

bool strobeLightState = OFF;

// Module: temperature_sensor -------------------------

float lm35TemperatureC = 0.0;
float lm35ReadingsArray[LM35_NUMBER_OF_AVG_SAMPLES];
int lm35SampleIndex     = 0;

// Module: user_interface -----------------------------

char codeSequenceFromUserInterface[CODE_NUMBER_OF_KEYS];
bool incorrectCodeState = OFF;
bool systemBlockedState = OFF;
bool codeComplete = false;
int numberOfCodeChars = 0;
int numberOfHashKeyReleased = 0;

//=====[Declarations (prototypes) of public functions]=========================

// Module: code ---------------------------------------

void codeWrite( char* newCodeSequence );
bool codeMatchFrom( codeOrigin_t codeOrigin );
bool codeMatch( char* codeToCompare );
void codeDeactivate();

// Module: date_and_time ------------------------------

char* dateAndTimeRead();
void dateAndTimeWrite( int year, int month, int day, 
                       int hour, int minute, int second );

// Module: event_log ----------------------------------

void eventLogUpdate();
int eventLogNumberOfStoredEvents();
void eventLogRead( int index, char* str );
void eventLogWrite( bool currentState, const char* elementName );
void eventLogElementStateUpdate( bool lastState,
                                 bool currentState,
                                 const char* elementName );

// Module: fire_alarm ---------------------------------

void fireAlarmInit();
void fireAlarmUpdate();
bool gasDetectorStateRead();
bool overTemperatureDetectorStateRead();
bool gasDetectedRead();
bool overTemperatureDetectedRead();
void fireAlarmActivationUpdate();
void fireAlarmDeactivationUpdate();
void fireAlarmDeactivate();
int fireAlarmStrobeTime();

// Module: gas_sensor ---------------------------------

void gasSensorInit();
void gasSensorUpdate();
bool gasSensorRead();

// Module: matrix_keypad ------------------------------

void matrixKeypadInit( int updateTime_ms );
char matrixKeypadUpdate();
char matrixKeypadScan();
void matrixKeypadReset();

// Module: pc_serial_com ------------------------------

void pcSerialComInit();
char pcSerialComCharRead();
void pcSerialComStringWrite( const char* str );
void pcSerialComUpdate();
bool pcSerialComCodeCompleteRead();
void pcSerialComCodeCompleteWrite( bool state );
void pcSerialComGetCodeUpdate( char receivedChar );
void pcSerialComSaveNewCodeUpdate( char receivedChar );
void pcSerialComCommandUpdate( char receivedChar );
void availableCommands();
void commandShowCurrentAlarmState();
void commandShowCurrentGasDetectorState();
void commandShowCurrentOverTemperatureDetectorState();
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
void sirenUpdate( int strobeTime );

// Module: strobe light --------------------------------------

void strobeLightInit();
bool strobeLightStateRead();
void strobeLightStateWrite( bool state );
void strobeLightUpdate( int strobeTime );

// Module: smart_home_system --------------------------

void smartHomeSystemInit();
void smartHomeSystemUpdate();

// Module: temperature_sensor -------------------------

void temperatureSensorInit();
void temperatureSensorUpdate();
float temperatureSensorReadCelsius();
float temperatureSensorReadFahrenheit();
float celsiusToFahrenheit( float tempInCelsiusDegrees );
float analogReadingScaledWithTheLM35Formula( float analogReading );

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
    int i;
    for (i = 0; i < CODE_NUMBER_OF_KEYS; i++) {
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
                    pcSerialComStringWrite( "\r\nThe code is correct\r\n\r\n" );
                } else {
                    incorrectCodeStateWrite(ON);
                    numberOfIncorrectCodes++;
                    pcSerialComStringWrite( "\r\nThe code is incorrect\r\n\r\n" );
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

char* dateAndTimeRead()
{
    time_t epochSeconds;
    epochSeconds = time(NULL);
    return ctime(&epochSeconds);    
}

void dateAndTimeWrite( int year, int month, int day, 
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

    currentState = overTemperatureDetectorStateRead();
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

void eventLogRead( int index, char* str )
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

    pcSerialComStringWrite(eventAndStateStr);
    pcSerialComStringWrite("\r\n");

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
    temperatureSensorInit();
    gasSensorInit();
    sirenInit();
    strobeLightInit();	

    alarmTestButton.mode(PullDown); 
}

void fireAlarmUpdate()
{
    fireAlarmActivationUpdate();
    fireAlarmDeactivationUpdate();
    sirenUpdate( fireAlarmStrobeTime() );
    strobeLightUpdate( fireAlarmStrobeTime() );
}

bool gasDetectorStateRead()
{
    return gasDetectorState;
}

bool overTemperatureDetectorStateRead()
{
    return overTemperatureDetectorState;
}

bool gasDetectedRead()
{
    return gasDetected;
}

bool overTemperatureDetectedRead()
{
    return overTemperatureDetected;
}

void fireAlarmActivationUpdate()
{
    temperatureSensorUpdate();
    gasSensorUpdate();

    overTemperatureDetectorState = temperatureSensorReadCelsius() > 
                            TEMPERATURE_C_LIMIT_ALARM;

    if ( overTemperatureDetectorState ) {
        overTemperatureDetected = ON;
        sirenStateWrite(ON);
        strobeLightStateWrite(ON);
    }

    gasDetectorState = !gasSensorRead();

    if ( gasDetectorState ) {
        gasDetected = ON;
        sirenStateWrite(ON);
        strobeLightStateWrite(ON);
    }
    
    if ( alarmTestButton ) {
        overTemperatureDetected = ON;
        gasDetected = ON;
        sirenStateWrite(ON);
        strobeLightStateWrite(ON);
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
    strobeLightStateWrite(OFF);
    overTemperatureDetected = OFF;
    gasDetected      = OFF;    
}

int fireAlarmStrobeTime()
{
    if( gasDetectedRead() && overTemperatureDetectedRead() ) {
        return STROBE_TIME_GAS_AND_OVER_TEMP;
    } else if ( gasDetectedRead() ) {
        return STROBE_TIME_GAS;
    } else if ( overTemperatureDetectedRead() ) {
        return STROBE_TIME_OVER_TEMP;
    } else {
        return 0;
    }
}

// Module: gas_sensor ---------------------------------

void gasSensorInit()
{
    return;
}

void gasSensorUpdate()
{
    return;
}

bool gasSensorRead()
{
    return mq2;
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
            DEBOUNCE_KEY_TIME_MS ) {
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
    int row = 0;
    int col = 0;
    int i = 0; 

    char matrixKeypadIndexToCharArray[] = {
        '1', '2', '3', 'A',
        '4', '5', '6', 'B',
        '7', '8', '9', 'C',
        '*', '0', '#', 'D',
    };

    for( row=0; row<MATRIX_KEYPAD_NUMBER_OF_ROWS; row++ ) {

        for( i=0; i<MATRIX_KEYPAD_NUMBER_OF_ROWS; i++ ) {
            keypadRowPins[i] = ON;
        }

        keypadRowPins[row] = OFF;

        for( col=0; col<MATRIX_KEYPAD_NUMBER_OF_COLS; col++ ) {
            if( keypadColPins[col] == OFF ) {
                return matrixKeypadIndexToCharArray[
                    row*MATRIX_KEYPAD_NUMBER_OF_ROWS + col];
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

void pcSerialComInit()
{
    availableCommands();
}

char pcSerialComCharRead()
{
    char receivedChar = '\0';
    if( uartUsb.readable() ) {
        receivedChar = uartUsb.getc();
    }
    return receivedChar;
}

void pcSerialComStringWrite( const char* str )
{
    uartUsb.printf( "%s", str );
}

void pcSerialComUpdate()
{
    char receivedChar = pcSerialComCharRead();
    if( receivedChar != '\0' ) {
        switch ( pcSerialComMode ) {
            case PC_SERIAL_COMMANDS:
                pcSerialComCommandUpdate( receivedChar );
            break;

            case PC_SERIAL_GET_CODE:
                pcSerialComGetCodeUpdate( receivedChar );
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
    return codeCompleteFromPcSerialCom;
}

void pcSerialComCodeCompleteWrite( bool state )
{
    codeCompleteFromPcSerialCom = state;
}

void pcSerialComGetCodeUpdate( char receivedChar )
{
    codeSequenceFromPcSerialCom[numberOfCodeCharsFromPcSerialCom] = receivedChar;
    uartUsb.printf( "*" );
    numberOfCodeCharsFromPcSerialCom++;
   if ( numberOfCodeCharsFromPcSerialCom >= CODE_NUMBER_OF_KEYS ) {
        pcSerialComMode = PC_SERIAL_COMMANDS;
        codeCompleteFromPcSerialCom = true;
        numberOfCodeCharsFromPcSerialCom = 0;
    } 
}

void pcSerialComSaveNewCodeUpdate( char receivedChar )
{
    newCodeSequence[numberOfCodeCharsFromPcSerialCom] = receivedChar;
    uartUsb.printf( "*" );
    numberOfCodeCharsFromPcSerialCom++;
    if ( numberOfCodeCharsFromPcSerialCom >= CODE_NUMBER_OF_KEYS ) {
        pcSerialComMode = PC_SERIAL_COMMANDS;
        numberOfCodeCharsFromPcSerialCom = 0;
        codeWrite( newCodeSequence );
        uartUsb.printf( "\r\nNew code configured\r\n\r\n" );
    } 
}

void pcSerialComCommandUpdate( char receivedChar )
{
    switch (receivedChar) {
        case '1': commandShowCurrentAlarmState(); break;
        case '2': commandShowCurrentGasDetectorState(); break;
        case '3': commandShowCurrentOverTemperatureDetectorState(); break;
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
    uartUsb.printf( "Press '4' to enter the code to deactivate the alarm\r\n" );
    uartUsb.printf( "Press '5' to enter a new code to deactivate the alarm\r\n" );
    uartUsb.printf( "Press 'f' or 'F' to get lm35 reading in Fahrenheit\r\n" );
    uartUsb.printf( "Press 'c' or 'C' to get lm35 reading in Celsius\r\n" );
    uartUsb.printf( "Press 's' or 'S' to set the date and time\r\n" );
    uartUsb.printf( "Press 't' or 'T' to get the date and time\r\n" );
    uartUsb.printf( "Press 'e' or 'E' to get the stored events\r\n" );
    uartUsb.printf( "\r\n" );
}

void commandShowCurrentAlarmState()
{
    if ( sirenStateRead() ) {
        uartUsb.printf( "The alarm is activated\r\n");
    } else {
        uartUsb.printf( "The alarm is not activated\r\n");
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

void commandShowCurrentOverTemperatureDetectorState()
{
    if ( overTemperatureDetectorStateRead() ) {
        uartUsb.printf( "Temperature is above the maximum level\r\n");
    } else {
        uartUsb.printf( "Temperature is below the maximum level\r\n");
    }
}

void commandEnterCodeSequence()
{
    if( sirenStateRead() ) {
        uartUsb.printf( "Please enter the four digits numeric code " );
        uartUsb.printf( "to deactivate the alarm: " );
        pcSerialComMode = PC_SERIAL_GET_CODE;
        codeCompleteFromPcSerialCom = false;
        numberOfCodeCharsFromPcSerialCom = 0;
    } else {
        uartUsb.printf( "Alarm is not activated.\r\n" );
    }
}

void commandEnterNewCode()
{
    uartUsb.printf( "Please enter the new four digits numeric code " );
    uartUsb.printf( "to deactivate the alarm: " );
    numberOfCodeCharsFromPcSerialCom = 0;
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

    dateAndTimeWrite( year, month, day, hour, minute, second );
}

void commandShowDateAndTime()
{
    uartUsb.printf("Date and Time = %s", dateAndTimeRead());
}

void commandShowStoredEvents()
{
    char str[EVENT_STR_LENGTH];
    int i;
    for (int i = 0; i < eventLogNumberOfStoredEvents(); i++) {
        eventLogRead( i, str );
        uartUsb.printf( "%s\r\n", str );                       
    }
}

// Module: siren --------------------------------------

void sirenInit()
{
    sirenPin = ON;
}

bool sirenStateRead()
{
    return sirenState;
}

void sirenStateWrite( bool state )
{
    sirenState = state;
}

void sirenUpdate( int strobeTime )
{
    static int accumulatedTimeAlarm = 0;
    accumulatedTimeAlarm = accumulatedTimeAlarm + SYSTEM_TIME_INCREMENT_MS;
    
    if( sirenState ) {
        if( accumulatedTimeAlarm >= strobeTime ) {
                accumulatedTimeAlarm = 0;
                sirenPin= !sirenPin;
        }
    } else {
        sirenPin = ON;
    }
}

// Module: strobe_light -------------------------

void strobeLightInit()
{
    strobeLight = OFF;
}

bool strobeLightStateRead()
{
    return strobeLightState;
}

void strobeLightStateWrite( bool state )
{
    strobeLightState = state;
}

void strobeLightUpdate( int strobeTime )
{
    static int accumulatedTimeAlarm = 0;
    accumulatedTimeAlarm = accumulatedTimeAlarm + SYSTEM_TIME_INCREMENT_MS;
    
    if( strobeLightState ) {
        if( accumulatedTimeAlarm >= strobeTime ) {
            accumulatedTimeAlarm = 0;
            strobeLight= !strobeLight;
        }
    } else {
        strobeLight = OFF;
    }
}

// Module: smart_home_system --------------------------

void smartHomeSystemInit()
{
    userInterfaceInit();
    fireAlarmInit();
    pcSerialComInit();
}

void smartHomeSystemUpdate()
{
    fireAlarmUpdate();    
    userInterfaceUpdate();
    pcSerialComUpdate();
    eventLogUpdate();
    delay(SYSTEM_TIME_INCREMENT_MS);
}

// Module: temperature_sensor -------------------------

void temperatureSensorInit()
{
    int i;
    
    for( i=0; i<LM35_NUMBER_OF_AVG_SAMPLES ; i++ ) {
        lm35ReadingsArray[i] = 0;
    }
}

void temperatureSensorUpdate()
{
    static int lm35SampleIndex = 0;
    float lm35ReadingsSum = 0.0;
    float lm35ReadingsAverage = 0.0;

    int i = 0;

    lm35ReadingsArray[lm35SampleIndex] = lm35.read();
	   lm35SampleIndex++;
    if ( lm35SampleIndex >= LM35_NUMBER_OF_AVG_SAMPLES) {
        lm35SampleIndex = 0;
    }
	
   lm35ReadingsSum = 0.0;
    for (i = 0; i < LM35_NUMBER_OF_AVG_SAMPLES; i++) {
        lm35ReadingsSum = lm35ReadingsSum + lm35ReadingsArray[i];
    }
    lm35ReadingsAverage = lm35ReadingsSum / LM35_NUMBER_OF_AVG_SAMPLES;
	   lm35TemperatureC = analogReadingScaledWithTheLM35Formula ( lm35ReadingsAverage );	
}


float temperatureSensorReadCelsius()
{
    return lm35TemperatureC;
}

float temperatureSensorReadFahrenheit()
{
    return celsiusToFahrenheit( lm35TemperatureC );
}

float celsiusToFahrenheit( float tempInCelsiusDegrees )
{
    return ( tempInCelsiusDegrees * 9.0 / 5.0 + 32.0 );
}

float analogReadingScaledWithTheLM35Formula( float analogReading )
{
    return ( analogReading * 3.3 / 0.01 );
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
                    numberOfHashKeyReleased++;
                    if( numberOfHashKeyReleased >= 2 ) {
                        numberOfHashKeyReleased = 0;
                        numberOfCodeChars = 0;
                        codeComplete = false;
                        incorrectCodeState = OFF;
                    }
                }
            }
        }
    }
}