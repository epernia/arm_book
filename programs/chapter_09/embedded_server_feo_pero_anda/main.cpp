/*
ESP01 (ESP8266) connections:

   +--------------------------------------+
   |  |          +----+                   |
   |  +--+  |    |    |      RX o o VCC   |
   |     |  |    +----+   GPIO0 o o RST   |
   |  +--+  |    +----+   GPIO2 o o CH_PD |
   |  |     |    |    |     GND o o TX    |
   |  +-----+    +----+                   |
   +--------------------------------------+

   VCC ESP8266 <--> +3.3V EDU-CIAA-NXP
   RST ESP8266 <--> (SIN CONEXION)
 CH_PD ESP8266 <--> +3.3V EDU-CIAA-NXP
    TX ESP8266 <--> RX EDU-CIAA-NXP

    RX ESP8266 <--> TX EDU-CIAA-NXP
 GPIO0 ESP8266 <--> (SIN CONEXION)
 GPIO0 ESP8266 <--> (SIN CONEXION)
   GND ESP8266 <--> GND EDU-CIAA-NXP

  AT commands: http://www.pridopia.co.uk/pi-doc/ESP8266ATCommandsSet.pdf
*/

//=====[Libraries]=============================================================

#include <arm_book_lib.h>
#include <mbed.h>

#include "sapi_delay.h"

#include "esp8266_http_server.h"
#include "wifi_credentials.h"

#include "pc_serial_com.h"

/*==================[macros and definitions]=================================*/

// El maximo tiempo que se espera una respuesta del modulo ESP8266
#define WIFI_MAX_DELAY    60000

// El inicio y fin HTML le dan formato al texto enviado por el usuario
#define BEGIN_USER_LINE   "<h3 style=\"text-align: center;\"><strong>"
#define END_USER_LINE     "</strong></h3>"

/*==================[internal data definition]===============================*/

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

//=====[Main function, the program entry point after power on or reset]========

int main()
{
   bool error;
   uint8_t counter = 0;
   char strToSend[50];
   char c;

   delay_t wifiDelay;

   // Configura la UART USB a 115200 baudios
//   uartConfig (UART_USB, 115200);
   pcSerialComInit();
   esp8266UartInit();
   startMillis();


    int numberReceivedEnters = 0;
    bool esp01Error = false;
    delayInit( &wifiDelay, 1000 );
    pcSerialComStringWrite( "\r\nSending AT, to test if ESP01 module are working\r\n" );
    esp8266UartStringWrite( "AT\r\n" );

    while( numberReceivedEnters < 3 )
    {        
        if( esp8266UartCharRead(&c) ) {
            pcSerialComCharWrite( c );
            if( c == '\n' ) {
                numberReceivedEnters++;
            }
        }
        if( delayRead( &wifiDelay) ) {
            esp01Error = true;
            break;
        }
    }

    if( esp01Error ) {
        pcSerialComStringWrite( "ERROR - ESP01 module not detected!.\r\n\r\n" );
    } else {
        pcSerialComStringWrite( "ESP01 module working fine.\r\n\r\n" );
    }

/*
   delayInit( &wifiDelay, 1000 );
     while( true ) {

        if( delayRead(&wifiDelay) ) {
            //pcSerialComStringWrite( "Prueba AT\r\n" );
            esp8266UartStringWrite( "AT\r\n" );
        }

        if( esp8266UartCharRead(&c) ) {
            pcSerialComCharWrite( c );
        }
     }
*/

   // Envia un mensaje de bienvenida.
//   stdioPrintf(UART_USB, "\n\rBienvenido al servidor HTTP Esp8266 con EDU CIAA");
//   stdioPrintf(UART_USB, "\n\rLa configuracion puede tardar hasta 1 minuto.");
   pcSerialComStringWrite( "\n\rBienvenido al servidor HTTP Esp8266 con EDU CIAA" );
   pcSerialComStringWrite( "\n\rLa configuracion puede tardar hasta 1 minuto." );

   error = FALSE;
   // Configura un delay para salir de la configuracion en caso de error.
   delayInit(&wifiDelay, WIFI_MAX_DELAY);

   // Mientras no termine la configuracion o mientras no pase el tiempo maximo, ejecuta la configuracion.
   // A la configuracion se le pasa nombre y contrasenia de RED
   while (!esp8266ConfigHttpServer(WIFI_SSID, WIFI_PASSWORD) && !error) {
      if (delayRead(&wifiDelay)) {
         error = TRUE;
      }
   }

   // Avisa al usuario como salio la configuracion
   if (!error) {
//      stdioPrintf(UART_USB, "\n\rServidor HTTP configurado. IP: %s", esp8266GetIpAddress());
      sprintf( strToSend, "\n\rServidor HTTP configurado. IP: %s", esp8266GetIpAddress()) ;
      pcSerialComStringWrite( strToSend );

      // Enciende LEDG indicando que el modulo esta configurado.
//      gpioWrite(LEDG, TRUE);
   } else {
//      stdioPrintf(UART_USB, "\n\rError al configurar servidor HTTP.");
      pcSerialComStringWrite( "\n\rError al configurar servidor HTTP." );

      // Enciende LEDR indicando que el modulo esta en error.
//      gpioWrite(LEDR, TRUE);
   }

   while( true ) {

      // Si llego una peticion al servidor http
      if (esp8266ReadHttpServer()) {

         // Los datos a enviar a la web deben estar en formato HTML. Notar que
         // BEGIN_USER_LINE y END_USER_LINE solo formatean la cadena a enviar con tags HTML.
//         stdioSprintf(HttpWebPageBody, "%s VALOR CONTADOR: %d - VALOR ADC: %d %s", BEGIN_USER_LINE, counter, 3+counter*2, END_USER_LINE);
         sprintf( HttpWebPageBody, "%s VALOR CONTADOR: %d - VALOR ADC: %d %s", BEGIN_USER_LINE, counter, 3+counter*2, END_USER_LINE );

         error = FALSE;
         // Configura un delay para salir de la configuracion en caso de error.
         delayInit(&wifiDelay, WIFI_MAX_DELAY);

         // Mientras no termine el envio o mientras no pase el tiempo maximo, ejecuta el envio.
         while (!esp8266WriteHttpServer(HttpWebPageHeader, HttpWebPageBody, HttpWebPageEnd) && !error) {
            if (delayRead(&wifiDelay)) {
               error = TRUE;
            }
         }

         // Avisa al usuario como fue el envio
         if (!error) {
//            stdioPrintf(UART_USB, "\n\rPeticion respondida al cliente HTTP %d.", esp8266GetConnectionId());

            sprintf( strToSend, "\n\rPeticion respondida al cliente HTTP %d.", esp8266GetConnectionId() );
            pcSerialComStringWrite( strToSend );

//            gpioToggle(LEDG);
         } else {
//            stdioPrintf(UART_USB, "\n\rPeticion no respondida al cliente HTTP %d.", esp8266GetConnectionId());
            sprintf( strToSend, "\n\rPeticion no respondida al cliente HTTP %d.", esp8266GetConnectionId() );
            pcSerialComStringWrite( strToSend );

//            gpioToggle(LEDR);
         }

         counter++;
      }

   }

}