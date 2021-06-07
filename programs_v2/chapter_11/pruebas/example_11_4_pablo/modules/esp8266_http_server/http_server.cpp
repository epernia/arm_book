//=====[Libraries]=============================================================

#include <arm_book_lib.h>
#include <mbed.h>

#include "sapi.h"

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
   "<p style=\"text-align: center;\"><span style=\"color: #0000ff;\"><strong><img src=\"https://encrypted-tbn0.gstatic.com/images?q=tbn:ANd9GcT2moZZ0qZ5Az_Qt4kRv6NsJtIxKJl8phyn0APAyfsshhpvpjPs\" alt=\"Logo\" width=\"135\" height=\"135\" /></strong></span></p>"
   "<h1 style=\"text-align: center;\"><span style=\"color: #0000ff;\"><strong>Servidor Web HTTP<br />ESP8266 - Nucleo Board</strong></span></h1>"
   "<h4 style=\"text-align: center;\"><strong><em>En esta pagina web se muestran datos provenientes de la EDU-CIAA NXP</em></strong><br /><strong><em>enviados cuando un cliente HTTP como esta pagina realiza una peticion al servidor</em></strong></h4>"
   "<p style=\"text-align: center;\">INICIO USER HTML</p>"
   "<p style=\"text-align: center;\">&nbsp;</p>"
   "<p style=\"text-align: center;\">&nbsp;</p>"
   ;

const char HttpWebPageEnd [] =
   "<p style=\"text-align: center;\">&nbsp;</p>"
   "<p style=\"text-align: center;\">&nbsp;</p>"
   "<p style=\"text-align: center;\">FIN USER HTML</p>"
   "<hr />"
   "<p style=\"text-align: center;\"><em>Copyright&nbsp;Agustin Bassi -&nbsp;</em><em>Pablo Gomez</em></p>"
   "<p style=\"text-align: center;\">CURSOS INET 2017</p>"
   "<p style=\"text-align: center;\"><a href=\"http://www.proyecto-ciaa.com.ar\">www.proyecto-ciaa.com.ar</a></p>"
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
   
   char c;
   delayInit(&wifiDelay, WIFI_MAX_DELAY);
    esp8266UartInit();
   esp8266ConfigHttpServer(WIFI_SSID, WIFI_PASSWORD);
}

void httpServerUpdate()
{
    static int counter = 0;
    bool error;
    char strToSend[50];

    ExcecuteHttpServerFsm();

    if (esp8266ReadHttpServer()) {

        sprintf( HttpWebPageBody, "%s VALOR CONTADOR: %d - VALOR ADC: %d %s", BEGIN_USER_LINE, counter, 3+counter*2, END_USER_LINE );

        error = FALSE;

        delayInit(&wifiDelay, WIFI_MAX_DELAY);

        esp8266WriteHttpServer(HttpWebPageHeader, HttpWebPageBody, HttpWebPageEnd);
    }

    counter++;
}

//=====[Implementations of private functions]==================================