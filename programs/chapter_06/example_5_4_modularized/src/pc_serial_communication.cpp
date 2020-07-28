//=====[Libraries]=============================================================

#include "mbed.h"
#include "arm_book_lib.h"

#include "pc_serial_communication.h"

#include "alarm.h"
#include "date_and_time.h"
#include "temperature_sensor.h"
#include "gas_sensor.h"
#include "event_log.h"

//=====[Declaration of private constants]======================================

//=====[Declaration of private data types]=====================================

//=====[Declaration of external public global objects]=========================

//=====[Declaration and intitalization of public global objects]===============

Serial uartUsb(USBTX, USBRX);

//=====[Declaration and intitalization of private global objects]==============

//=====[Declaration of external public global variables]=======================

//=====[Declaration and intitalization of public global variables]=============

//=====[Declaration and intitalization of private global variables]============

//=====[Declarations (prototypes) of private functions]========================

static void availableCommands();
static void commandShowCurrentAlarmState();
static void commandShowCurrentGasDetectorState();
static void commandShowCurrentOverTempDetectorState();
static void commandEnterNewCode();
static void commandEnterCodeSequence();
static void commandShowCurrentTemperatureInCelsius();
static void commandShowCurrentTemperatureInFahrenheit();
static void commandSetDateAndTime();
static void commandShowDateAndTime();
static void commandShowStoredEvents();

//=====[Implementations of public functions]===================================

void pcSerialCommunicationWrite( const char* str )
{
    uartUsb.printf( "%s", str );
}

void pcSerialCommunicationCommandUpdate()
{
    if( uartUsb.readable() ) {
        char receivedChar = uartUsb.getc();

        switch (receivedChar) {
            case '1': commandShowCurrentAlarmState(); break;
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

//=====[Implementations of private functions]==================================

static void availableCommands()
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

static void commandShowCurrentAlarmState()
{
    if ( alarmReadState() ) {
        uartUsb.printf( "The alarmLed is activated\r\n");
    } else {
        uartUsb.printf( "The alarmLed is not activated\r\n");
    }
}

static void commandShowCurrentGasDetectorState()
{
    if ( alarmGasDetectorReadState() ) {
        uartUsb.printf( "Gas is being detected\r\n");
    } else {
        uartUsb.printf( "Gas is not being detected\r\n");
    }    
}

static void commandShowCurrentOverTempDetectorState()
{
    if ( alarmOverTempDetectorReadState() ) {
        uartUsb.printf( "Temperature is above the maximum level\r\n");
    } else {
        uartUsb.printf( "Temperature is below the maximum level\r\n");
    }
}

static void commandEnterCodeSequence()
{
    char receivedCodeSequence[ALARM_CODE_NUMBER_OF_KEYS];
    
    uartUsb.printf( "Please enter the new four digits numeric code " );
    uartUsb.printf( "to deactivate the alarm.\r\n" );

    for ( int i = 0; i < ALARM_CODE_NUMBER_OF_KEYS; i++) {
        receivedCodeSequence[i] = uartUsb.getc();
        uartUsb.printf( "*" );
    }

    if ( alarmCodeMatch(receivedCodeSequence) ) {
        uartUsb.printf( "\r\nThe code is correct\r\n\r\n" );
    } else {
        uartUsb.printf( "\r\nThe code is incorrect\r\n\r\n" );
    }    
}

static void commandEnterNewCode()
{
    char newCodeSequence[ALARM_CODE_NUMBER_OF_KEYS];

    uartUsb.printf( "Please enter the new four digits numeric code" );

    for ( int i = 0; i < ALARM_CODE_NUMBER_OF_KEYS; i++) {
        newCodeSequence[i] = uartUsb.getc();
        uartUsb.printf( "*" );
    }

    alarmCodeWrite( newCodeSequence );

    uartUsb.printf( "\r\nNew code configurated\r\n\r\n" ); 
}

static void commandShowCurrentTemperatureInCelsius()
{
    uartUsb.printf( "Temperature: %.2f °C\r\n",
                    temperatureSensorReadCelsius() );    
}

static void commandShowCurrentTemperatureInFahrenheit()
{
    uartUsb.printf( "Temperature: %.2f °F\r\n", 
                    temperatureSensorReadFahrenheit() );    
}

static void commandSetDateAndTime()
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

static void commandShowDateAndTime()
{
    uartUsb.printf("Date and Time = %s", dateAndTimeReadString());
}

static void commandShowStoredEvents()
{
    char str[100];
    for (int i = 0; i < eventLogNumberOfStoredEvents(); i++) {
        eventLogReadStriangAtIndex( i, str );
        uartUsb.printf( "%s\r\n", str );                       
    }
}