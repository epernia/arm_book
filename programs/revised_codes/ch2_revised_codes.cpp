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
// PROGRAMA DE PRUEBA DE HARDWARE DE CHAPTER 2 (NO CAMBIO)
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
// Partiendo de este Example 1-4 se pone abajo la versión modularizada en funciones
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
// Example 1-4 modularizado en funciones
//-----------------------------------------------------------------------------

/*=====[Libraries]===========================================================*/

#include "mbed.h"
#include "arm_book_lib.h"

/*=====[Declaration and intitalization of public global objects]=============*/

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

/*=====[Declaration and intitalization of public global variables]===========*/
  
bool allowNewTry = false;
bool isCancelEnable = false;
int attemps = 0;

/*=====[Declarations (prototypes) of public functions]=======================*/

void inputsInit();
void outputsInit();

void alarmActivationUpdate();
void alarmDeactivationUpdate();

/*=====[Main function, the program entry point after power on or reset]======*/

int main()
{
    inputsInit();
    outputsInit();
    while (true) {
        alarmActivationUpdate();
        alarmDeactivationUpdate();
    }
}         

/*=====[Implementations of public functions]=================================*/

void inputsInit()
{
    gasDetector.mode(PullDown);
    overTempDetector.mode(PullDown);
    aButton.mode(PullDown);
    bButton.mode(PullDown);
    cButton.mode(PullDown);
    dButton.mode(PullDown);
    okButton.mode(PullDown);
}

void outputsInit()
{
    alarmLed = OFF;
    systemBlockedLed = OFF;
    provideCodeLed = OFF;
}

void alarmActivationUpdate()
{
    if ( gasDetector || overTempDetector ) {
        alarmLed = ON;
        isCancelEnable = true;
        provideCodeLed = ON;
        allowNewTry = true;
    }
}

void alarmDeactivationUpdate()
{
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


//-----------------------------------------------------------------------------
// Example 2-1 Monitor the alarm state with a PC
//-----------------------------------------------------------------------------

// Example 2-1 propuesto:
//      Misma consigna que el ex Example 1-5, pero distinta solucion.

// Funcionamiento:
//      

// Se debe explicar: 
//     
//-----------------------------------------------------------------------------


