//=====[Libraries]=============================================================

#include "arm_book_lib.h"

#include "wifi_com.h"

#include "non_blocking_delay.h"
#include "pc_serial_com.h"

//=====[Declaration of private defines]========================================

#define ESP8266_BAUD_RATE       115200
#define DELAY_5_SECONDS         5000

//=====[Declaration of private data types]=====================================

typedef enum {
   WIFI_STATE_INIT,
   WIFI_STATE_SEND_AT,
   WIFI_STATE_WAIT_AT,
   WIFI_STATE_IDLE,
   WIFI_STATE_ERROR
} wifiComState_t;

//=====[Declaration and initialization of public global objects]===============

Serial uartWifi( PE_8, PE_7 ); 

//=====[Declaration of external public global variables]=======================

//=====[Declaration and initialization of public global variables]=============

//=====[Declaration and initialization of private global variables]============

static const char responseOk[] = "OK";

static const char * wifiComExpectedResponse;
static wifiComState_t wifiComState;

static nonBlockingDelay_t wifiComDelay;

//=====[Declarations (prototypes) of private functions]========================

static bool isExpectedResponse();
bool wifiComCharRead( char* receivedChar );
void wifiComStringWrite( char const* str );

//=====[Implementations of public functions]===================================

void wifiComInit()
{
    uartWifi.baud(ESP8266_BAUD_RATE);
    wifiComState = WIFI_STATE_INIT;
}

void wifiComRestart()
{
	wifiComState = WIFI_STATE_INIT;
}

void wifiComUpdate()
{
   static char receivedCharWifiCom;

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
            pcSerialComStringWrite("AT command responded ");
            pcSerialComStringWrite("correctly\r\n");
            wifiComState = WIFI_STATE_IDLE;
         }
         if (nonBlockingDelayRead(&wifiComDelay)) {
            pcSerialComStringWrite("AT command not responded ");
            pcSerialComStringWrite("correctly\r\n");
            wifiComState = WIFI_STATE_ERROR;
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
    if( uartWifi.readable() ) {
        *receivedChar = uartWifi.getc();
        return true;
    }
    return false;
}

void wifiComStringWrite( char const* str )
{
    uartWifi.printf( "%s", str );
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
