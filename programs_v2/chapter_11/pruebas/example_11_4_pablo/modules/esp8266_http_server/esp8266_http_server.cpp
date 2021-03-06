/* Copyright 2017, Pablo Gomez - Agustin Bassi.
 * Copyright 2016, Marcelo Vieytes.
 * All rights reserved.
 *
 * This file is part sAPI library for microcontrollers.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 */

/* Date: 2017-14-11 */

/*==================[inlcusiones]============================================*/

#include "esp8266_http_server.h"

/*==================[definiciones y macros]==================================*/

//#define ESP8266_UART          UART_232 // FIXME: this!

#define ESP8266_BAUD_RATE     115200
#define ESP8266_TMO           5000
#define ESP8266_PAUSE         5000
#define ESP8266_WAIT          1000

#define MAX_COMMAND_LENGHT	   40
#define MAX_HTTP_WEB_LENGHT   1500

typedef enum Esp8266State {
   ESP_INIT,
   ESP_SEND_AT,
   ESP_WAIT_AT,
   ESP_SEND_CWJAP_CONS,
   ESP_WAIT_CWJAP_CONS_1,
   ESP_WAIT_CWJAP_CONS_2,
   ESP_SEND_CWMODE_SET,
   ESP_WAIT_CWMODE_SET,
   ESP_SEND_CWJAP_SET,
   ESP_WAIT_CWJAP_SET_1,
   ESP_WAIT_CWJAP_SET_2,
   ESP_WAIT_CWJAP_SET_3,
   ESP_CONNECTED,
   ESP_SEND_START,
   ESP_WAIT_START_1,
   ESP_WAIT_START_2,
   ESP_SEND_SEND,
   ESP_WAIT_SEND,
   ESP_SEND_DATA,
   ESP_WAIT_DATA_1,
   ESP_WAIT_DATA_2,
   ESP_CIPMUX_SEND,
   ESP_WAIT_CIPMUX,
   ESP_SEND_CIPSERVER,
   ESP_WAIT_CIPSERVER,
   ESP_WAIT_GET,
   ESP_SEND_CIPSTATUS,
   ESP_WAIT_CIPSTATUS_3,
   ESP_WAIT_GET_ID,
   ESP_SEND_CIPSEND,
   ESP_WAIT_CIPSEND,
   ESP_SEND_HTTP,
   ESP_WAIT_HTTP,
   ESP_WAIT_CIPSTATUS_OK,
   ESP_WAIT_CIPSTATUS,
   ESP_SEND_CIPCLOSE,
   ESP_WAIT_CIPCLOSE,
   ESP_SEND_CIFSR,
   ESP_WAIT_CIFSR,
   ESP_LOAD_IP,
   ESP_SEND_CWMODE,
   ESP_WAIT_CWMODE
} Esp8266Status_t;

/*==================[definiciones de datos internos]=========================*/

//Nombres de los estados en forma de strings para imprimir por pantalla.
static const char Esp8266StatusToString[][MAX_COMMAND_LENGHT] = {
   "ESP_INIT",
   "ESP_SEND_AT",
   "ESP_WAIT_AT",
   "ESP_SEND_CWJAP_CONS",
   "ESP_WAIT_CWJAP_CONS_1",
   "ESP_WAIT_CWJAP_CONS_2",
   "ESP_SEND_CWMODE_SET",
   "ESP_WAIT_CWMODE_SET",
   "ESP_SEND_CWJAP_SET",
   "ESP_WAIT_CWJAP_SET_1",
   "ESP_WAIT_CWJAP_SET_2",
   "ESP_WAIT_CWJAP_SET_3",
   "ESP_CONNECTED",
   "ESP_SEND_START",
   "ESP_WAIT_START_1",
   "ESP_WAIT_START_2",
   "ESP_SEND_SEND",
   "ESP_WAIT_SEND",
   "ESP_SEND_DATA",
   "ESP_WAIT_DATA_1",
   "ESP_WAIT_DATA_2",
   "ESP_CIPMUX_SEND",
   "ESP_WAIT_CIPMUX",
   "ESP_SEND_CIPSERVER",
   "ESP_WAIT_CIPSERVER",
   "ESP_WAIT_GET",
   "ESP_SEND_CIPSTATUS",
   "ESP_WAIT_CIPSTATUS_3",
   "ESP_WAIT_GET_ID",
   "ESP_SEND_CIPSEND",
   "ESP_WAIT_CIPSEND",
   "ESP_SEND_HTTP",
   "ESP_WAIT_HTTP",
   "ESP_WAIT_CIPSTATUS_OK",
   "ESP_WAIT_CIPSTATUS",
   "ESP_SEND_CIPCLOSE",
   "ESP_WAIT_CIPCLOSE",
   "ESP_SEND_CIFSR",
   "ESP_WAIT_CIFSR",
   "ESP_LOAD_IP",
   "ESP_SEND_CWMODE",
   "ESP_WAIT_CWMODE"
};

// Respuestas del ESP8266
static const char responseOk[] = "OK";
static const char Response_no_change[] = "no change";
static const char responseCwjapOk[] = "+CWJAP:";
static const char responseCwjap1[] = "WIFI CONNECTED";
static const char responseCwjap2[] = "WIFI GOT IP";
static const char responseSendOk[] = "SEND OK";
static const char responseStatus3[] = "STATUS:3";
static const char responseCipstatus[] = "+CIPSTATUS:";
static const char responseCipclose[] = "CLOSED";
static const char responseCifsr[] = "+CIFSR:STAIP,\"";

// Memoria asociada a las conexiones
static uint8_t currentConnectionId;
static char WifiName [30] = "";
static char WifiPass [30] = "";
static char WifiIp   [20];

// Punteros a la pagina web a mostrar
static char * PointerOfHttpBody;
static const char* PointerOfHttpHeader;
static const char* PointerOfHttpEnd;

// Variables utilizadas en la maquina de estados.
static const char* Esp8266ResponseToWait;
static uint8_t Esp8266Status = ESP_INIT;
static uint32_t Esp8266DebugBaudRate = 0;

static delay_t Esp8266Delay;

/*==================[definiciones de datos externos]=========================*/

//Serial esp8266Uart(D1, D0); // UART de BLE

// D42 = PE_8 = UART7_TX
// D41 = PE_7 = UART7_RX
Serial esp8266Uart( PE_8, PE_7 ); 

void esp8266UartInit()
{
    esp8266Uart.baud(ESP8266_BAUD_RATE);
}


bool esp8266UartCharRead( char* receivedChar )
{
    if( esp8266Uart.readable() ) {
        *receivedChar = esp8266Uart.getc();
        return true;
    }
    return false;
}

void esp8266UartCharWrite( char c )
{
    esp8266Uart.putc(c);
}

void esp8266UartStringWrite( char const* str )
{
    esp8266Uart.printf( "%s", str );
}

void esp8266UartSendAT( )
{
    esp8266UartStringWrite( "AT\r\n" );
}
/*==================[declaraciones de funciones internas]====================*/

static bool IsWaitedResponse();
static void SetEsp8622Status( Esp8266Status_t status );

/*==================[declaraciones de funciones externas]====================*/


// Funcion para obtener la direccion IP del modulo Esp8266
// @return devuelve un puntero al arreglo local donde esta alojado el dato.
char * esp8266GetIpAddress()
{
   return WifiIp;
}

// Funcion para obtener el nombre de la red del modulo Esp8266
// @return devuelve un puntero al arreglo local donde esta alojado el dato.
char * esp8266GetWifiName()
{
   return WifiName;
}

// Funcion para obtener la contrasenia de la red del modulo Esp8266
// @return devuelve un puntero al arreglo local donde esta alojado el dato.
char * esp8266GetWifiPass()
{
   return WifiPass;
}

// Devuelve al usuario el indice de la conexion establecida
// @return un entero correspondiente a
uint8_t esp8266GetConnectionId()
{
   return currentConnectionId - '0';
}

// Configura la conexion para que el modulo Esp8266 sea un servidor HTTP.
// Realiza llamadas no bloqueantes a la maquina de estados que maneja la conexion.
// La variable parametersReceived sirve para cargar por unica vez los datos de la red
// @param wifiName puntero con el nombre de la red Wifi
// @param wifiPass puntero con la contrasenia de la red Wifi
// @param debugUart es la uart por donde va a reportar los datos. Este parametro es opcional, poner 0 sino se necesita.
// @param debugBaudRate es la velocidad de la uart de debug. Poner en 0 si no se necesita.
// @return TRUE si se configuro correctamente como servidor HTTP, FALSE caso contrario.
bool esp8266InitHttpServer( char const* wifiName, char const* wifiPass )
{
   static bool parametersReceived = FALSE;

   if (!parametersReceived) {
      strcpy(WifiName, wifiName);
      strcpy(WifiPass, wifiPass);
      SetEsp8622Status(ESP_INIT);
      parametersReceived = TRUE;
   }
   ExcecuteHttpServerFsm();

   return (Esp8266Status == ESP_SEND_CIPSTATUS);
}

// Funcion para determinar si hay alguna peticion HTTP realizada desde una
// pagina web, realizada por algun cliente.
// Ademas realiza llamadas no bloqueantes a la maquina de estados del
// modulo que administra las conexiones y peticiones de clientes.
// @return TRUE si se recibio una peticion, FALSE caso contrario.
bool esp8266ReadHttpServer()
{
   //ExcecuteHttpServerFsm();
   return (Esp8266Status == ESP_SEND_CIPSEND);
}


// Funcion para enviar una pagina web actualizada en respuesta a la
// peticion del cliente.
// Corrobora quue haya una peticion real del usuario antes de enviar datos.
// Si hay una peticion, carga en los punteros correspondientes toda la
// pagina web que se le enviara al cliente.
// Un detalle importante es que tanto el header como el end de la web son
// constantes, ya que se supone que no cambian a lo largo del programa.
// Lo que si cambiara es el body HTTP que es el que contiene la informacion
// actualizada del estado del sistema.
// @param webHttpHeader puntero al header http (debe ser parte de la aplicacion de usuario).
// @param webHttpBody puntero al body http (debe ser parte de la aplicacion de usuario).
// @param webHttpEnd puntero al end http (debe ser parte de la aplicacion de usuario).
// @return TRUE si pudo mandar la web correctamente, FALSE caso contrario.
bool esp8266WriteHttpServer( char const* webHttpHeader, 
                             char* webHttpBody, 
                             char const* webHttpEnd )
{
   //antes de enviar se asegura que haya datos pendientes para enviar
   if (Esp8266Status == ESP_SEND_CIPSEND) {
      PointerOfHttpHeader = webHttpHeader;
      PointerOfHttpBody   = webHttpBody;
      PointerOfHttpEnd    = webHttpEnd;
   }
   //ExcecuteHttpServerFsm();

   return (Esp8266Status == ESP_SEND_CIPCLOSE);
}


/*==================[definiciones de funciones internas]=====================*/

// Funcion principal del modulo Wifi Esp8266 para funcionar como servidor HTTP.
// Desde aca se manejan los comandos a enviar, los tiempos a esperar y
// las respuestas a recibir.
// Automaticamente cambia de estados en funcion de los eventos que ocurran.
void ExcecuteHttpServerFsm(void)
{
   uint16_t lenghtOfHttpLines;
   static uint8_t auxIndex;
   static char byteReceived;

   char strToSend[50];

   switch (Esp8266Status) {

      case ESP_INIT:
         delayWrite(&Esp8266Delay, ESP8266_PAUSE);
         SetEsp8622Status(ESP_SEND_AT);
      break;

      case ESP_SEND_AT:
         if (delayRead(&Esp8266Delay)) {
            esp8266UartStringWrite( "AT\r\n" );
            Esp8266ResponseToWait = responseOk;
            delayWrite(&Esp8266Delay, ESP8266_TMO);
            SetEsp8622Status(ESP_WAIT_AT);
         }
      break;

      case ESP_WAIT_AT:
         if (IsWaitedResponse()) {
            delayWrite(&Esp8266Delay, ESP8266_PAUSE);
            SetEsp8622Status(ESP_SEND_CWMODE);
         }
         //Si no recibe OK vuelve a enviar AT
         if (delayRead(&Esp8266Delay)) {
            delayWrite(&Esp8266Delay, ESP8266_PAUSE);
            SetEsp8622Status(ESP_SEND_AT);
         }
      break;

      case ESP_SEND_CWMODE:
         if (delayRead(&Esp8266Delay)) {
//            stdioPrintf(ESP8266_UART, "AT+CWMODE=3\r\n");
            esp8266UartStringWrite( "AT+CWMODE=3\r\n" );
            Esp8266ResponseToWait = responseOk;
            delayWrite(&Esp8266Delay, ESP8266_TMO);
            SetEsp8622Status(ESP_WAIT_CWMODE);
         }
      break;

      case ESP_WAIT_CWMODE:
         if (IsWaitedResponse()) {
            delayWrite(&Esp8266Delay, ESP8266_PAUSE);
            SetEsp8622Status(ESP_SEND_CWJAP_CONS);
         }
         //Si no recibe OK vuelve a enviar AT
         if (delayRead(&Esp8266Delay)) {
            delayWrite(&Esp8266Delay, ESP8266_PAUSE);
            SetEsp8622Status(ESP_SEND_CWMODE);
         }
      break;

      case ESP_SEND_CWJAP_CONS:
         if (delayRead(&Esp8266Delay)) {
//            stdioPrintf(ESP8266_UART, "AT+CWJAP?\r\n");
            esp8266UartStringWrite( "AT+CWJAP?\r\n" );
            Esp8266ResponseToWait = responseCwjapOk;
            delayWrite(&Esp8266Delay, ESP8266_TMO);
            SetEsp8622Status(ESP_WAIT_CWJAP_CONS_1);
         }
      break;

      case ESP_WAIT_CWJAP_CONS_1:
         if (IsWaitedResponse()) {
            Esp8266ResponseToWait = responseOk;
            SetEsp8622Status(ESP_WAIT_CWJAP_CONS_2);
         }
         if (delayRead(&Esp8266Delay)) {
            delayWrite(&Esp8266Delay, ESP8266_PAUSE);
            SetEsp8622Status(ESP_SEND_CWMODE_SET);
         }
      break;

      case ESP_WAIT_CWJAP_CONS_2:
         if (IsWaitedResponse()) {
            delayWrite(&Esp8266Delay, ESP8266_PAUSE);
            SetEsp8622Status(ESP_CIPMUX_SEND);
         }
         if (delayRead(&Esp8266Delay)) {
            delayWrite(&Esp8266Delay, ESP8266_PAUSE);
            SetEsp8622Status(ESP_SEND_AT);
         }
      break;

      case ESP_SEND_CWMODE_SET:
         if (delayRead(&Esp8266Delay)) {
//            stdioPrintf(ESP8266_UART, "AT+CWMODE=3\r\n");
            esp8266UartStringWrite( "AT+CWMODE=3\r\n" );
            Esp8266ResponseToWait = responseOk;
            delayWrite(&Esp8266Delay, ESP8266_TMO);
            SetEsp8622Status(ESP_WAIT_CWMODE_SET);
         }
      break;

      case ESP_WAIT_CWMODE_SET:
         if (IsWaitedResponse()) {
            delayWrite(&Esp8266Delay, ESP8266_PAUSE);
            SetEsp8622Status(ESP_SEND_CWJAP_SET);
         }
         if (delayRead(&Esp8266Delay)) {
            delayWrite(&Esp8266Delay, ESP8266_PAUSE);
            SetEsp8622Status(ESP_SEND_AT);
         }
      break;

      case ESP_SEND_CWJAP_SET:
         if (delayRead(&Esp8266Delay)) {
//            stdioPrintf(ESP8266_UART, "AT+CWJAP=\"%s\",\"%s\"", WifiName, WifiPass);
            esp8266UartStringWrite( "AT+CWJAP=\"" );
            esp8266UartStringWrite( WifiName );
            esp8266UartStringWrite( "\",\"" );
            esp8266UartStringWrite( WifiPass );
            esp8266UartStringWrite( "\"" );
            esp8266UartStringWrite( "\r\n" );
            Esp8266ResponseToWait = responseCwjap1;
            delayWrite(&Esp8266Delay, ESP8266_TMO);
            SetEsp8622Status(ESP_WAIT_CWJAP_SET_1);
         }
      break;

      case ESP_WAIT_CWJAP_SET_1:
         if (IsWaitedResponse()) {
            Esp8266ResponseToWait = responseCwjap2;
            SetEsp8622Status(ESP_WAIT_CWJAP_SET_2);
         }
         if (delayRead(&Esp8266Delay)) {
            delayWrite(&Esp8266Delay, ESP8266_PAUSE);
            SetEsp8622Status(ESP_SEND_AT);
         }
         break;

      case ESP_WAIT_CWJAP_SET_2:
         if (IsWaitedResponse()) {
            Esp8266ResponseToWait = responseOk;
            SetEsp8622Status(ESP_WAIT_CWJAP_SET_3);
         }
         if (delayRead(&Esp8266Delay)) {
            delayWrite(&Esp8266Delay, ESP8266_PAUSE);
            SetEsp8622Status(ESP_SEND_AT);
         }
      break;

      case ESP_WAIT_CWJAP_SET_3:
         if (IsWaitedResponse())
            SetEsp8622Status(ESP_CIPMUX_SEND);
         if (delayRead(&Esp8266Delay)) {
            delayWrite(&Esp8266Delay, ESP8266_PAUSE);
            SetEsp8622Status(ESP_SEND_AT);
         }
      break;

      case ESP_CIPMUX_SEND:
         if (delayRead(&Esp8266Delay)) {
//            stdioPrintf(ESP8266_UART, "AT+CIPMUX=1\r\n");
            esp8266UartStringWrite( "AT+CIPMUX=1\r\n" );
            Esp8266ResponseToWait = responseOk;
            delayWrite(&Esp8266Delay, ESP8266_TMO);
            SetEsp8622Status(ESP_WAIT_CIPMUX);
            auxIndex=0;
         }
      break;

      case ESP_WAIT_CIPMUX:
         if (IsWaitedResponse()) {
            delayWrite(&Esp8266Delay, ESP8266_PAUSE);
            SetEsp8622Status(ESP_SEND_CIPSERVER);
         }
         if (delayRead(&Esp8266Delay)) {
            delayWrite(&Esp8266Delay, ESP8266_PAUSE);
            // cierra todas las posibles conexioes
//            stdioPrintf(ESP8266_UART, "AT+CIPCLOSE=%d\r\n", auxIndex);
            esp8266UartStringWrite( "AT+CIPCLOSE=" );
            esp8266UartCharWrite( auxIndex + 48 ); // auxIndex no puede ser mayor a 9
            esp8266UartStringWrite( "\r\n" );
            if (++auxIndex >= 4) {
               SetEsp8622Status(ESP_CIPMUX_SEND);
            }
         }
      break;

      case ESP_SEND_CIPSERVER:
         if (delayRead(&Esp8266Delay)) {
//            stdioPrintf(ESP8266_UART, "AT+CIPSERVER=1,80\r\n");
            esp8266UartStringWrite( "AT+CIPSERVER=1,80\r\n" );
            Esp8266ResponseToWait = responseOk;
            delayWrite(&Esp8266Delay, ESP8266_TMO);
            SetEsp8622Status(ESP_WAIT_CIPSERVER);
         }
      break;

      case ESP_WAIT_CIPSERVER:
         if (IsWaitedResponse()) {
            delayWrite(&Esp8266Delay, ESP8266_PAUSE);
            SetEsp8622Status(ESP_SEND_CIFSR);
         }
         if (delayRead(&Esp8266Delay)) {
            delayWrite(&Esp8266Delay, ESP8266_PAUSE);
            SetEsp8622Status(ESP_SEND_AT);
         }
      break;

      case ESP_SEND_CIFSR:
         if (delayRead(&Esp8266Delay)) {
//            stdioPrintf(ESP8266_UART, "AT+CIFSR\r\n");
            esp8266UartStringWrite( "AT+CIFSR\r\n" );
            Esp8266ResponseToWait = responseCifsr;
            delayWrite(&Esp8266Delay, ESP8266_PAUSE);
            SetEsp8622Status(ESP_WAIT_CIFSR);
         }
      break;

      case ESP_WAIT_CIFSR:
         if (IsWaitedResponse()) {
            SetEsp8622Status(ESP_LOAD_IP);
            auxIndex=0;
         }
         if (delayRead(&Esp8266Delay)) {
            delayWrite(&Esp8266Delay, ESP8266_PAUSE);
            SetEsp8622Status(ESP_SEND_CIFSR);
         }
      break;

      //Recibe byte a byte la direccion IP y la almacena en WifiIp
      case ESP_LOAD_IP:
//         if (uartReadByte(ESP8266_UART, &byteReceived)) {
         if( esp8266UartCharRead(&byteReceived) ){
            if (byteReceived != '"') {
               WifiIp [auxIndex] = byteReceived;
               auxIndex++;
            } else {
               WifiIp [auxIndex] = '\0';
               SetEsp8622Status(ESP_SEND_CIPSTATUS);
            }
         }
      break;

      // En este estado el modulo ya esta configurado como servidor HTTP
      // entonces cada vez que pasa el delay ESP8266_PAUSE mediante
      // el compando CIP_STATUS le pregunta al moduloWifi si hay una nueva peticion
      case ESP_SEND_CIPSTATUS:
         if (delayRead(&Esp8266Delay)) {
//            stdioPrintf(ESP8266_UART, "AT+CIPSTATUS\r\n");
            esp8266UartStringWrite( "AT+CIPSTATUS\r\n" );
            Esp8266ResponseToWait = responseStatus3;
            delayWrite(&Esp8266Delay, ESP8266_PAUSE);
            SetEsp8622Status(ESP_WAIT_CIPSTATUS_3);
         }
      break;

      case ESP_WAIT_CIPSTATUS_3:
         if (IsWaitedResponse()) {
            delayWrite(&Esp8266Delay, ESP8266_PAUSE);
            Esp8266ResponseToWait = responseCipstatus;
            SetEsp8622Status(ESP_WAIT_CIPSTATUS);
         }
         if (delayRead(&Esp8266Delay)) {
            delayWrite(&Esp8266Delay, ESP8266_PAUSE);
            SetEsp8622Status(ESP_SEND_CIPSTATUS);
         }
      break;

      case ESP_WAIT_CIPSTATUS:
         if (IsWaitedResponse()) {
            SetEsp8622Status(ESP_WAIT_GET_ID);
         }
         if (delayRead(&Esp8266Delay)) {
            delayWrite(&Esp8266Delay, ESP8266_PAUSE);
            SetEsp8622Status(ESP_SEND_CIPSTATUS);
         }
      break;

      case ESP_WAIT_GET_ID:
//         if (uartReadByte(ESP8266_UART, &byteReceived)) {
         if( esp8266UartCharRead(&byteReceived) ){
            currentConnectionId = byteReceived;
            Esp8266ResponseToWait = responseOk;
            SetEsp8622Status(ESP_WAIT_CIPSTATUS_OK);
            auxIndex = 0;
         }
      break;

      case ESP_WAIT_CIPSTATUS_OK:
         if (IsWaitedResponse()) {
            SetEsp8622Status(ESP_SEND_CIPSEND);
         }
         if (delayRead(&Esp8266Delay)) {
            delayWrite(&Esp8266Delay, ESP8266_PAUSE);
            SetEsp8622Status(ESP_SEND_CIPSTATUS);
         }
      break;

      //En este estado le dice al modulo cuantos bytes va a mandar
      // El comando esta compuesto por el comando CIPSEND, mas el ID
      // de la conexion abierta, mas la cantidad en ASCII de los bytes
      // que tiene la pagina web (o al menos los bytes de la primer linea).
      case ESP_SEND_CIPSEND:
         lenghtOfHttpLines = (strlen(PointerOfHttpHeader) + strlen(PointerOfHttpBody) + strlen(PointerOfHttpEnd));
         // Si se pasa del maximo largo permitido lo avisa en la web
         if (lenghtOfHttpLines >= MAX_HTTP_WEB_LENGHT) {
//            stdioSprintf(PointerOfHttpBody, "ERROR: La longitud de datos HTTP supera el maximo permitido de %d bytes.", MAX_HTTP_WEB_LENGHT);
            sprintf( PointerOfHttpBody, "ERROR: La longitud de datos HTTP supera el maximo permitido de %d bytes.", MAX_HTTP_WEB_LENGHT) ;
            lenghtOfHttpLines = (strlen(PointerOfHttpHeader) + strlen(PointerOfHttpBody) + strlen(PointerOfHttpEnd));
         }
//         stdioPrintf(ESP8266_UART, "AT+CIPSEND=%c,%d\r\n", currentConnectionId, lenghtOfHttpLines);
        sprintf( strToSend, "AT+CIPSEND=%c,%d\r\n", currentConnectionId, lenghtOfHttpLines );
        esp8266UartStringWrite( strToSend );

         SetEsp8622Status(ESP_WAIT_CIPSEND);
         Esp8266ResponseToWait = responseOk;
      break;

      case ESP_WAIT_CIPSEND:
         if (IsWaitedResponse()) {
            delayWrite(&Esp8266Delay, ESP8266_PAUSE);
            SetEsp8622Status(ESP_SEND_HTTP);
         }
         if (delayRead(&Esp8266Delay)) {
            delayWrite(&Esp8266Delay, ESP8266_PAUSE);
            SetEsp8622Status(ESP_SEND_CIPSTATUS);
         }
      break;

      case ESP_SEND_HTTP:
//         stdioPrintf(ESP8266_UART, "%s%s%s", PointerOfHttpHeader, PointerOfHttpBody, PointerOfHttpEnd);
        esp8266UartStringWrite( PointerOfHttpHeader );
        esp8266UartStringWrite( PointerOfHttpBody );
        esp8266UartStringWrite( PointerOfHttpEnd );

         SetEsp8622Status(ESP_WAIT_HTTP);
         Esp8266ResponseToWait = responseSendOk;
      break;

      case ESP_WAIT_HTTP:
         if (IsWaitedResponse()) {
            delayWrite(&Esp8266Delay, ESP8266_PAUSE);
            SetEsp8622Status(ESP_SEND_CIPCLOSE);
         }
         if (delayRead(&Esp8266Delay)) {
            delayWrite(&Esp8266Delay, ESP8266_PAUSE);
            SetEsp8622Status(ESP_SEND_CIPSEND);
         }
      break;

      case ESP_SEND_CIPCLOSE:
         if (delayRead(&Esp8266Delay)) {
//            stdioPrintf(ESP8266_UART, "AT+CIPCLOSE=%c\r\n", currentConnectionId);
            sprintf( strToSend, "AT+CIPCLOSE=%c\r\n", currentConnectionId );
            esp8266UartStringWrite( strToSend );
            Esp8266ResponseToWait  = responseCipclose;
            delayWrite(&Esp8266Delay, ESP8266_PAUSE);
            SetEsp8622Status(ESP_WAIT_CIPCLOSE);
         }
      break;

      case ESP_WAIT_CIPCLOSE:
         if (IsWaitedResponse()) {
            delayWrite(&Esp8266Delay, ESP8266_PAUSE);
            SetEsp8622Status(ESP_SEND_CIPSTATUS);
         }
         if (delayRead(&Esp8266Delay)) {
            delayWrite(&Esp8266Delay, ESP8266_PAUSE);
            SetEsp8622Status(ESP_SEND_CIPSTATUS);
         }
      break;
   }
}

// Determina si la respuesta que envio el modulo Esp8266 al
// sistema embebido sea correcta.
// Dentro de esta funcion va leyendo datos de la UART asociada
// al modulo Esp8266 hasta encontrar el patron de datos necesario.
// @return TRUE si es la respuesta esperada, FALSE caso contrario.
static bool IsWaitedResponse(void)
{
   static uint8_t index = 0;
   char charReceived;
   bool moduleResponse = FALSE;

//   if (uartReadByte(ESP8266_UART, &charReceived)) {
   if( esp8266UartCharRead(&charReceived) ){
      if (charReceived == Esp8266ResponseToWait[index]) {
         index++;
         if (Esp8266ResponseToWait[index] == '\0') {
            index = 0;
            moduleResponse = TRUE;
         }
      } else {
         index = 0;
      }
   }
   return moduleResponse;
}

/**
 * Setea el estado global del modulo Esp8266.
 * @param status Estado a setear.
 */
static void SetEsp8622Status( Esp8266Status_t status )
{
    Esp8266Status = status;
}

uint8_t getEsp8622Status(  )
{

   return Esp8266Status;
}

/*==================[fin del archivo]========================================*/
