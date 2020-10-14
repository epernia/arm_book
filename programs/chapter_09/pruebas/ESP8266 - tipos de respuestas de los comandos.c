-------------------------------------------------------------------------------
RESPONSES
-------------------------------------------------------------------------------

Response: "OK\r\n"

AT - Test AT commands
AT+CWMODE=<mode> - Wi-Fi Mode (Station, Soft AP, Station+ Soft AP)

AT+CIPMUX=<mode> - Set connection type (single or multiple)


    return esp8266SendCommandWithOneResponse(
              "AT+CIPMUX=1\r\n", "OK\r\n", 
              WIFI_MODULE_BUSY,
              ESP8266_MOST_COMMON_AT_CMD_TIMEOUT );

    return esp8266CheckCommandWithOneResponse( WIFI_MODULE_BUSY );

AT+CIPSERVER=<mode>[,<port>] - Deletes/Creates TCP Server

    return esp8266SendCommandWithOneResponse(
              "AT+CIPSERVER=1,80\r\n", "OK\r\n", 
              WIFI_MODULE_????????????????_STARTED,
              ESP8266_MOST_COMMON_AT_CMD_TIMEOUT );
              
              
    return esp8266CheckCommandWithOneResponse( WIFI_MODULE_????????_COMPLETE );

    AT+CIPMODE=<mode> - Set the Transmission Mode 
    (este comando no esta ver si lo agrego en modulo esp8266_module.c/h, el
    ejemplo original que anda no lo usa)

Estos comandos se implementan asi:
    return esp8266SendCommandWithOkResponse( "AT+RST\r\n" );
    return esp8266CheckOkResponse();

-------------------------------------------------------------------------------

Response: Algo con parámetro(s) para guardar e identificar luego y termina en "\r\nOK\r\n"

AT+CWMODE?
    +CWMODE:<mode>
    
    OK

AT+CWLAP
    +CWLAP:<ecn>,<ssid>,<rssi>,<mac>,<ch>
    
    OK

AT+CIPMUX? - Query connection type (single or multiple)
    +CIPMUX:<mode>
    
    OK

AT+CIPMODE? - Query the Transmission Mode
    +CIPMODE:<mode>
    
    OK
    
AT+RST - Reset ESP8266
>>> OK
    WIFI DISCONNECT

     ets Jan  8 2013,rst cause:2, boot mode:(3,6)

    load 0x40100000, len 1396, room 16 
    tail 4
    chksum 0x89
    load 0x3ffe8000, len 776, room 4 
    tail 4
    chksum 0xe8
    load 0x3ffe8308, len 540, room 4 
    tail 8
    chksum 0xc0
    csum 0xc0

    2nd boot version : 1.4(b1)
      SPI Speed      : 40MHz
      SPI Mode       : DIO
      SPI Flash Size & Map: 8Mbit(512KB+512KB)
    jump to run user1 @ 1000

    .ò..n't use rtc mem data
    r0.æ$.’b.Ì÷
    Ai-Thinker Technology Co.,Ltd.

>>> ready
    WIFI CONNECTED
>>> WIFI GOT IP


-------------------------------------------------------------------------------

2 possible responses: 
    - Algo con parámetro(s) para guardar e identificar luego y termina en "\r\nOK\r\n"
    - "ERROR\r\n": 

AT+CWLAP=<ssid>[,<mac>,<ch>]
    +CWLAP:<ecn>,<ssid>,<rssi>,<mac>,<ch>
    OK
  or
    ERROR

AT+CWJAP=<ssid>,<pwd>[,<bssid>]
    OK
  or
    +CWJAP:<error code>
    ERROR

AT+CWJAP?
    Si esta conectado da:
        +CWJAP:<ssid>,<bssid>,<channel>,<rssi>
        OK
    Si no:
    No AP

    OK

AT+CWQAP - Disconnects from the AP
    Si está conectado da:
        AT+CWQAP
    
        WIFI DISCONNECT
        
        OK
        
    Si no está conectado da:
        AT+CWQAP
        
        
        OK
AT+CIPCLOSE - Closes TCP/UDP/SSL Connection
        OK
    Si no estaba conectado da 
        ERROR

-------------------------------------------------------------------------------

3 possible responses:

AT+CIPSTART... - Establishes TCP, UDP or SSL Connection
    OK
  or
    ERROR
  If TCP is already connected, the response is:
    ALREADY	CONNECT

-------------------------------------------------------------------------------
Other responses:
-------------------------------------------------------------------------------

AT+CIPSTATUS - Gets the Connection Status
    STATUS:<stat>
    +CIPSTATUS:<link ID>,<type>,<remote	IP>,<remote	port>,<local port>,<tetype>

Este responde antes que este creado el server:
    STATUS:<stat>
    
    OK

Si esta creado el server:

charDigitToIntDigit(c)
intDigitToCharDigit(n)

-------------------------------------------------------------------------------

AT+CIPSEND... - Sends Data (ver respuesta compleja, pagina 31):

  Function: to configure the data length in normal transmission mode.

    1. Single connection: (+CIPMUX=0)
        AT+CIPSEND=<length>
    2. Multiple connections: (+CIPMUX=1)
        AT+CIPSEND=<link ID>,<length>
    3. Remote IP and ports can be set in UDP transmission:
    AT+CIPSEND=[<link ID>,]<length>[,<remote IP>,<remote port>]
    
    Response:
      Send data of designated length.
      Wrap return > after the set command. Begin receiving serial data. When the requirement of data length is met, the transmission of data starts.
      If the connection cannot be established or gets disrupted during data transmission, the system returns:
        ERROR
      If data is transmitted successfully, the system returns:
        SEND OK
    
  Function: to start sending data in transparent transmission mode.

    AT+CIPSEND

    Response:
      Wrap return > after executing this command.
      Enter transparent transmission, with a 20-ms interval between each packet, and a maximum of 2048 bytes per packet.
      When a single packet containing +++ is received, ESP32 returns to normal command mode. Please wait for at least one second before sending the next AT command.
      This command can only be used in transparent transmission mode which requires single connection.
      For UDP transparent transmission, the value of <UDP mode> has to be 0 when using AT+CIPSTART.

-------------------------------------------------------------------------------

+IPD - Receives Network Data (ver pagina 38)

    OJOOO ESTO LO RECIBO ESPONTANEAMENTE DEDE EL ESP !!!

    The command is valid in normal command mode. When the module receives network data, it will send the data through the serial port using the +IPD command.
    
    Command:    
     - Single connection: 
       (+CIPMUX=0)+IPD,<len>[,<remote IP>,<remote port>]:<data>
     - Multiple connections:
       (+CIPMUX=1)+IPD,<link ID>,<len>[,<remote IP>,<remote	port>]:<data>
    
    Parameters:
     - [<remote	IP>]: remote IP, enabled by command AT+CIPDINFO=1.
     - [<remote	port>]: remote port, enabled by command AT+CIPDINFO=1.
     - <link ID>: ID number of connection.
     - <len>: data length.
     - <data>: data received.

-------------------------------------------------------------------------------

===============================================================================
===============================================================================






---------------------------------------------------
AT+CWJAP="NISUTA-Home","CeMaThBe09241727" - Timeout: 20 s
---------------------------------------------------
Cuando falla al conectar:
---------------------------------------------------

WIFI CONNECTED
WIFI DISCONNECTED
+CWJAP:1

FAIL

---------------------------------------------------
Cuando conecta bien:
---------------------------------------------------
AT+CWJAP="NISUTA-Home","CeMaThBe09241727"

WIFI CONNECTED
WIFI GOT IP

OK

---------------------------------------------------
Si ya estaba conectado:
---------------------------------------------------
AT+CWJAP="NISUTA-Home","CeMaThBe09241727"

WIFI DISCONNECTED
WIFI CONNECTED
WIFI GOT IP

OK

---------------------------------------------------
AT+CWJAP="NISUTA-Home","CeMaThBe09241727"

OK

---------------------------------------------------
Preguntarle si ya estaba conectado
AT+CWJAP?
---------------------------------------------------
+CWJAP:"NISUTA-Home","00:13:33:ab:fb:d6",11,-68

OK

---------------------------------------------------



---------------------------------------------------
AT+CWQAP - Disconnects from the AP - Timeout: 50 ms
---------------------------------------------------
Si está conectado da:
---------------------------------------------------
        AT+CWQAP
    
        WIFI DISCONNECT
        
        OK

---------------------------------------------------
    Si no está conectado da:
---------------------------------------------------
        AT+CWQAP
        
        
        OK

---------------------------------------------------


A veces se resetea solo em modulo.


---------------------------------------------------
AT+CWMODE? - Timeout: 50 ms
---------------------------------------------------
AT+CWMODE?

+CWMODE:3

OK

---------------------------------------------------



---------------------------------------------------
AT+CIFSR - Timeout: 50 ms
---------------------------------------------------
AT+CIFSR

En modo CWMODE=1:
    +CIFSR:STAIP,"192.168.1.103"
    +CIFSR:STAMAC,"5c:cf:7f:87:41:bb"

    OK
En modo CWMODE=3:
    +CIFSR:APIP,"192.168.4.1"
    +CIFSR:APMAC,"5e:cf:7f:87:41:bb"
    +CIFSR:STAIP,"192.168.1.103"
    +CIFSR:STAMAC,"5c:cf:7f:87:41:bb"

    OK

---------------------------------------------------
Si no esta conectado tira
---------------------------------------------------
+CIFSR:STAIP,"0.0.0.0"
+CIFSR:STAMAC,"5c:cf:7f:87:41:bb"

OK

---------------------------------------------------



---------------------------------------------------
AT+CIPSERVER=1,80 - Timeout: 50 ms
---------------------------------------------------
AT+CIPSERVER=1,80


OK

---------------------------------------------------
Si ya lo había mandado
---------------------------------------------------
AT+CIPSERVER=1,80

no change

OK

---------------------------------------------------



---------------------------------------------------
AT+CIPSEND=0,52 - Timeout: 50 ms
---------------------------------------------------
AT+CIPSEND=0,52

link is not valid

ERROR

------------------------
Cuando pasa este error es porque no me llego
ninguna peticion desde ningun bwowser
---------------------------------------------------
AT+CIPSEND=0,52

link is not valid

ERROR

---------------------------------------------------




---------------------------------------------------
AT+CIPCLOSE=0
---------------------------------------------------
Si nadie me pidio nada
---------------------------------------------------
AT+CIPCLOSE=0

UNLINK

ERROR

---------------------------------------------------
Si 
---------------------------------------------------
AT+CIPCLOSE=0

UNLINK

ERROR

---------------------------------------------------



Cuando poenen en el browser: 192.168.1.103
0,CONNECT
1,CONNECT

+IPD,0,459:GET / HTTP/1.1
Host: 192.168.1.103
Connection: keep-alive
Cache-Control: max-age=0
Upgrade-Insecure-Requests: 1
User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/85.0.4183.121 Safari/537.36
Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3;q=0.9
Accept-Encoding: gzip, deflate
Accept-Language: es-419,es;q=0.9

Debo mandar:
AT+CIPSEND=0,54

Me responde:
OK
> 

Debo mandar:
<!doctype html> <html> <body> Hello! </body> </html>

Contesta:
Recv 54 bytes

SEND OK

Debo mandar:
AT+CIPCLOSE=0  ---> Cuando mando esto el bowser reenderiza el HTML

Responde:
0,CLOSED

OK

+IPD,1,361:GET /favicon.ico HTTP/1.1
Host: 192.168.1.103
Connection: keep-alive
User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/85.0.4183.121 Safari/537.36
Accept: image/avif,image/webp,image/apng,image/*,*/*;q=0.8
Referer: http://192.168.1.103/
Accept-Encoding: gzip, deflate
Accept-Language: es-419,es;q=0.9



---------------------------------------------------



AT                                                    50 ms
AT+CWMODE=1                                           50 ms
AT+CWJAP="NISUTA-Home","CeMaThBe09241727"             20 s
AT+CWJAP="Wifi SweetHome","CeMaThBe09241727"          20 s
AT+CIFSR                                              50 ms
AT+CIPMUX=1                                           50 ms
AT+CIPSERVER=1,80                                     50 ms

Cuando poenen en el browser:
192.168.1.103
Me llega una peticion
0,CONNECT

+IPD...
si no respondo a tiempo y el usuario cierra el bowser me llega
0,CLOSED

AT+CIPSEND=0,54

<!doctype html> <html> <body> Hello! </body> </html>
<!doctype html> <html> <body> Eric ! </body> </html>
<!doctype html> <html> <body> pepe   </body> </html>

AT+CIPCLOSE=0

--------------------------------

Cualquier comando mal enviado puede responder "ERORR\r\n"

------------------------------

Si me piden:
http://192.168.1.103/a/pepe.html

Me llega:
2,CONNECT

+IPD,2,366:GET /a/pepe.html HTTP/1.1
Host: 192.168.1.103
User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:81.0) Gecko/20100101 Firefox/81.0
Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,*/*;q=0.8
Accept-Language: es-AR,es;q=0.8,en-US;q=0.5,en;q=0.3
Accept-Encoding: gzip, deflate
Connection: keep-alive
Upgrade-Insecure-Requests: 


------------------------------

Si me piden:
http://192.168.1.103

Me llega:
0,CLOSED
0,CONNECT

+IPD,0,355:GET / HTTP/1.1
Host: 192.168.1.103
User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:81.0) Gecko/20100101 Firefox/81.0
Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,*/*;q=0.8
Accept-Language: es-AR,es;q=0.8,en-US;q=0.5,en;q=0.3
Accept-Encoding: gzip, deflate
Connection: keep-alive
Upgrade-Insecure-Requests: 1


--------------------------------------------------
FSM
--------------------------------------------------

AT                                                    50 ms
AT+RST                                                10 s
AT+CWMODE=1                                           50 ms

AT+CIPSTATUS                                          VER

AT+CWJAP="NISUTA-Home","CeMaThBe09241727"             20 s

AT+CIFSR                                              50 ms

AT+CIPMUX=1                                           50 ms
AT+CIPSERVER=1,80                                     50 ms

AT+CIPSTATUS                                          VER

+IPD...
AT+CIPSEND=0,54
AT+CIPCLOSE=0

-----------------------

Detecto el modulo:

    AT
        Si no da intento resetear.
        
        AT+RST
    
Inicializo el módulo:

    Seteo el modo del Wifi a 1:
        AT+CWMODE=1
        
Chequeo que este conectado y tenga IP:
        
    AT+CIPSTATUS
        Si da no conectado tengo que conectar:
            AT+CWJAP="NISUTA-Home","CeMaThBe09241727"
        Si da conectado averiguo la IP y se la muestro al usaurio
            AT+CIFSR

Si esta conectado arranco el server:

    AT+CIPMUX=1
        OK
    AT+CIPSERVER=1,80
        OK

Me quedo esperando peticiones:

    0,CONNECT

    +IPD...
    si no respondo a tiempo y el usuario cierra el bowser me llega
    0,CLOSED

Respondo a la peticion:

    AT+CIPSEND=0,54

    <!doctype html> <html> <body> Hello! </body> </html>

    AT+CIPCLOSE=0

--------------------------------------------------


