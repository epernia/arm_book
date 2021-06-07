//=====[Libraries]=============================================================

#include "arm_book_lib.h"

#include "wifi_com.h"

#include "non_blocking_delay.h"
#include "pc_serial_com.h"

//=====[Declaration of private defines]========================================

#define ESP8266_BAUD_RATE       115200
#define DELAY_10_SECONDS        10000
#define DELAY_5_SECONDS         5000

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

Serial uartEsp8266( PE_8, PE_7 ); 

//=====[Declaration of external public global variables]=======================

//=====[Declaration and initialization of public global variables]=============

//=====[Declaration and initialization of private global variables]============

static const char Response_OK[] = "OK";
static const char Response_CWJAP_OK[] = "+CWJAP:";
static const char Response_CWJAP_1[] = "WIFI CONNECTED";
static const char Response_CWJAP_2[] = "WIFI GOT IP";
static const char Response_CIFSR[] = "+CIFSR:STAIP,\"";
static const char Response_STATUS_3[] = "STATUS:3";
static const char Response_CIPSTATUS[] = "+CIPSTATUS:";
static const char Response_SEND_OK[] = "SEND OK";
static const char Response_CIPCLOSE[] = "CLOSED";

static int CurrentConnectionId;
static char wifiComApSsid[30] = "";
static char wifiComApPassword[30] = "";
static char wifiComIpAddress[20];

static const char * wifiComExpectedResponse;
static wifiComState_t wifiComState;

static nonBlockingDelay_t wifiComDelay;

static const char httpWebPage [] =
   "<!doctype html> <html> <body> Hello! </body> </html>"
   ;

//=====[Declarations (prototypes) of private functions]========================

static bool isExpectedResponse();
bool wifiComCharRead( char* receivedChar );
void wifiComStringWrite( char const* str );

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
    uartEsp8266.baud(ESP8266_BAUD_RATE);
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
            wifiComExpectedResponse = Response_OK;
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
         pcSerialComStringWrite("AT command not responded correctly\r\n\r\n");
         wifiComState = WIFI_STATE_ERROR;
         }
      break;

      case WIFI_STATE_SEND_CWMODE:
         if (nonBlockingDelayRead(&wifiComDelay)) {
            wifiComStringWrite( "AT+CWMODE=1\r\n" );
            wifiComExpectedResponse = Response_OK;
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
         pcSerialComStringWrite("AT+CWMODE=1 command not responded correctly\r\n\r\n");
         wifiComState = WIFI_STATE_ERROR;
         }
      break;

      case WIFI_STATE_SEND_CWJAP_IS_SET:
         if (nonBlockingDelayRead(&wifiComDelay)) {
            wifiComStringWrite( "AT+CWJAP?\r\n" );
            wifiComExpectedResponse = Response_CWJAP_OK;
            nonBlockingDelayWrite(&wifiComDelay, DELAY_5_SECONDS);
            wifiComState = WIFI_STATE_WAIT_CWJAP_IS_SET;
         }
      break;

      case WIFI_STATE_WAIT_CWJAP_IS_SET:
         if (isExpectedResponse()) {
            wifiComExpectedResponse = Response_OK;
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
            wifiComExpectedResponse = Response_CWJAP_1;
            nonBlockingDelayWrite(&wifiComDelay, DELAY_10_SECONDS);
            wifiComState = WIFI_STATE_WAIT_CWJAP_SET_1;
         }
      break;

      case WIFI_STATE_WAIT_CWJAP_SET_1:
         if (isExpectedResponse()) {
            wifiComExpectedResponse = Response_CWJAP_2;
            wifiComState = WIFI_STATE_WAIT_CWJAP_SET_2;
         }
         if (nonBlockingDelayRead(&wifiComDelay)) {
         	pcSerialComStringWrite("Error in state: WIFI_STATE_WAIT_CWJAP_SET_1\r\n\r\n");
         	pcSerialComStringWrite("Check Wi-Fi AP credentials and restart\r\n\r\n");
         	wifiComState = WIFI_STATE_ERROR;
         }
         break;

      case WIFI_STATE_WAIT_CWJAP_SET_2:
         if (isExpectedResponse()) {
            wifiComState = WIFI_STATE_SEND_CIFSR;
         }
         if (nonBlockingDelayRead(&wifiComDelay)) {
        	pcSerialComStringWrite("Error in state: WIFI_STATE_WAIT_CWJAP_SET_2\r\n\r\n");
         	pcSerialComStringWrite("Check Wi-Fi AP credentials and restart\r\n\r\n");
         	wifiComState = WIFI_STATE_ERROR;
         }
      break;

      case WIFI_STATE_SEND_CIFSR:
         if (nonBlockingDelayRead(&wifiComDelay)) {
            wifiComStringWrite( "AT+CIFSR\r\n" );
            wifiComExpectedResponse = Response_CIFSR;
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
         	pcSerialComStringWrite("AT+CIFSR command not responded correctly\r\n\r\n");
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
            wifiComExpectedResponse = Response_OK;
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
         pcSerialComStringWrite("AT+CIPMUX=1 command not responded correctly\r\n\r\n");
         wifiComState = WIFI_STATE_ERROR;
         }
      break;

      case WIFI_STATE_SEND_CIPSERVER:
         if (nonBlockingDelayRead(&wifiComDelay)) {
            wifiComStringWrite( "AT+CIPSERVER=1,80\r\n" );
            wifiComExpectedResponse = Response_OK;
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
         pcSerialComStringWrite("AT+CIPSERVER=1,80 command not responded correctly\r\n\r\n");
         wifiComState = WIFI_STATE_ERROR;
         }
      break;

      case WIFI_STATE_SEND_CIPSTATUS:
         if (nonBlockingDelayRead(&wifiComDelay)) {
            wifiComStringWrite( "AT+CIPSTATUS\r\n" );
            wifiComExpectedResponse = Response_STATUS_3;
            nonBlockingDelayWrite(&wifiComDelay, DELAY_5_SECONDS);
            wifiComState = WIFI_STATE_WAIT_CIPSTATUS_STATUS_3;
         }
      break;

      case WIFI_STATE_WAIT_CIPSTATUS_STATUS_3:
         if (isExpectedResponse()) {
            nonBlockingDelayWrite(&wifiComDelay, DELAY_5_SECONDS);
            wifiComExpectedResponse = Response_CIPSTATUS;
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
            CurrentConnectionId = receivedCharWifiCom;
            wifiComExpectedResponse = Response_OK;
            wifiComState = WIFI_STATE_WAIT_CIPSTATUS_OK;
         }
      break;

      case WIFI_STATE_WAIT_CIPSTATUS_OK:
         if (isExpectedResponse()) {
            wifiComState = WIFI_STATE_SEND_CIPSEND;
         }
         if (nonBlockingDelayRead(&wifiComDelay)) {
            nonBlockingDelayWrite(&wifiComDelay, DELAY_5_SECONDS);
            wifiComState = WIFI_STATE_SEND_CIPSTATUS;
         }
      break;

      case WIFI_STATE_SEND_CIPSEND:
         lengthOfHttpLines = (strlen(httpWebPage));
         sprintf( strToSend, "AT+CIPSEND=%c,%d\r\n", CurrentConnectionId, lengthOfHttpLines );
         wifiComStringWrite( strToSend );
         wifiComState = WIFI_STATE_WAIT_CIPSEND;
         wifiComExpectedResponse = Response_OK;
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
        wifiComStringWrite( httpWebPage );
        wifiComState = WIFI_STATE_WAIT_HTTP;
        wifiComExpectedResponse = Response_SEND_OK;
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
            sprintf( strToSend, "AT+CIPCLOSE=%c\r\n", CurrentConnectionId );
            wifiComStringWrite( strToSend );
            wifiComExpectedResponse  = Response_CIPCLOSE;
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
    if( uartEsp8266.readable() ) {
        *receivedChar = uartEsp8266.getc();
        return true;
    }
    return false;
}

void wifiComStringWrite( char const* str )
{
    uartEsp8266.printf( "%s", str );
}

static bool isExpectedResponse()
{
   static int erStringPositionIndex = 0;
   char charReceived;
   bool moduleResponse = FALSE;

   if( wifiComCharRead(&charReceived) ){
      if (charReceived == wifiComExpectedResponse[erStringPositionIndex]) {
         erStringPositionIndex++;
         if (wifiComExpectedResponse[erStringPositionIndex] == '\0') {
            erStringPositionIndex = 0;
            moduleResponse = TRUE;
         }
      } else {
         erStringPositionIndex = 0;
      }
   }
   return moduleResponse;
}