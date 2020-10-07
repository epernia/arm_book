//=====[Libraries]=============================================================

#include "mbed.h"
#include "arm_book_lib.h"

#include "pc_serial_com.h"

#include "siren.h"
#include "fire_alarm.h"
#include "code.h"
#include "date_and_time.h"
#include "temperature_sensor.h"
#include "gas_sensor.h"
#include "event_log.h"
#include "sd_card.h"
#include "esp8266_http_server.h"

//=====[Declaration of private defines]======================================



//=====[Declaration of private data types]=====================================

typedef enum{
    PC_SERIAL_GET_FILE_NAME,
    PC_SERIAL_COMMANDS,
    PC_SERIAL_GET_CODE,
    PC_SERIAL_SAVE_NEW_CODE,
} pcSerialComMode_t;

//=====[Declaration and initialization of public global objects]===============

Serial uartUsb(USBTX, USBRX);

//=====[Declaration of external public global variables]=======================

extern char systemBuffer[EVENT_STR_LENGTH*EVENT_LOG_MAX_STORAGE];
extern char ssid[100];
extern char pass[100];

//=====[Declaration and initialization of public global variables]=============

char codeSequenceFromPcSerialCom[CODE_NUMBER_OF_KEYS];

//=====[Declaration and initialization of private global variables]============

static char fileName[40];

static pcSerialComMode_t pcSerialComMode = PC_SERIAL_COMMANDS;
static bool codeComplete = false;
static int numberOfCodeChars = 0;
static int numberOfFileNameChar = 0;

//=====[Declarations (prototypes) of private functions]========================

static void pcSerialComGetCodeUpdate( char receivedChar );
static void pcSerialComSaveNewCodeUpdate( char receivedChar );
static void pcSerialComGetFileName( char receivedChar );
static void pcSerialComShowSdCardFile( char * readBuffer ) ;

static void pcSerialComCommandUpdate( char receivedChar );

static void availableCommands();
static void commandShowCurrentSirenState();
static void commandShowCurrentGasDetectorState();
static void commandShowCurrentOverTemperatureDetectorState();
static void commandGetFileName();
static void commandEnterCodeSequence();
static void commandEnterNewCode();
static void commandShowCurrentTemperatureInCelsius();
static void commandShowCurrentTemperatureInFahrenheit();
static void commandSetDateAndTime();
static void commandShowDateAndTime();
static void commandShowStoredEvents();
static void commandEventLogSaveToSdCard();
static void commandsdCardListFiles();
static void commandSendATCommand();
static void commandGetESPStatus();

//=====[Implementations of public functions]===================================

void pcSerialComInit()
{
    uartUsb.printf("\r\n");
    uartUsb.printf("*Subsection 9.2.1 test program*\r\n");
    uartUsb.printf("\r\n");
    uartUsb.printf("Please enter the SSID of the access point:\r\n");
    uartUsb.printf("> ");
    uartUsb.scanf("%s", ssid);
    uartUsb.printf("%s\r\n", ssid);
    uartUsb.printf("\r\n");
    uartUsb.printf("Please enter the password:\r\n");
    uartUsb.printf("> ");
    uartUsb.scanf("%s", pass);
    uartUsb.printf("%s\r\n", pass);
}

char pcSerialComCharRead()
{
    char receivedChar = '\0';
    if( uartUsb.readable() ) {
        receivedChar = uartUsb.getc();
    }
    return receivedChar;
}

void pcSerialComCharWrite( char c )
{
    uartUsb.putc(c);
}

void pcSerialComStringWrite( const char* str )
{
    uartUsb.printf( "%s", str );
}

void pcSerialComIntWrite( int number )
{
    uartUsb.printf( "%d", number );
}

void pcSerialComUpdate()
{

}

bool pcSerialComCodeCompleteRead()
{
    return codeComplete;
}

void pcSerialComCodeCompleteWrite( bool state )
{
    codeComplete = state;
}

//=====[Implementations of private functions]==================================

static void pcSerialComGetCodeUpdate( char receivedChar )
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

static void pcSerialComSaveNewCodeUpdate( char receivedChar )
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

static void pcSerialComCommandUpdate( char receivedChar )
{
    switch (receivedChar) {
        case '1': commandShowCurrentSirenState(); break;
        case '2': commandShowCurrentGasDetectorState(); break;
        case '3': commandShowCurrentOverTemperatureDetectorState(); break;
        case '4': commandEnterCodeSequence(); break;
        case '5': commandEnterNewCode(); break;
        case 'c': case 'C': commandShowCurrentTemperatureInCelsius(); break;
        case 'f': case 'F': commandShowCurrentTemperatureInFahrenheit(); break;
        case 's': case 'S': commandSetDateAndTime(); break;
        case 't': case 'T': commandShowDateAndTime(); break;
        case 'e': case 'E': commandShowStoredEvents(); break;
        case 'w': case 'W': commandEventLogSaveToSdCard(); break;
        case 'o': case 'O': commandGetFileName(); break;
        case 'l': case 'L': commandsdCardListFiles(); break;
        case 'a': case 'A': commandSendATCommand(); break;
        case 'p': case 'P': commandGetESPStatus(); break;
        default: availableCommands(); break;
    } 
}

static void commandSendATCommand() 
{
    esp8266UartSendAT();
}

static void commandGetESPStatus() 
{
    pcSerialComIntWrite( getEsp8622Status() );
}

static void availableCommands()
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
    uartUsb.printf( "Press 'w' or 'W' to store new events in SD Card\r\n" );
    uartUsb.printf( "Press 'o' or 'O' to show an SD Card file contents\r\n" );
    uartUsb.printf( "Press 'l' or 'L' to list all files in the SD Card\r\n" );
    uartUsb.printf( "\r\n" );
}

static void commandShowCurrentSirenState()
{
    if ( sirenStateRead() ) {
        uartUsb.printf( "The alarmLed is activated\r\n");
    } else {
        uartUsb.printf( "The alarmLed is not activated\r\n");
    }
}

static void commandShowCurrentGasDetectorState()
{
    if ( gasDetectorStateRead() ) {
        uartUsb.printf( "Gas is being detected\r\n"); // Ver de poner la concentracion de gas no este superando el umbral
    } else {
        uartUsb.printf( "Gas is not being detected\r\n");
    }    
}

static void commandShowCurrentOverTemperatureDetectorState()
{
    if ( overTemperatureDetectorStateRead() ) {
        uartUsb.printf( "Temperature is above the maximum level\r\n");
    } else {
        uartUsb.printf( "Temperature is below the maximum level\r\n");
    }
}

static void commandEnterCodeSequence()
{
    if( sirenStateRead() ) {
        uartUsb.printf( "Please enter the four digits numeric code " );
        uartUsb.printf( "to deactivate the alarm.\r\n" );
        pcSerialComMode = PC_SERIAL_GET_CODE;
        codeComplete = false;
        numberOfCodeChars = 0;
    } else {
        uartUsb.printf( "Alarm is not activated.\r\n" );
    }
}

static void commandGetFileName()
{
    uartUsb.printf( "Please enter the file name \r\n" );
    pcSerialComMode = PC_SERIAL_GET_FILE_NAME ;
    numberOfFileNameChar = 0;
}

static void commandEnterNewCode()
{
    uartUsb.printf( "Please enter the new four digits numeric code " );
    uartUsb.printf( "to deactivate the alarm.\r\n" );
    numberOfCodeChars = 0;
    pcSerialComMode = PC_SERIAL_SAVE_NEW_CODE;

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

static void commandEventLogSaveToSdCard()
{
    eventLogSaveToSdCard();
}

static void commandsdCardListFiles()
{
    systemBuffer[0] = NULL;
    sdCardListFiles( systemBuffer, 
                     sizeof(systemBuffer) );
    pcSerialComStringWrite( systemBuffer );
    pcSerialComStringWrite( "\r\n" );
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

static void commandShowDateAndTime()
{
    uartUsb.printf("Date and Time = %s", dateAndTimeRead());
}

static void commandShowStoredEvents()
{
    char str[EVENT_STR_LENGTH];
    int i;
    for (i = 0; i < eventLogNumberOfStoredEvents(); i++) {
        eventLogRead( i, str );
        uartUsb.printf( "%s\r\n", str );                       
    }
}

static void pcSerialComGetFileName( char receivedChar )
{
   if ( receivedChar == '\r' ) {
        pcSerialComMode = PC_SERIAL_COMMANDS;
        fileName[numberOfFileNameChar] = NULL;
        numberOfFileNameChar = 0;
        pcSerialComShowSdCardFile( fileName );
   } else {
    fileName[numberOfFileNameChar] = receivedChar;
    uartUsb.printf( "%c", receivedChar );
    numberOfFileNameChar++;
    }
}

static void pcSerialComShowSdCardFile( char * fileName ) 
{
    systemBuffer[0] = NULL;
    pcSerialComStringWrite( "\r\n" );
    if ( sdCardReadFile( fileName, systemBuffer ) ) {
        pcSerialComStringWrite( "The file content is:\r\n");
        pcSerialComStringWrite( systemBuffer );
        pcSerialComStringWrite( "\r\n" );
    };
}