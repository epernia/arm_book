CAMBIOS POSIBLES:
-----------------

Ch1 - Los nuevos codes (todo GPIO)

Ch2 - Todo UART sencillo

Ch3 - Sigue Uart reemplazando a BLE funciones y modularizacion en funciones.
String.

Ch4 - Todo GPIOs y manejo de tiempos
- Blinky con la alarma (ver agregado de buzzer)
- Blinky no bloqueante con la alarma
- Debounce de un botón (el enter del code)
- Keypad

Ch5 - Todo analógico
- Potentiometer
- LM35 Con Filtro promedio de Batch de muestras
- LM35 Con Filtro promedio móvil
- Gas sensor con umbral (ver tiempo de preheat no bloqueante)

Ch6 - Modularización, refactor y RTC.

Ch? - BLE lo que sobra del CH3



//-----------------------------------------------------------------------------
// NUEVO PROGRAMA DE PRUEBA DE HARDWARE DE CHAPTER 1
//-----------------------------------------------------------------------------

#include "mbed.h"
#include "arm_book_lib.h"

int main()
{
    DigitalIn b1(BUTTON1);
    DigitalIn d2(D2);
    DigitalIn d3(D3);
    DigitalIn d4(D4);
    DigitalIn d5(D5);
    DigitalIn d6(D6);
    DigitalIn d7(D7);
    DigitalIn d8(D8);
    DigitalOut led1(LED1);
    DigitalOut led2(LED2);
    DigitalOut led3(LED3);

    b1.mode(PullDown);
    d2.mode(PullDown);
    d3.mode(PullDown);
    d4.mode(PullDown);
    d5.mode(PullDown);
    d6.mode(PullDown);
    d7.mode(PullDown);
    d8.mode(PullDown);
    led1 = OFF;
    led2 = OFF;
    led3 = OFF;

    while (true) {
        led1 = b1 || d2 || d3;
        led2 = b1 || d4 || d5 || d6 || d7;
        led3 = b1 || d8;
    }
}


//-----------------------------------------------------------------------------
// Elementos con nombre hasta ahora y direcciones de los pines
//-----------------------------------------------------------------------------

Inputs:
    BUTTON1     cancelButton
    D2          gasDetector
    D3          overTempDetector
    D4          aButton
    D5          bButton
    D6          cButton
    D7          dButton
    D7          dButton
    D8          okButton

Outputs:
    LED3(red)   alarmLed
    LED2(blue)  systemBlockedLed
    LED1(green) provideCodeLed

Variables:
    bool allowNewTry = false; // Permitir un nuevo intento de desactivacion de la alarma
    bool isCancelEnable = false; // Habilitar o deshabilitar el boton cancel
    int attemps = 0; // Cantidad de veces que se ingreso mal el codigo de desactivacion de la alarma

Panel:
    LED2(blue)  System blocked
    LED1(green) Provide code
                A   B   C   D
                 Cancel   Ok


//-----------------------------------------------------------------------------
// Example 1-1: Activate the alarm when gas is detected
//-----------------------------------------------------------------------------

// Example 1-1 propuesto :
//     Reemplazar "gasDetector" por "cancelButton", de esta forma cuando
//     toquen cancelButton se apague la alarma y al soltarlo se prenda.

// Ventajas:
//     Nos evita explicar el pull-down en el primer ejemplo.
//     Nos deja ese botón solo para ese propósito.
//     Nos evita tener que cambiar luego el funcionamiento de los pines

// Funcionamiento:
//     La alarma comienza encendida "sonando" (alarmLed = LED3 = Red LED = ON)
//     y al presionar el boton cancelButton se apaga "deja de sonar"

// Se debe explicar: 
//     bibliotecas (include), funcion main, 
//     while(true), if()
//     DigitalIn, DigitalOut, objetos y referencias (varible que apunta a un
//     objeto)
//     Establecer valor de un pin, ON y OFF (booleanos) y que es lo mismo que
//     true y false o 1 y 0 
//-----------------------------------------------------------------------------

#include "mbed.h"
#include "arm_book_lib.h"

int main()
{
    DigitalIn cancelButton(BUTTON1);
    DigitalOut alarmLed(LED3);

    alarmLed = ON;

    while (true) {
        if ( cancelButton == ON ) {
            alarmLed = OFF;
        }
    }
}

//-----------------------------------------------------------------------------
// Example 1-2: Keep the alarm active after gas or over temperature was detected
//-----------------------------------------------------------------------------

// Example 1-2 propuesto:
//     Como el ex 1-3, el ex 1-2 no aportaba mucho con respecto al nuevo 1-1.

// Funcionamiento:
//     La alarma comienza apagada "sin sonar" (alarmLed = LED3 = Red LED = OFF)
//     al presionar gasDetector OR overTempDetector (si detecta temperatura 
//     excesiva o precencia de gas) se prende la alarma "empeiza a sonar"
//     Al presionar el boton cancelButton se apaga la alarma "deja de sonar"

// Pregunta que le pueden surgir al usuario:
//     ¿Por que a Burton 1 no se le pone pull-down y a los otros si?
// Respuesta:
//     Porque lo trae en la placa externo al micro el pull-down

// Se debe explicar: 
//     OR lógica (||). Explicar PullDown
//-----------------------------------------------------------------------------

#include "mbed.h"
#include "arm_book_lib.h"

int main()
{
    DigitalIn cancelButton(BUTTON1);
    DigitalIn gasDetector(D2);
    DigitalIn overTempDetector(D3);
    DigitalOut alarmLed(LED3);

    gasDetector.mode(PullDown);
    overTempDetector.mode(PullDown);
    alarmLed = OFF;

    while (true) {
        if ( gasDetector || overTempDetector ) {
            alarmLed = ON;
        }
        if ( cancelButton ) {
            alarmLed = OFF;
        }
    }
}


//-----------------------------------------------------------------------------
// Example 1-3: Secure the alarm deactivation by means of a password
//-----------------------------------------------------------------------------

// Example 1-3 propuesto:
//     Misma consigna que el ex Example 1-4, pero distinta solucion.

// Funcionamiento:

//     La alarma comienza apagada "sin sonar" (alarmLed = LED3 = Red LED = OFF)
//     al presionar gasDetector OR overTempDetector (si detecta temperatura 
//     excesiva o precencia de gas) se prende la alarma "empeiza a sonar".
//     Ademas se enciende el led "provideCodeLed" (LED1 verde) que indica que 
//     hay que ingresar un código de desactivación para apagar la alarma.
       
//     El código de desactivación que consiste en presionar una combinacion 
//     secreta en simultáneo de los botones A, B C y D, junto con okButton
//     (la clave secreta es: aButton y bButton presionados, mientras que 
//     cButton y dButton deben estar liberados y okButton).
//     Cada vez que se realiza un intento erroneo se apaga el led 
//     "provideCodeLed"
       
//     Al presionar el boton cancelButton se permite un nuevo intento de poner
//     el codigo de desactivacion y se enciende el led "provideCodeLed" 
//     nuevamente.
       
//     Aparecen variables por primera vez (booleanas). Una permite habilitar o
//     deshabilitar el cancelButton (isCancelEnable), la otra indica que se 
//     puede hacer un nuevo intento para poner el codigo de desactivación
//     (allowNewTry)

//     [@Ari y @Pablo: Se que estas variables son innecesarias porque se podria
//     leer ambas salidas de los leds pero esta muy mal visto leer una salida 
//     para saber un estado]
       
//     Entre el boton okButton y cancelButton se crea una operacion 
//     autorretención o (set y reset) usando la variable allowNewTry como 
//     memoria de la salida).
       
//     Con esto no se permite probar combinaciones de mantener diferentes
//     botones y enter hasta acertar la clave, sin embargo tenemos infinitos
//     intentos y podriamos agotarlos todos hasta romper el codigo y lograr 
//     apagar la alarma de todas formas.

// Se debe explicar: 
//     AND lógica (&&), concepto de variable y tipo de datos (bool)
//-----------------------------------------------------------------------------


/*
24/07/2020 OJO QUE ACA HAY UN SALTO GRANDE DEL EJERCICIO ANTERIORE A ESTE, SE PODRIA AGRAGAR UN EXAMPLE INTERMEDIO QUE NO TENGA LO DE PROVIDE Y ALLOW.
*/

#include "mbed.h"
#include "arm_book_lib.h"

int main()
{
    DigitalIn cancelButton(BUTTON1);
    DigitalIn gasDetector(D2);
    DigitalIn overTempDetector(D3);
    DigitalIn aButton(D4);
    DigitalIn bButton(D5);
    DigitalIn cButton(D6);
    DigitalIn dButton(D7);
    DigitalIn okButton(D8);

    DigitalOut provideCodeLed(LED1); // LED1 = Green LED
    DigitalOut alarmLed(LED3);       // LED3 = Red LED

    gasDetector.mode(PullDown);
    overTempDetector.mode(PullDown);
    aButton.mode(PullDown);
    bButton.mode(PullDown);
    cButton.mode(PullDown);
    dButton.mode(PullDown);
    okButton.mode(PullDown);

    alarmLed = OFF;

    provideCodeLed = OFF;

    bool allowNewTry = false;
    bool isCancelEnable = false;

    while (true) {
        if ( gasDetector || overTempDetector ) {
            alarmLed = ON;
            provideCodeLed = ON;
            allowNewTry = true;
            isCancelEnable = true;
        }

        if ( allowNewTry && okButton ) {
            if ( aButton && bButton && !cButton && !dButton ) {
                alarmLed = OFF;
                isCancelEnable = false;
            }
            provideCodeLed = OFF;
            allowNewTry = false;
        }

        if ( isCancelEnable && cancelButton ) {
            provideCodeLed = ON;
            allowNewTry = true;
        }

}



//-----------------------------------------------------------------------------
// Example 1-4: Block the system when five incorrect passwords are entered
//-----------------------------------------------------------------------------

// Example 1-4 propuesto:
//      Misma consigna que el ex Example 1-5, pero distinta solucion.

// Funcionamiento:
//      Igual que el caso anterior pero ahora si se ingresa mal el código de 
//      desactivacion de la alarma 5 veces entonces se bloquea el sistema 
//      (LED azul)

// Se debe explicar: 
//     tipo de datos int, suma (+), return; (en este caso termina la funcion 
//     main).
//-----------------------------------------------------------------------------

#include "mbed.h"
#include "arm_book_lib.h"

int main()
{
    DigitalIn cancelButton(BUTTON1);
    DigitalIn gasDetector(D2);
    DigitalIn overTempDetector(D3);
    DigitalIn aButton(D4);
    DigitalIn bButton(D5);
    DigitalIn cButton(D6);
    DigitalIn dButton(D7);
    DigitalIn okButton(D8);

    DigitalOut provideCodeLed(LED1);    // LED1 = Green LED
    DigitalOut systemBlockedLed(LED2);  // LED2 = Blue LED
    DigitalOut alarmLed(LED3);          // LED3 = Red LED

    gasDetector.mode(PullDown);
    overTempDetector.mode(PullDown);
    aButton.mode(PullDown);
    bButton.mode(PullDown);
    cButton.mode(PullDown);
    dButton.mode(PullDown);
    okButton.mode(PullDown);

    alarmLed = OFF;

    systemBlockedLed = OFF;
    provideCodeLed = OFF;
  
    bool allowNewTry = false;
    bool isCancelEnable = false;
    int attemps = 0;

    while (true) {
        if ( gasDetector || overTempDetector ) {
            alarmLed = ON;
            isCancelEnable = true;
            provideCodeLed = ON;
            allowNewTry = true;
        }

        if ( allowNewTry && okButton ) {
            if ( aButton && bButton && !cButton && !dButton ) {
                alarmLed = OFF;
                isCancelEnable = false;
            }
            provideCodeLed = OFF;
            allowNewTry = false;
            isCancelEnable = true;
        }

        if ( isCancelEnable && cancelButton ) {
            provideCodeLed = ON;
            allowNewTry = true;
            isCancelEnable = false;
            attemps = attemps + 1;
        }

        if ( attemps > 5 ) { 
            systemBlockedLed = ON;
            provideCodeLed = OFF;
            return 0; // o while(true)
        }
    }
}


//-----------------------------------------------------------------------------


