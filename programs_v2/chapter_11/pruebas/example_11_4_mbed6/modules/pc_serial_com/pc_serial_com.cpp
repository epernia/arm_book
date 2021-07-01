//=====[Libraries]=============================================================

#include "arm_book_lib.h"

#include "pc_serial_com.h"

#include "alarm.h"
#include "siren.h"
#include "fire_alarm.h"
#include "code.h"
#include "date_and_time.h"
#include "temperature_sensor.h"
#include "motion_sensor.h"
#include "gas_sensor.h"
#include "event_log.h"
#include "motor.h"
#include "gate.h"
#include "sd_card.h"
#include "wifi_com.h"

//=====[Declaration of private defines]=======================================

//=====[Declaration of private data types]=====================================

typedef enum{
    PC_SERIAL_GET_FILE_NAME,
    PC_SERIAL_COMMANDS,
    PC_SERIAL_GET_CODE,
    PC_SERIAL_SAVE_NEW_CODE,
    PC_SERIAL_GET_WIFI_AP_CREDENTIALS_SSID,
    PC_SERIAL_GET_WIFI_AP_CREDENTIALS_PASSWORD,
} pcSerialComMode_t;

//=====[Declaration and initialization of public global objects]===============

UnbufferedSerial uartUsb(USBTX, USBRX,115200);

//=====[Declaration of external public global variables]=======================

extern char systemBuffer[EVENT_STR_LENGTH*EVENT_LOG_MAX_STORAGE];

//=====[Declaration and initialization of public global variables]=============

char codeSequenceFromPcSerialCom[CODE_NUMBER_OF_KEYS];

//=====[Declaration and initialization of private global variables]============

static pcSerialComMode_t pcSerialComMode = PC_SERIAL_COMMANDS;
static bool codeComplete = false;
static int numberOfCodeChars = 0;
static int numberOfCharsInFileName = 0;
static int numberOfCharsInAPCredentials = 0;

static char fileName[40];
static char APSsid[40];
static char APPassword[40];

//=====[Declarations (prototypes) of private functions]========================

static void pcSerialComStringRead( char* str, int strLength );

static void pcSerialComGetCodeUpdate( char receivedChar );
static void pcSerialComSaveNewCodeUpdate( char receivedChar );
static void pcSerialComGetFileName( char receivedChar );
static void pcSerialComGetWiFiComApSsid( char receivedChar );
static void pcSerialComGetWiFiComApPassword( char receivedChar );
static void pcSerialComShowSdCardFile( char * readBuffer ) ;

static void pcSerialComCommandUpdate( char receivedChar );

static void availableCommands();
static void commandShowCurrentSirenState();
static void commandShowCurrentMotorState();
static void commandShowCurrentGateState();
static void commandShowCurrentGasDetectorState();
static void commandShowCurrentOverTemperatureDetectorState();
static void commandEnterCodeSequence();
static void commandEnterNewCode();
static void commandShowCurrentTemperatureInCelsius();
static void commandShowCurrentTemperatureInFahrenheit();
static void commandSetDateAndTime();
static void commandShowStoredEvents();
static void commandShowDateAndTime();
static void commandMotionSensorActivate();
static void commandMotionSensorDeactivate();
static void commandEventLogSaveToSdCard();
static void commandsdCardListFiles();
static void commandGetFileName();
static void commandRestartWifiCom();
static void commandSetWifiComApSsid();
static void commandSetWifiComApPassword();
static void commandGetWifiComAssignedIp();

//=====[Implementations of public functions]===================================

void pcSerialComInit()
{
    availableCommands();
}

char pcSerialComCharRead()
{
    char receivedChar = '\0';
    if( uartUsb.readable() ) {
        uartUsb.read(&receivedChar,1);
    }
    return receivedChar;
}

void pcSerialComStringWrite( const char* str )
{
    uartUsb.write( str, strlen(str) );
}

void pcSerialComCharWrite( char chr )
{
    char str[2];
    sprintf (str, "%c", chr);
    uartUsb.write( str, strlen(str) );
}

void pcSerialComIntWrite( int number )
{
    char str[50];
    sprintf( str, "%d", number );
    uartUsb.write( str, strlen(str) );
}

void pcSerialComUpdate()
{
    char receivedChar = pcSerialComCharRead();
    if( receivedChar != '\0' ) {
        switch ( pcSerialComMode ) {
            case PC_SERIAL_GET_FILE_NAME:
                pcSerialComGetFileName( receivedChar );
            break;
            case PC_SERIAL_COMMANDS:
                pcSerialComCommandUpdate( receivedChar );
            break;

            case PC_SERIAL_GET_CODE:
                pcSerialComGetCodeUpdate( receivedChar );
            break;

            case PC_SERIAL_SAVE_NEW_CODE:
                pcSerialComSaveNewCodeUpdate( receivedChar );
            break;
            case PC_SERIAL_GET_WIFI_AP_CREDENTIALS_SSID:
                pcSerialComGetWiFiComApSsid( receivedChar );
            break;
            case PC_SERIAL_GET_WIFI_AP_CREDENTIALS_PASSWORD:
                pcSerialComGetWiFiComApPassword( receivedChar );
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

//=====[Implementations of private functions]==================================

static void pcSerialComStringRead( char* str, int strLength )
{
    int strIndex;
    for ( strIndex = 0; strIndex < strLength; strIndex++) {
        uartUsb.read( &str[strIndex] , 1 );
        uartUsb.write( &str[strIndex] ,1 );
    }
    str[strLength]='\0';
    pcSerialComStringWrite("\r\n");
    pcSerialComStringWrite( str );
    pcSerialComStringWrite("\r\n");
}

static void pcSerialComGetCodeUpdate( char receivedChar )
{
    codeSequenceFromPcSerialCom[numberOfCodeChars] = receivedChar;
    pcSerialComStringWrite( "*" );
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
    pcSerialComStringWrite( "*" );
    numberOfCodeChars++;
    if ( numberOfCodeChars >= CODE_NUMBER_OF_KEYS ) {
        pcSerialComMode = PC_SERIAL_COMMANDS;
        numberOfCodeChars = 0;
        codeWrite( newCodeSequence );
        pcSerialComStringWrite( "\r\nNew code configured\r\n\r\n" );
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
        case 'm': case 'M': commandShowCurrentMotorState(); break;
        case 'g': case 'G': commandShowCurrentGateState(); break;
        case 'i': case 'I': commandMotionSensorActivate(); break;
        case 'h': case 'H': commandMotionSensorDeactivate(); break;
        case 'w': case 'W': commandEventLogSaveToSdCard(); break;
        case 'l': case 'L': commandsdCardListFiles(); break;
        case 'o': case 'O': commandGetFileName(); break;
        case 'a': case 'A': commandRestartWifiCom(); break;
        case 'd': case 'D': commandSetWifiComApSsid(); break;
        case 'r': case 'R': commandSetWifiComApPassword(); break;
        case 'p': case 'P': commandGetWifiComAssignedIp(); break;
        default: availableCommands(); break;
    } 
}

static void availableCommands()
{
    pcSerialComStringWrite( "Available commands:\r\n" );
    pcSerialComStringWrite( "Press '1' to get the alarm state\r\n" );
    pcSerialComStringWrite( "Press '2' for gas detector state\r\n" );
    pcSerialComStringWrite( "Press '3' for over temperature detector state\r\n" );
    pcSerialComStringWrite( "Press '4' to enter the code to deactivate the alarm\r\n" );
    pcSerialComStringWrite( "Press '5' to enter a new code to deactivate the alarm\r\n" );
    pcSerialComStringWrite( "Press 'f' or 'F' to get lm35 reading in Fahrenheit\r\n" );
    pcSerialComStringWrite( "Press 'c' or 'C' to get lm35 reading in Celsius\r\n" );
    pcSerialComStringWrite( "Press 's' or 'S' to set the date and time\r\n" );
    pcSerialComStringWrite( "Press 't' or 'T' to get the date and time\r\n" );
    pcSerialComStringWrite( "Press 'e' or 'E' to get the stored events\r\n" );
    pcSerialComStringWrite( "Press 'm' or 'M' to show the motor status\r\n" );
    pcSerialComStringWrite( "Press 'g' or 'G' to show the gate status\r\n" );
    pcSerialComStringWrite( "Press 'I' or 'I' to activate the motion sensor\r\n" );
    pcSerialComStringWrite( "Press 'h' or 'H' to deactivate the motion sensor\r\n" );
    pcSerialComStringWrite( "Press 'w' or 'W' to store the events log in the SD card\r\n" );
    pcSerialComStringWrite( "Press 'l' or 'L' to list all the files in the root directory of the SD card\r\n" );
    pcSerialComStringWrite( "Press 'o' or 'O' to show an SD Card file contents\r\n" );
    pcSerialComStringWrite( "Press 'a' or 'A' to restart the Wi-Fi communication\r\n" );
    pcSerialComStringWrite( "Press 'd' or 'D' to set Wi-Fi AP SSID\r\n" );
    pcSerialComStringWrite( "Press 'r' or 'R' to set Wi-Fi AP Password\r\n" );
    pcSerialComStringWrite( "Press 'p' or 'P' to get Wi-Fi assigned IP\r\n" );
    pcSerialComStringWrite( "\r\n" );
}

static void commandShowCurrentSirenState()
{
    if ( alarmStateRead() ) {
        pcSerialComStringWrite( "The alarm is activated\r\n");
    } else {
        pcSerialComStringWrite( "The alarm is not activated\r\n");
    }
}

static void commandShowCurrentMotorState()
{
    switch ( motorDirectionRead() ) {
        case STOPPED: pcSerialComStringWrite( "The motor is stopped\r\n"); break;
        case DIRECTION_1: 
            pcSerialComStringWrite( "The motor is turning in direction 1\r\n"); break;
        case DIRECTION_2: 
            pcSerialComStringWrite( "The motor is turning in direction 2\r\n"); break;
    }

}

static void commandShowCurrentGateState()
{
    switch ( gateStatusRead() ) {
        case GATE_CLOSED: pcSerialComStringWrite( "The gate is closed\r\n"); break;
        case GATE_OPEN: pcSerialComStringWrite( "The gate is open\r\n"); break;
        case GATE_OPENING: pcSerialComStringWrite( "The gate is opening\r\n"); break;
        case GATE_CLOSING: pcSerialComStringWrite( "The gate is closing\r\n"); break;
    }
}

static void commandShowCurrentGasDetectorState()
{
    if ( gasDetectorStateRead() ) {
        pcSerialComStringWrite( "Gas is being detected\r\n"); // Ver de poner la concentracion de gas no este superando el umbral
    } else {
        pcSerialComStringWrite( "Gas is not being detected\r\n");
    }    
}

static void commandMotionSensorActivate()
{
    motionSensorActivate();
}

static void commandMotionSensorDeactivate()
{
    motionSensorDeactivate();
}

static void commandShowCurrentOverTemperatureDetectorState()
{
    if ( overTemperatureDetectorStateRead() ) {
        pcSerialComStringWrite( "Temperature is above the maximum level\r\n");
    } else {
        pcSerialComStringWrite( "Temperature is below the maximum level\r\n");
    }
}

static void commandEnterCodeSequence()
{
    if( alarmStateRead() ) {
        pcSerialComStringWrite( "Please enter the four digits numeric code " );
        pcSerialComStringWrite( "to deactivate the alarm: " );
        pcSerialComMode = PC_SERIAL_GET_CODE;
        codeComplete = false;
        numberOfCodeChars = 0;
    } else {
        pcSerialComStringWrite( "Alarm is not activated.\r\n" );
    }
}

static void commandEnterNewCode()
{
    pcSerialComStringWrite( "Please enter the new four digits numeric code " );
    pcSerialComStringWrite( "to deactivate the alarm: " );
    numberOfCodeChars = 0;
    pcSerialComMode = PC_SERIAL_SAVE_NEW_CODE;
}

static void commandShowCurrentTemperatureInCelsius()
{
    char str[100];
    sprintf ( str, "Temperature: %.2f °C\r\n",
                    temperatureSensorReadCelsius() );
    pcSerialComStringWrite( str );  
}

static void commandShowCurrentTemperatureInFahrenheit()
{
    char str[100];
    sprintf ( str, "Temperature: %.2f °C\r\n",
                    temperatureSensorReadFahrenheit() );
    pcSerialComStringWrite( str );  
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
    char year[4];
    char month[2];
    char day[2];
    char hour[2];
    char minute[2];
    char second[2];
    
    pcSerialComStringWrite("\r\nType de current year (YYYY) and press enter: ");
    pcSerialComStringRead( year, 4);

    pcSerialComStringWrite("Type de current month (1-12) and press enter: ");
    pcSerialComStringRead( month, 2);

    pcSerialComStringWrite("Type de current day (1-31) and press enter: ");
    pcSerialComStringRead( day, 2);

    pcSerialComStringWrite("Type de current hour (0-23) and press enter: ");
    pcSerialComStringRead( hour, 2);

    pcSerialComStringWrite("Type de current minutes (0-59) and press enter: ");
    pcSerialComStringRead( minute, 2);

    pcSerialComStringWrite("Type de current seconds (0-59) and press enter: ");
    pcSerialComStringRead( second, 2);
    
    pcSerialComStringWrite("Date and time has been set\r\n");

    dateAndTimeWrite( atoi(year), atoi(month), atoi(day), atoi(hour), atoi(minute), atoi(second) );
}

static void commandShowDateAndTime()
{
    char str[100];
    sprintf ( str, "Date and Time = %s", dateAndTimeRead() );
    pcSerialComStringWrite( str );
}

static void commandShowStoredEvents()
{
    char str[EVENT_STR_LENGTH];
    int i;
    for (i = 0; i < eventLogNumberOfStoredEvents(); i++) {
        eventLogRead( i, str );
        pcSerialComStringWrite( str );   
        pcSerialComStringWrite( "\r\n" );                    
    }
}

static void commandGetFileName()
{
    pcSerialComStringWrite( "Please enter the file name \r\n" );
    pcSerialComMode = PC_SERIAL_GET_FILE_NAME ;
    numberOfCharsInFileName = 0;
}

static void commandRestartWifiCom()
{
    pcSerialComStringWrite( "Wi-Fi communication restarted \r\n" );
    wifiComRestart();
}

static void commandSetWifiComApSsid()
{
    pcSerialComStringWrite("\r\nPlease provide the SSID of the Wi-Fi "); 
    pcSerialComStringWrite("Access Point and press the Enter key\r\n");
    pcSerialComStringWrite("> ");
    pcSerialComMode = PC_SERIAL_GET_WIFI_AP_CREDENTIALS_SSID;
    numberOfCharsInAPCredentials = 0;
}

static void commandSetWifiComApPassword()
{
    pcSerialComStringWrite("\r\nPlease provide the  APPassword of the Wi-Fi "); 
    pcSerialComStringWrite("Access Point and press the Enter key\r\n");
    pcSerialComStringWrite("> ");
    pcSerialComMode = PC_SERIAL_GET_WIFI_AP_CREDENTIALS_PASSWORD;
    numberOfCharsInAPCredentials = 0;
}

static void commandGetWifiComAssignedIp()
{
    pcSerialComStringWrite( "The assigned IP is: " );
    pcSerialComStringWrite( wifiComGetIpAddress() );
    pcSerialComStringWrite( "\r\n" );
}

static void pcSerialComGetFileName( char receivedChar )
{
    if ( receivedChar == '\r' ) {
        pcSerialComMode = PC_SERIAL_COMMANDS;
        fileName[numberOfCharsInFileName] = NULL;
        numberOfCharsInFileName = 0;
        pcSerialComShowSdCardFile( fileName );
    } else {
        fileName[numberOfCharsInFileName] = receivedChar;
        pcSerialComCharWrite( receivedChar );
        numberOfCharsInFileName++;
    }
}

static void pcSerialComGetWiFiComApSsid( char receivedChar )
{
    if ( receivedChar == '\r' ) {
        pcSerialComMode = PC_SERIAL_COMMANDS;
        APSsid[numberOfCharsInAPCredentials] = NULL;
        wifiComSetWiFiComApSsid(APSsid);
        pcSerialComStringWrite( "\r\nWi-Fi Access Point SSID configured\r\n\r\n" );
    } else {
        APSsid[numberOfCharsInAPCredentials] = receivedChar;
        pcSerialComCharWrite( receivedChar );
        numberOfCharsInAPCredentials++;
    }
}

static void pcSerialComGetWiFiComApPassword( char receivedChar )
{
    if ( receivedChar == '\r' ) {
        pcSerialComMode = PC_SERIAL_COMMANDS;
        APPassword[numberOfCharsInAPCredentials] = NULL;
        wifiComSetWiFiComApPassword(APPassword);
        pcSerialComStringWrite( "\r\nWi-Fi Access Point password configured\r\n\r\n" );
    } else {
        APPassword[numberOfCharsInAPCredentials] = receivedChar;
        pcSerialComStringWrite( "*" );
        numberOfCharsInAPCredentials++;
    }
}

static void pcSerialComShowSdCardFile( char* fileName ) 
{
    systemBuffer[0] = NULL;
    pcSerialComStringWrite( "\r\n" );
    if ( sdCardReadFile( fileName, systemBuffer ) ) {
        pcSerialComStringWrite( "The file content is:\r\n");
        pcSerialComStringWrite( systemBuffer );
        pcSerialComStringWrite( "\r\n" );
    }
}