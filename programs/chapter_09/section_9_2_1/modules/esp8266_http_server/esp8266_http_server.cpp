/*==================[inlcusiones]============================================*/

#include "esp8266_http_server.h"
#include "pc_serial_com.h"

/*==================[definiciones y macros]==================================*/

//#define ESP8266_UART          UART_232 // FIXME: this!

#define ESP8266_BAUD_RATE     115200
#define DELAY_5_SECONDS         5000
#define DELAY_1_SECOND          1000

#define MAX_COMMAND_LENGHT	   40
#define MAX_HTTP_WEB_LENGHT   1500

typedef enum Esp8266State {
   WIFI_STATE_INIT,
   WIFI_STATE_ERROR,
   WIFI_STATE_SEND_AT,
   WIFI_STATE_WAIT_AT,
   WIFI_STATE_SEND_CWJAP_IS_SET,
   WIFI_STATE_WAIT_CWJAP_IS_SET,
   WIFI_STATE_WAIT_CWJAP_CONS_2,
   WIFI_STATE_SEND_CWMODE_SET,
   WIFI_STATE_WAIT_CWMODE_SET,
   WIFI_STATE_SEND_CWJAP_SET,
   WIFI_STATE_WAIT_CWJAP_SET_1,
   WIFI_STATE_WAIT_CWJAP_SET_2,
   WIFI_STATE_WAIT_CWJAP_SET_3,
   WIFI_STATE_CONNECTED,
   WIFI_STATE_SEND_START,
   WIFI_STATE_WAIT_START_1,
   WIFI_STATE_WAIT_START_2,
   WIFI_STATE_SEND_SEND,
   WIFI_STATE_WAIT_SEND,
   WIFI_STATE_SEND_DATA,
   WIFI_STATE_WAIT_DATA_1,
   WIFI_STATE_WAIT_DATA_2,
   WIFI_STATE_SEND_CIPMUX,
   WIFI_STATE_WAIT_CIPMUX,
   WIFI_STATE_SEND_CIPSERVER,
   WIFI_STATE_WAIT_CIPSERVER,
   WIFI_STATE_WAIT_GET,
   WIFI_STATE_SEND_CIPSTATUS,
   WIFI_STATE_WAIT_CIPSTATUS_STATUS_3,
   WIFI_STATE_WAIT_GET_ID,
   WIFI_STATE_SEND_CIPSEND,
   WIFI_STATE_WAIT_CIPSEND,
   WIFI_STATE_SEND_HTTP,
   WIFI_STATE_WAIT_HTTP,
   WIFI_STATE_WAIT_CIPSTATUS_OK,
   WIFI_STATE_WAIT_CIPSTATUS,
   WIFI_STATE_SEND_CIPCLOSE,
   WIFI_STATE_WAIT_CIPCLOSE,
   WIFI_STATE_SEND_CIFSR,
   WIFI_STATE_WAIT_CIFSR,
   WIFI_STATE_LOAD_IP,
   WIFI_STATE_SEND_CWMODE,
   WIFI_STATE_WAIT_CWMODE
} Esp8266Status_t;

/*==================[definiciones de datos internos]=========================*/

// Respuestas del ESP8266
static const char Response_OK[] = "OK";
static const char Response_no_change[] = "no change";
static const char Response_CWJAP_OK[] = "+CWJAP:";
static const char Response_CWJAP_1[] = "WIFI CONNECTED";
static const char Response_CWJAP_2[] = "WIFI GOT IP";
static const char Response_SEND_OK[] = "SEND OK";
static const char Response_STATUS_3[] = "STATUS:3";
static const char Response_CIPSTATUS[] = "+CIPSTATUS:";
static const char Response_CIPCLOSE[] = "CLOSED";
static const char Response_CIFSR[] = "+CIFSR:STAIP,\"";

// Memoria asociada a las conexiones
static uint8_t CurrentConnectionId;
static char WifiName [30] = "";
static char WifiPass [30] = "";
static char WifiIp   [20];

// Punteros a la pagina web a mostrar
static char * PointerOfHttpBody;
static const char * PointerOfHttpHeader;
static const char * PointerOfHttpEnd;

// Variables utilizadas en la maquina de estados.
static const char * wifiComExpectedResponse;
static uint8_t Esp8266Status = WIFI_STATE_INIT;
static uint32_t Esp8266DebugBaudRate = 0;

static nonBlockingDelay_t wifiComDelay;

/*==================[definiciones de datos externos]=========================*/

Serial uartEsp8266( PE_8, PE_7 ); 

void wifiComInit()
{
    uartEsp8266.baud(ESP8266_BAUD_RATE);
}


bool wifiComCharRead( char* receivedChar )
{
    if( uartEsp8266.readable() ) {
        *receivedChar = uartEsp8266.getc();
        return true;
    }
    return false;
}

void wifiComCharWrite( char c )
{
    uartEsp8266.putc(c);
}

void wifiComStringWrite( char const* str )
{
    uartEsp8266.printf( "%s", str );
}

void wifiComSendAT( )
{
    wifiComStringWrite( "AT\r\n" );
}
/*==================[declaraciones de funciones internas]====================*/

static bool IsExpectedResponse();
static void wifiComFsmState( Esp8266Status_t status );
static void wifiComSetWiFiAPCredentialsSsid( char * APSssid );
static void pcSerialComGetWiFiAPCredentialsPassword( char * APPassword );

/*==================[declaraciones de funciones externas]====================*/


char * esp8266GetIpAddress()
{
   return WifiIp;
}

bool esp8266ReadHttpServer()
{
   return (Esp8266Status == WIFI_STATE_SEND_CIPSEND);
}

static void wifiComSetWiFiAPCredentialsSsid( char * APSssid )
{
	strcpy(wifiComAPSsid, APSssid);
}

static void pcSerialComGetWiFiAPCredentialsPassword( char * APPassword )
{
    strcpy(wifiComAPPassword, APPassword);
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
   if (Esp8266Status == WIFI_STATE_SEND_CIPSEND) {
      PointerOfHttpHeader = webHttpHeader;
      PointerOfHttpBody   = webHttpBody;
      PointerOfHttpEnd    = webHttpEnd;
   }
   //ExcecuteHttpServerFsm();

   return (Esp8266Status == WIFI_STATE_SEND_CIPCLOSE);
}


/*==================[definiciones de funciones internas]=====================*/

// Funcion principal del modulo Wifi Esp8266 para funcionar como servidor HTTP.
// Desde aca se manejan los comandos a enviar, los tiempos a esperar y
// las respuestas a recibir.
// Automaticamente cambia de estados en funcion de los eventos que ocurran.
void ExcecuteHttpServerFsm(void)
{
   uint16_t lenghtOfHttpLines;
   static uint8_t stringPositionIndex;
   static char receivedCharWifiCom;

   char strToSend[50];

   switch (Esp8266Status) {

      case WIFI_STATE_INIT:
         nonBlockingDelayWrite(&wifiComDelay, DELAY_5_SECONDS);
         wifiComFsmState(WIFI_STATE_SEND_AT);
      break;

      case WIFI_STATE_SEND_AT:
         if (nonBlockingDelayRead(&wifiComDelay)) {
            wifiComStringWrite( "AT\r\n" );
            wifiComExpectedResponse = Response_OK;
            nonBlockingDelayWrite(&wifiComDelay, DELAY_5_SECONDS);
            wifiComFsmState(WIFI_STATE_WAIT_AT);
         }
      break;

      case WIFI_STATE_WAIT_AT:
         if (IsExpectedResponse()) {
            nonBlockingDelayWrite(&wifiComDelay, DELAY_5_SECONDS);
            wifiComFsmState(WIFI_STATE_SEND_CWMODE);
         }
         if (nonBlockingDelayRead(&wifiComDelay)) {
         pcSerialComStringWrite("AT command not responded correctly\r\n\r\n");
         wifiComFsmState(WIFI_STATE_ERROR);
         }
      break;

      case WIFI_STATE_SEND_CWMODE:
         if (nonBlockingDelayRead(&wifiComDelay)) {
            wifiComStringWrite( "AT+CWMODE=1\r\n" );
            wifiComExpectedResponse = Response_OK;
            nonBlockingDelayWrite(&wifiComDelay, DELAY_5_SECONDS);
            wifiComFsmState(WIFI_STATE_WAIT_CWMODE);
         }
      break;

      case WIFI_STATE_WAIT_CWMODE:
         if (IsExpectedResponse()) {
            nonBlockingDelayWrite(&wifiComDelay, DELAY_5_SECONDS);
            wifiComFsmState(WIFI_STATE_SEND_CWJAP_IS_SET);
         }
         if (nonBlockingDelayRead(&wifiComDelay)) {
         pcSerialComStringWrite("AT+CWMODE=1 command not responded correctly\r\n\r\n");
         wifiComFsmState(WIFI_STATE_ERROR);
         }
      break;

      case WIFI_STATE_SEND_CWJAP_IS_SET:
         if (nonBlockingDelayRead(&wifiComDelay)) {
            wifiComStringWrite( "AT+CWJAP?\r\n" );
            wifiComExpectedResponse = Response_CWJAP_OK;
            nonBlockingDelayWrite(&wifiComDelay, DELAY_5_SECONDS);
            wifiComFsmState(WIFI_STATE_WAIT_CWJAP_IS_SET);
         }
      break;

      case WIFI_STATE_WAIT_CWJAP_IS_SET:
         if (IsExpectedResponse()) {
            wifiComExpectedResponse = Response_OK;
            wifiComFsmState(WIFI_STATE_SEND_CIFSR);
         }
         if (nonBlockingDelayRead(&wifiComDelay)) {
            nonBlockingDelayWrite(&wifiComDelay, DELAY_5_SECONDS);
            wifiComFsmState(WIFI_STATE_SEND_CWJAP_SET);
         }
      break;

      case WIFI_STATE_SEND_CWJAP_SET:
         if (nonBlockingDelayRead(&wifiComDelay)) {
            wifiComStringWrite( "AT+CWJAP=\"" );
            wifiComStringWrite( WifiName );
            wifiComStringWrite( "\",\"" );
            wifiComStringWrite( WifiPass );
            wifiComStringWrite( "\"" );
            wifiComStringWrite( "\r\n" );
            wifiComExpectedResponse = Response_CWJAP_1;
            nonBlockingDelayWrite(&wifiComDelay, DELAY_5_SECONDS);
            wifiComFsmState(WIFI_STATE_WAIT_CWJAP_SET_1);
         }
      break;

      case WIFI_STATE_WAIT_CWJAP_SET_1:
         if (IsExpectedResponse()) {
            wifiComExpectedResponse = Response_CWJAP_2;
            wifiComFsmState(WIFI_STATE_WAIT_CWJAP_SET_2);
         }
         if (nonBlockingDelayRead(&wifiComDelay)) {
         pcSerialComStringWrite("Wi-Fi module not connected\r\n\r\n");
         wifiComFsmState(WIFI_STATE_ERROR);
         }
         break;

      case WIFI_STATE_WAIT_CWJAP_SET_2:
         if (IsExpectedResponse()) {
            wifiComExpectedResponse = Response_OK;
            wifiComFsmState(WIFI_STATE_WAIT_CWJAP_SET_3);
         }
         if (nonBlockingDelayRead(&wifiComDelay)) {
         pcSerialComStringWrite("IP direction not assigned to Wi-Fi module\r\n\r\n");
         wifiComFsmState(WIFI_STATE_ERROR);
         }
      break;

      case WIFI_STATE_WAIT_CWJAP_SET_3:
         if (IsExpectedResponse()) {
            wifiComFsmState(WIFI_STATE_SEND_CIFSR);
         }
         if (nonBlockingDelayRead(&wifiComDelay)) {
         pcSerialComStringWrite("AT+CWJAP command not responded correctly\r\n\r\n");
         wifiComFsmState(WIFI_STATE_ERROR);
         }
      break;

      case WIFI_STATE_SEND_CIFSR:
         if (nonBlockingDelayRead(&wifiComDelay)) {
            wifiComStringWrite( "AT+CIFSR\r\n" );
            wifiComExpectedResponse = Response_CIFSR;
            nonBlockingDelayWrite(&wifiComDelay, DELAY_5_SECONDS);
            wifiComFsmState(WIFI_STATE_WAIT_CIFSR);
         }
      break;

      case WIFI_STATE_WAIT_CIFSR:
         if (IsExpectedResponse()) {
            wifiComFsmState(WIFI_STATE_LOAD_IP);
            stringPositionIndex=0;
         }
         if (nonBlockingDelayRead(&wifiComDelay)) {
         pcSerialComStringWrite("AT+CIFSR command not responded correctly\r\n\r\n");
         wifiComFsmState(WIFI_STATE_ERROR);
         }
      break;

      case WIFI_STATE_LOAD_IP:
         if( wifiComCharRead(&receivedCharWifiCom) ){
            if (receivedCharWifiCom != '"') {
               WifiIp [stringPositionIndex] = receivedCharWifiCom;
               stringPositionIndex++;
            } else {
               WifiIp [stringPositionIndex] = '\0';
               wifiComFsmState(WIFI_STATE_SEND_CIPMUX);
            }
         }
      break;

      case WIFI_STATE_SEND_CIPMUX:
         if (nonBlockingDelayRead(&wifiComDelay)) {
            wifiComStringWrite( "AT+CIPMUX=1\r\n" );
            wifiComExpectedResponse = Response_OK;
            nonBlockingDelayWrite(&wifiComDelay, DELAY_5_SECONDS);
            wifiComFsmState(WIFI_STATE_WAIT_CIPMUX);
            stringPositionIndex=0;
         }
      break;

      case WIFI_STATE_WAIT_CIPMUX:
         if (IsExpectedResponse()) {
            nonBlockingDelayWrite(&wifiComDelay, DELAY_5_SECONDS);
            wifiComFsmState(WIFI_STATE_SEND_CIPSERVER);
         }
         if (nonBlockingDelayRead(&wifiComDelay)) {
         pcSerialComStringWrite("AT+CIPMUX=1 command not responded correctly\r\n\r\n");
         wifiComFsmState(WIFI_STATE_ERROR);
         }
      break;

      case WIFI_STATE_SEND_CIPSERVER:
         if (nonBlockingDelayRead(&wifiComDelay)) {
            wifiComStringWrite( "AT+CIPSERVER=1,80\r\n" );
            wifiComExpectedResponse = Response_OK;
            nonBlockingDelayWrite(&wifiComDelay, DELAY_5_SECONDS);
            wifiComFsmState(WIFI_STATE_WAIT_CIPSERVER);
         }
      break;

      case WIFI_STATE_WAIT_CIPSERVER:
         if (IsExpectedResponse()) {
            nonBlockingDelayWrite(&wifiComDelay, DELAY_5_SECONDS);
            wifiComFsmState(WIFI_STATE_SEND_CIPSTATUS);
         }
         if (nonBlockingDelayRead(&wifiComDelay)) {
         pcSerialComStringWrite("AT+CIPSERVER=1,80 command not responded correctly\r\n\r\n");
         wifiComFsmState(WIFI_STATE_ERROR);
         }
      break;

      case WIFI_STATE_SEND_CIPSTATUS:
         if (nonBlockingDelayRead(&wifiComDelay)) {
            wifiComStringWrite( "AT+CIPSTATUS\r\n" );
            wifiComExpectedResponse = Response_STATUS_3;
            nonBlockingDelayWrite(&wifiComDelay, DELAY_5_SECONDS);
            wifiComFsmState(WIFI_STATE_WAIT_CIPSTATUS_STATUS_3);
         }
      break;

      case WIFI_STATE_WAIT_CIPSTATUS_STATUS_3:
         if (IsExpectedResponse()) {
            nonBlockingDelayWrite(&wifiComDelay, DELAY_5_SECONDS);
            wifiComExpectedResponse = Response_CIPSTATUS;
            wifiComFsmState(WIFI_STATE_WAIT_CIPSTATUS);
         }
         if (nonBlockingDelayRead(&wifiComDelay)) {
            nonBlockingDelayWrite(&wifiComDelay, DELAY_5_SECONDS);
            wifiComFsmState(WIFI_STATE_SEND_CIPSTATUS);
         }
      break;

      case WIFI_STATE_WAIT_CIPSTATUS:
         if (IsExpectedResponse()) {
            wifiComFsmState(WIFI_STATE_WAIT_GET_ID);
         }
         if (nonBlockingDelayRead(&wifiComDelay)) {
            nonBlockingDelayWrite(&wifiComDelay, DELAY_5_SECONDS);
            wifiComFsmState(WIFI_STATE_SEND_CIPSTATUS);
         }
      break;

      case WIFI_STATE_WAIT_GET_ID:
         if( wifiComCharRead(&receivedCharWifiCom) ){
            CurrentConnectionId = receivedCharWifiCom;
            wifiComExpectedResponse = Response_OK;
            wifiComFsmState(WIFI_STATE_WAIT_CIPSTATUS_OK);
            stringPositionIndex = 0;
         }
      break;

      case WIFI_STATE_WAIT_CIPSTATUS_OK:
         if (IsExpectedResponse()) {
            wifiComFsmState(WIFI_STATE_SEND_CIPSEND);
         }
         if (nonBlockingDelayRead(&wifiComDelay)) {
            nonBlockingDelayWrite(&wifiComDelay, DELAY_5_SECONDS);
            wifiComFsmState(WIFI_STATE_SEND_CIPSTATUS);
         }
      break;

      case WIFI_STATE_SEND_CIPSEND:
         lenghtOfHttpLines = (strlen(PointerOfHttpHeader) + strlen(PointerOfHttpBody) + strlen(PointerOfHttpEnd));
         if (lenghtOfHttpLines >= MAX_HTTP_WEB_LENGHT) {
            sprintf( PointerOfHttpBody, "ERROR: La longitud de datos HTTP supera el maximo permitido de %d bytes.", MAX_HTTP_WEB_LENGHT) ;
            lenghtOfHttpLines = (strlen(PointerOfHttpHeader) + strlen(PointerOfHttpBody) + strlen(PointerOfHttpEnd));
         }
        sprintf( strToSend, "AT+CIPSEND=%c,%d\r\n", CurrentConnectionId, lenghtOfHttpLines );
        wifiComStringWrite( strToSend );

         wifiComFsmState(WIFI_STATE_WAIT_CIPSEND);
         wifiComExpectedResponse = Response_OK;
      break;

      case WIFI_STATE_WAIT_CIPSEND:
         if (IsExpectedResponse()) {
            nonBlockingDelayWrite(&wifiComDelay, DELAY_5_SECONDS);
            wifiComFsmState(WIFI_STATE_SEND_HTTP);
         }
         if (nonBlockingDelayRead(&wifiComDelay)) {
            nonBlockingDelayWrite(&wifiComDelay, DELAY_5_SECONDS);
            wifiComFsmState(WIFI_STATE_SEND_CIPSTATUS);
         }
      break;

      case WIFI_STATE_SEND_HTTP:
//         stdioPrintf(ESP8266_UART, "%s%s%s", PointerOfHttpHeader, PointerOfHttpBody, PointerOfHttpEnd);
        wifiComStringWrite( PointerOfHttpHeader );
        wifiComStringWrite( PointerOfHttpBody );
        wifiComStringWrite( PointerOfHttpEnd );

         wifiComFsmState(WIFI_STATE_WAIT_HTTP);
         wifiComExpectedResponse = Response_SEND_OK;
      break;

      case WIFI_STATE_WAIT_HTTP:
         if (IsExpectedResponse()) {
            nonBlockingDelayWrite(&wifiComDelay, DELAY_5_SECONDS);
            wifiComFsmState(WIFI_STATE_SEND_CIPCLOSE);
         }
         if (nonBlockingDelayRead(&wifiComDelay)) {
            nonBlockingDelayWrite(&wifiComDelay, DELAY_5_SECONDS);
            wifiComFsmState(WIFI_STATE_SEND_CIPSEND);
         }
      break;

      case WIFI_STATE_SEND_CIPCLOSE:
         if (nonBlockingDelayRead(&wifiComDelay)) {
//            stdioPrintf(ESP8266_UART, "AT+CIPCLOSE=%c\r\n", CurrentConnectionId);
            sprintf( strToSend, "AT+CIPCLOSE=%c\r\n", CurrentConnectionId );
            wifiComStringWrite( strToSend );
            wifiComExpectedResponse  = Response_CIPCLOSE;
            nonBlockingDelayWrite(&wifiComDelay, DELAY_5_SECONDS);
            wifiComFsmState(WIFI_STATE_WAIT_CIPCLOSE);
         }
      break;

      case WIFI_STATE_WAIT_CIPCLOSE:
         if (IsExpectedResponse()) {
            nonBlockingDelayWrite(&wifiComDelay, DELAY_5_SECONDS);
            wifiComFsmState(WIFI_STATE_SEND_CIPSTATUS);
         }
         if (nonBlockingDelayRead(&wifiComDelay)) {
            nonBlockingDelayWrite(&wifiComDelay, DELAY_5_SECONDS);
            wifiComFsmState(WIFI_STATE_SEND_CIPSTATUS);
         }
      break;
   }
}

// Determina si la respuesta que envio el modulo Esp8266 al
// sistema embebido sea correcta.
// Dentro de esta funcion va leyendo datos de la UART asociada
// al modulo Esp8266 hasta encontrar el patron de datos necesario.
// @return TRUE si es la respuesta esperada, FALSE caso contrario.
static bool IsExpectedResponse()
{
   static uint8_t index = 0;
   char charReceived;
   bool moduleResponse = FALSE;

//   if (uartReadByte(ESP8266_UART, &charReceived)) {
   if( wifiComCharRead(&charReceived) ){
      if (charReceived == wifiComExpectedResponse[index]) {
         index++;
         if (wifiComExpectedResponse[index] == '\0') {
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
static void wifiComFsmState( Esp8266Status_t status )
{
    Esp8266Status = status;
}

uint8_t getEsp8622Status(  )
{

   return Esp8266Status;
}

/*==================[fin del archivo]========================================*/
