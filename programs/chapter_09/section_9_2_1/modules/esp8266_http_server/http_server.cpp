//=====[Libraries]=============================================================

#include <arm_book_lib.h>
#include <mbed.h>

#include "sapi_delay.h"

#include "wifi_credentials.h"

#include "pc_serial_com.h"

#include "esp8266_http_server.h"
#include "http_server.h"

//=====[Declaration of private defines]======================================

#define WIFI_MAX_DELAY    60000

#define BEGIN_USER_LINE   "<h3 style=\"text-align: center;\"><strong>"
#define END_USER_LINE     "</strong></h3>"

//=====[Declaration of private data types]=====================================

//=====[Declaration and initialization of public global objects]===============

//=====[Declaration of external public global variables]=======================

extern char ssid[100];
extern char pass[100];

//=====[Declaration and initialization of public global variables]=============

//=====[Declaration and initialization of private global variables]============

const char HttpWebPageHeader [] =
   "<!DOCTYPE HTML>"
   "<html>"
   "<head><title>Nucleo Board</title>"
   "<meta http-equiv=\"refresh\" content=\"15\">"
   "</head>"
   "<body bgcolor=\"#E2E1E3\">"
   "<p style=\"text-align: center;\">&nbsp;</p>"
   "<h1 style=\"text-align: center;\"><span style=\"color: #0000ff;\"><strong>Draft Web Server HTTP<br />ESP8266 - Nucleo Board</strong></span></h1>"
   "<p style=\"text-align: center;\">&nbsp;</p>"
   "<p style=\"text-align: center;\">&nbsp;</p>"
   ;

const char HttpWebPageEnd [] =
   "<p style=\"text-align: center;\">&nbsp;</p>"
   "<p style=\"text-align: center;\">&nbsp;</p>"
   "<hr />"
   "</body>"
   "</html>"
   ;

char HttpWebPageBody [200];

delay_t wifiDelay;

//=====[Declarations (prototypes) of private functions]========================

//=====[Implementations of public functions]===================================

void httpServerInit()
{
   uint8_t counter = 0;
   startMillis();
   
   char c;
   delayInit(&wifiDelay, WIFI_MAX_DELAY);
    esp8266UartInit();

   esp8266ConfigHttpServer(ssid, pass);
}

void httpServerUpdate()
{
    static int counter = 0;
    bool error;
    char strToSend[50];

    ExcecuteHttpServerFsm();

    if (esp8266ReadHttpServer()) {

        sprintf( HttpWebPageBody, "%s ALARM: ON - GAS DETECTED - TEMPERATURE: 27.5 °C %s", BEGIN_USER_LINE, END_USER_LINE );

        error = FALSE;

        delayInit(&wifiDelay, WIFI_MAX_DELAY);

        esp8266WriteHttpServer(HttpWebPageHeader, HttpWebPageBody, HttpWebPageEnd);
    }

    counter++;
}

//=====[Implementations of private functions]==================================