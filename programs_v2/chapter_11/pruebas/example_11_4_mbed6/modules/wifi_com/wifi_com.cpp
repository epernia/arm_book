//=====[Libraries]=============================================================

#include "arm_book_lib.h"

#include "wifi_com.h"

#include "non_blocking_delay.h"
#include "pc_serial_com.h"
#include "temperature_sensor.h"
#include "siren.h"
#include "fire_alarm.h"
#include "motion_sensor.h"
#include "user_interface.h"

//=====[Declaration of private defines]========================================

#define ESP8266_BAUD_RATE       115200
#define DELAY_10_SECONDS        10000
#define DELAY_5_SECONDS         5000

#define MAX_HTTP_WEB_LENGTH     1500

#define BEGIN_USER_LINE   "<p style=\"text-align: center;\"><b>"
#define END_USER_LINE     "</b></p>"

//=====[Declaration of private data types]=====================================

typedef enum {
   WIFI_STATE_INIT,
   WIFI_STATE_SEND_AT,
   WIFI_STATE_WAIT_AT,
   WIFI_STATE_SEND_CWMODE,
   WIFI_STATE_WAIT_CWMODE,
   WIFI_STATE_SEND_CWJAP_IS_SET,
   WIFI_STATE_WAIT_CWJAP_IS_SET,
   WIFI_STATE_SEND_CWJAP_SET,
   WIFI_STATE_WAIT_CWJAP_SET_1,
   WIFI_STATE_WAIT_CWJAP_SET_2,
   WIFI_STATE_SEND_CIFSR,
   WIFI_STATE_WAIT_CIFSR,
   WIFI_STATE_LOAD_IP,
   WIFI_STATE_SEND_CIPMUX,
   WIFI_STATE_WAIT_CIPMUX,
   WIFI_STATE_SEND_CIPSERVER,
   WIFI_STATE_WAIT_CIPSERVER,
   WIFI_STATE_SEND_CIPSTATUS,
   WIFI_STATE_WAIT_CIPSTATUS_STATUS_3,
   WIFI_STATE_WAIT_CIPSTATUS,
   WIFI_STATE_WAIT_GET_ID,
   WIFI_STATE_WAIT_CIPSTATUS_OK,
   WIFI_STATE_SEND_CIPSEND,
   WIFI_STATE_WAIT_CIPSEND,
   WIFI_STATE_SEND_HTTP,
   WIFI_STATE_WAIT_HTTP,
   WIFI_STATE_SEND_CIPCLOSE,
   WIFI_STATE_WAIT_CIPCLOSE,
   WIFI_STATE_IDLE,
   WIFI_STATE_ERROR
} wifiComState_t;

//=====[Declaration and initialization of public global objects]===============

BufferedSerial uartWifi( PE_8, PE_7, 115200 ); 

//=====[Declaration of external public global variables]=======================

//=====[Declaration and initialization of public global variables]=============

//=====[Declaration and initialization of private global variables]============

static const char responseOk[] = "OK";
static const char responseCwjapOk[] = "+CWJAP:";
static const char responseCwjap1[] = "WIFI CONNECTED";
static const char responseCwjap2[] = "WIFI GOT IP";
static const char responseCifsr[] = "+CIFSR:STAIP,\"";
static const char responseStatus3[] = "STATUS:3";
static const char responseCipstatus[] = "+CIPSTATUS:";
static const char responseSendOk[] = "SEND OK";
static const char responseCipclose[] = "CLOSED";

static int currentConnectionId;
static char wifiComApSsid[30] = "";
static char wifiComApPassword[30] = "";
static char wifiComIpAddress[20];
static char stateString[4];

static const char * wifiComExpectedResponse;
static wifiComState_t wifiComState;

static nonBlockingDelay_t wifiComDelay;

static const char httpWebPageHeader [] =
   "<!doctype html>"
   "<html> <head> <title>Smart Home System</title> </head>"
   "<body> <h1 style=\"text-align: center;\">"
   "<font color=\"#0000ff\">Smart Home System</font></h1>"
   ;

static const char httpWebPageEnd [] = "</body> </html>";

static char httpWebPageBody[450];

//=====[Declarations (prototypes) of private functions]========================

static bool isExpectedResponse();
bool wifiComCharRead( char* receivedChar );
void wifiComStringWrite( char const* str );
void wifiComWebPageDataUpdate();
char * stateToString( bool state );

//=====[Implementations of public functions]===================================

void wifiComSetWiFiComApSsid( char * APSsid )
{
	strcpy(wifiComApSsid, APSsid);
}

void wifiComSetWiFiComApPassword( char * APPassword )
{
    strcpy(wifiComApPassword, APPassword);
}

char * wifiComGetIpAddress()
{
   return wifiComIpAddress;
}

void wifiComRestart()
{
	wifiComState = WIFI_STATE_INIT;
}

void wifiComInit()
{
    wifiComState = WIFI_STATE_INIT;
}

void wifiComUpdate()
{
   int lengthOfHttpLines;
   static char receivedCharWifiCom;
   static int IpStringPositionIndex;
   char strToSend[50];

   switch (wifiComState) {

      case WIFI_STATE_INIT:
         nonBlockingDelayWrite(&wifiComDelay, DELAY_5_SECONDS);
         wifiComState = WIFI_STATE_SEND_AT;
      break;

      case WIFI_STATE_SEND_AT:
         if (nonBlockingDelayRead(&wifiComDelay)) {
            wifiComStringWrite( "AT\r\n" );
            wifiComExpectedResponse = responseOk;
            nonBlockingDelayWrite(&wifiComDelay, DELAY_5_SECONDS);
            wifiComState = WIFI_STATE_WAIT_AT;
         }
      break;

      case WIFI_STATE_WAIT_AT:
         if (isExpectedResponse()) {
            nonBlockingDelayWrite(&wifiComDelay, DELAY_5_SECONDS);
            wifiComState = WIFI_STATE_SEND_CWMODE;
         }
         if (nonBlockingDelayRead(&wifiComDelay)) {
            pcSerialComStringWrite("AT command not responded ");
            pcSerialComStringWrite("correctly\r\n");
            wifiComState = WIFI_STATE_ERROR;
         }
      break;

      case WIFI_STATE_SEND_CWMODE:
         if (nonBlockingDelayRead(&wifiComDelay)) {
            wifiComStringWrite( "AT+CWMODE=1\r\n" );
            wifiComExpectedResponse = responseOk;
            nonBlockingDelayWrite(&wifiComDelay, DELAY_5_SECONDS);
            wifiComState = WIFI_STATE_WAIT_CWMODE;
         }
      break;

      case WIFI_STATE_WAIT_CWMODE:
         if (isExpectedResponse()) {
            nonBlockingDelayWrite(&wifiComDelay, DELAY_5_SECONDS);
            wifiComState = WIFI_STATE_SEND_CWJAP_IS_SET;
         }
         if (nonBlockingDelayRead(&wifiComDelay)) {
            pcSerialComStringWrite("AT+CWMODE=1 command not ");
            pcSerialComStringWrite("responded correctly\r\n");
            wifiComState = WIFI_STATE_ERROR;
         }
      break;

      case WIFI_STATE_SEND_CWJAP_IS_SET:
         if (nonBlockingDelayRead(&wifiComDelay)) {
            wifiComStringWrite( "AT+CWJAP?\r\n" );
            wifiComExpectedResponse = responseCwjapOk;
            nonBlockingDelayWrite(&wifiComDelay, DELAY_5_SECONDS);
            wifiComState = WIFI_STATE_WAIT_CWJAP_IS_SET;
         }
      break;

      case WIFI_STATE_WAIT_CWJAP_IS_SET:
         if (isExpectedResponse()) {
            wifiComExpectedResponse = responseOk;
            wifiComState = WIFI_STATE_SEND_CIFSR;
         }
         if (nonBlockingDelayRead(&wifiComDelay)) {
            nonBlockingDelayWrite(&wifiComDelay, DELAY_5_SECONDS);
            wifiComState = WIFI_STATE_SEND_CWJAP_SET;
         }
      break;

      case WIFI_STATE_SEND_CWJAP_SET:
         if (nonBlockingDelayRead(&wifiComDelay)) {
            wifiComStringWrite( "AT+CWJAP=\"" );
            wifiComStringWrite( wifiComApSsid );
            wifiComStringWrite( "\",\"" );
            wifiComStringWrite( wifiComApPassword );
            wifiComStringWrite( "\"" );
            wifiComStringWrite( "\r\n" );
            wifiComExpectedResponse = responseCwjap1;
            nonBlockingDelayWrite(&wifiComDelay, DELAY_10_SECONDS);
            wifiComState = WIFI_STATE_WAIT_CWJAP_SET_1;
         }
      break;

      case WIFI_STATE_WAIT_CWJAP_SET_1:
         if (isExpectedResponse()) {
            wifiComExpectedResponse = responseCwjap2;
            wifiComState = WIFI_STATE_WAIT_CWJAP_SET_2;
         }
         if (nonBlockingDelayRead(&wifiComDelay)) {
            pcSerialComStringWrite("Error in state: ");
            pcSerialComStringWrite("WIFI_STATE_WAIT_CWJAP_SET_1\r\n");
            pcSerialComStringWrite("Check Wi-Fi AP credentials ");
            pcSerialComStringWrite("and restart\r\n");
            wifiComState = WIFI_STATE_ERROR;
         }
         break;

      case WIFI_STATE_WAIT_CWJAP_SET_2:
         if (isExpectedResponse()) {
            wifiComState = WIFI_STATE_SEND_CIFSR;
         }
         if (nonBlockingDelayRead(&wifiComDelay)) {
            pcSerialComStringWrite("Error in state: ");
            pcSerialComStringWrite("WIFI_STATE_WAIT_CWJAP_SET_2\r\n");
            pcSerialComStringWrite("Check Wi-Fi AP credentials ");
            pcSerialComStringWrite("and restart\r\n");
            wifiComState = WIFI_STATE_ERROR;
         }
      break;

      case WIFI_STATE_SEND_CIFSR:
         if (nonBlockingDelayRead(&wifiComDelay)) {
            wifiComStringWrite( "AT+CIFSR\r\n" );
            wifiComExpectedResponse = responseCifsr;
            nonBlockingDelayWrite(&wifiComDelay, DELAY_5_SECONDS);
            wifiComState = WIFI_STATE_WAIT_CIFSR;
         }
      break;

      case WIFI_STATE_WAIT_CIFSR:
         if (isExpectedResponse()) {
            wifiComState = WIFI_STATE_LOAD_IP;
            IpStringPositionIndex = 0;
         }
         if (nonBlockingDelayRead(&wifiComDelay)) {
            pcSerialComStringWrite("AT+CIFSR command not responded ");
            pcSerialComStringWrite("correctly\r\n");
            wifiComState = WIFI_STATE_ERROR;
         }
      break;

      case WIFI_STATE_LOAD_IP:
         if( wifiComCharRead(&receivedCharWifiCom) ){
            if (receivedCharWifiCom != '"') {
               wifiComIpAddress[IpStringPositionIndex] = receivedCharWifiCom;
               IpStringPositionIndex++;
            } else {
               wifiComIpAddress[IpStringPositionIndex] = '\0';
               pcSerialComStringWrite("IP address assigned correctly\r\n\r\n");
               wifiComState = WIFI_STATE_SEND_CIPMUX;
            }
         }
      break;

      case WIFI_STATE_SEND_CIPMUX:
         if (nonBlockingDelayRead(&wifiComDelay)) {
            wifiComStringWrite( "AT+CIPMUX=1\r\n" );
            wifiComExpectedResponse = responseOk;
            nonBlockingDelayWrite(&wifiComDelay, DELAY_5_SECONDS);
            wifiComState = WIFI_STATE_WAIT_CIPMUX;
         }
      break;

      case WIFI_STATE_WAIT_CIPMUX:
         if (isExpectedResponse()) {
            nonBlockingDelayWrite(&wifiComDelay, DELAY_5_SECONDS);
            wifiComState = WIFI_STATE_SEND_CIPSERVER;
         }
         if (nonBlockingDelayRead(&wifiComDelay)) {
            pcSerialComStringWrite("AT+CIPMUX=1 command not ");
            pcSerialComStringWrite("responded correctly\r\n");
            wifiComState = WIFI_STATE_ERROR;
         }
      break;

      case WIFI_STATE_SEND_CIPSERVER:
         if (nonBlockingDelayRead(&wifiComDelay)) {
            wifiComStringWrite( "AT+CIPSERVER=1,80\r\n" );
            wifiComExpectedResponse = responseOk;
            nonBlockingDelayWrite(&wifiComDelay, DELAY_5_SECONDS);
            wifiComState = WIFI_STATE_WAIT_CIPSERVER;
         }
      break;

      case WIFI_STATE_WAIT_CIPSERVER:
         if (isExpectedResponse()) {
            nonBlockingDelayWrite(&wifiComDelay, DELAY_5_SECONDS);
            wifiComState = WIFI_STATE_SEND_CIPSTATUS;
         }
         if (nonBlockingDelayRead(&wifiComDelay)) {
         pcSerialComStringWrite("AT+CIPSERVER=1,80 command not ");
         pcSerialComStringWrite("responded correctly\r\n");
         wifiComState = WIFI_STATE_ERROR;
         }
      break;

      case WIFI_STATE_SEND_CIPSTATUS:
         if (nonBlockingDelayRead(&wifiComDelay)) {
            wifiComStringWrite( "AT+CIPSTATUS\r\n" );
            wifiComExpectedResponse = responseStatus3;
            nonBlockingDelayWrite(&wifiComDelay, DELAY_5_SECONDS);
            wifiComState = WIFI_STATE_WAIT_CIPSTATUS_STATUS_3;
         }
      break;

      case WIFI_STATE_WAIT_CIPSTATUS_STATUS_3:
         if (isExpectedResponse()) {
            nonBlockingDelayWrite(&wifiComDelay, DELAY_5_SECONDS);
            wifiComExpectedResponse = responseCipstatus;
            wifiComState = WIFI_STATE_WAIT_CIPSTATUS;
         }
         if (nonBlockingDelayRead(&wifiComDelay)) {
            nonBlockingDelayWrite(&wifiComDelay, DELAY_5_SECONDS);
            wifiComState = WIFI_STATE_SEND_CIPSTATUS;
         }
      break;

      case WIFI_STATE_WAIT_CIPSTATUS:
         if (isExpectedResponse()) {
            wifiComState = WIFI_STATE_WAIT_GET_ID;
         }
         if (nonBlockingDelayRead(&wifiComDelay)) {
            nonBlockingDelayWrite(&wifiComDelay, DELAY_5_SECONDS);
            wifiComState = WIFI_STATE_SEND_CIPSTATUS;
         }
      break;

      case WIFI_STATE_WAIT_GET_ID:
         if( wifiComCharRead(&receivedCharWifiCom) ){
            currentConnectionId = receivedCharWifiCom;
            wifiComExpectedResponse = responseOk;
            wifiComState = WIFI_STATE_WAIT_CIPSTATUS_OK;
         }
      break;

      case WIFI_STATE_WAIT_CIPSTATUS_OK:
         if (isExpectedResponse()) {
            wifiComState = WIFI_STATE_SEND_CIPSEND;
            wifiComWebPageDataUpdate();
         }
         if (nonBlockingDelayRead(&wifiComDelay)) {
            nonBlockingDelayWrite(&wifiComDelay, DELAY_5_SECONDS);
            wifiComState = WIFI_STATE_SEND_CIPSTATUS;
         }
      break;

      case WIFI_STATE_SEND_CIPSEND:
         lengthOfHttpLines = ( strlen(httpWebPageHeader) 
                               + strlen(httpWebPageBody) 
                               + strlen(httpWebPageEnd) );
         sprintf( strToSend, "AT+CIPSEND=%c,%d\r\n", 
                  currentConnectionId, lengthOfHttpLines );
         wifiComStringWrite( strToSend );
         wifiComState = WIFI_STATE_WAIT_CIPSEND;
         wifiComExpectedResponse = responseOk;
      break;

      case WIFI_STATE_WAIT_CIPSEND:
         if (isExpectedResponse()) {
            nonBlockingDelayWrite(&wifiComDelay, DELAY_5_SECONDS);
            wifiComState = WIFI_STATE_SEND_HTTP;
         }
         if (nonBlockingDelayRead(&wifiComDelay)) {
            nonBlockingDelayWrite(&wifiComDelay, DELAY_5_SECONDS);
            wifiComState = WIFI_STATE_SEND_CIPSTATUS;
         }
      break;

      case WIFI_STATE_SEND_HTTP:
        wifiComStringWrite( httpWebPageHeader );
        wifiComStringWrite( httpWebPageBody );
        wifiComStringWrite( httpWebPageEnd );
        wifiComState = WIFI_STATE_WAIT_HTTP;
        wifiComExpectedResponse = responseSendOk;
      break;

      case WIFI_STATE_WAIT_HTTP:
         if (isExpectedResponse()) {
            nonBlockingDelayWrite(&wifiComDelay, DELAY_5_SECONDS);
            wifiComState = WIFI_STATE_SEND_CIPCLOSE;
         }
         if (nonBlockingDelayRead(&wifiComDelay)) {
            nonBlockingDelayWrite(&wifiComDelay, DELAY_5_SECONDS);
            wifiComState = WIFI_STATE_SEND_CIPSEND;
         }
      break;

      case WIFI_STATE_SEND_CIPCLOSE:
         if (nonBlockingDelayRead(&wifiComDelay)) {
            sprintf( strToSend, "AT+CIPCLOSE=%c\r\n", currentConnectionId );
            wifiComStringWrite( strToSend );
            wifiComExpectedResponse  = responseCipclose;
            nonBlockingDelayWrite(&wifiComDelay, DELAY_5_SECONDS);
            wifiComState = WIFI_STATE_WAIT_CIPCLOSE;
         }
      break;

      case WIFI_STATE_WAIT_CIPCLOSE:
         if (isExpectedResponse()) {
            nonBlockingDelayWrite(&wifiComDelay, DELAY_5_SECONDS);
            wifiComState = WIFI_STATE_SEND_CIPSTATUS;
         }
         if (nonBlockingDelayRead(&wifiComDelay)) {
            nonBlockingDelayWrite(&wifiComDelay, DELAY_5_SECONDS);
            wifiComState = WIFI_STATE_SEND_CIPSTATUS;
         }
      break;

      case WIFI_STATE_IDLE:
      case WIFI_STATE_ERROR:
      break;
   }
}

//=====[Implementations of private functions]==================================

bool wifiComCharRead( char* receivedChar )
{
    char receivedCharLocal = '\0';
    if( uartWifi.readable() ) {
        uartWifi.read(&receivedCharLocal,1);
        *receivedChar = receivedCharLocal;
        return true;
    }
    return false;
}

void wifiComStringWrite( char const* str )
{
    uartWifi.write( str, strlen(str) );
}

static bool isExpectedResponse()
{
   static int responseStringPositionIndex = 0;
   char charReceived;
   bool moduleResponse = FALSE;

   if( wifiComCharRead(&charReceived) ){
      if (charReceived == wifiComExpectedResponse[responseStringPositionIndex]) {
         responseStringPositionIndex++;
         if (wifiComExpectedResponse[responseStringPositionIndex] == '\0') {
            responseStringPositionIndex = 0;
            moduleResponse = TRUE;
         }
      } else {
         responseStringPositionIndex = 0;
      }
   }
   return moduleResponse;
}

void wifiComWebPageDataUpdate()
{
    sprintf( httpWebPageBody, "%s Temperature: %.2f °C %s", 
             BEGIN_USER_LINE, temperatureSensorReadCelsius(), END_USER_LINE );
    
    sprintf( httpWebPageBody + strlen(httpWebPageBody), "%s Alarm: %s %s", 
             BEGIN_USER_LINE, stateToString( sirenStateRead() ), END_USER_LINE );
    
    sprintf( httpWebPageBody + strlen(httpWebPageBody), "%s Gas detected: %s %s", 
             BEGIN_USER_LINE, stateToString( gasDetectorStateRead() ), 
             END_USER_LINE );
    
    sprintf( httpWebPageBody + strlen(httpWebPageBody), 
             "%s Over temperature detected: %s %s", BEGIN_USER_LINE, 
             stateToString( overTemperatureDetectorStateRead() ), END_USER_LINE );
    
    sprintf( httpWebPageBody + strlen(httpWebPageBody), 
             "%s Incorrect code led: %s %s", BEGIN_USER_LINE, 
             stateToString( incorrectCodeStateRead() ), END_USER_LINE );
    
    sprintf( httpWebPageBody + strlen(httpWebPageBody), 
             "%s System blocked led: %s %s", BEGIN_USER_LINE, 
             stateToString( systemBlockedStateRead() ), END_USER_LINE );
    
    sprintf( httpWebPageBody + strlen(httpWebPageBody), 
             "%s Motion: %s %s", BEGIN_USER_LINE, 
             stateToString( motionSensorRead() ), END_USER_LINE );
}

char * stateToString( bool state )
{    
    if ( state ) {
        strcpy( stateString, "ON");
    } else {
        strcpy( stateString, "OFF");
    }
    return stateString;
}