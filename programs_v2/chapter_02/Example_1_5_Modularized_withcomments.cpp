/* This program was written by Pablo Gomez, Ariel Lutenberg and Eric Pernia.
   Its purpose is to explain how comments can be included over the code. 
   Version 1.0 - 27th January 2021                                          */

//=====[Libraries]=============================================================

#include "mbed.h"
#include "arm_book_lib.h"

//=====[Declaration and initialization of public global objects]===============

/* Digital input objects named gasDetector, overTempDetector and aButton to
   dButton, are declared and assigned to D2 to D7, respectively.             */

DigitalIn enterButton(BUTTON1); 
DigitalIn gasDetector(D2);
DigitalIn overTempDetector(D3);
DigitalIn aButton(D4);
DigitalIn bButton(D5);
DigitalIn cButton(D6);
DigitalIn dButton(D7);

/* Digital output objects named alarmLed, incorrectCodeLed and systemBlockedLed
   are declared and assigned to LED1, LED2 and LED3, respectively.           */

DigitalOut alarmLed(LED1);
DigitalOut incorrectCodeLed(LED3);
DigitalOut systemBlockedLed(LED2);

//=====[Declaration and initialization of public global variables]=============

/* The boolean variable alarmState is declared and assigned OFF. 
   The int variable numberOfIncorrectCodes is declared and assigned 0 (zero).*/

bool alarmState = OFF;
int numberOfIncorrectCodes = 0;

//=====[Declarations (prototypes) of public functions]=========================

/* Prototypes of the functions that are used in this examples are declared   */

void inputsInit();
void outputsInit();

void alarmActivationUpdate();
void alarmDeactivationUpdate();

//=====[Main function, the program entry point after power on or reset]========

/* The initialization functions are called once after power on or reset.
   Then the functions to update the alarm are called inside an infinite loop */     

int main()
{
    inputsInit();
    outputsInit();
    while (true) {
        alarmActivationUpdate();
        alarmDeactivationUpdate();
    }
}         

//=====[Implementations of public functions]===================================

/* The function inputsInit configures gasDetector, overTempDetector and 
   aButton to dButton with internal pull-down resistors.                     */   

void inputsInit()
{
    gasDetector.mode(PullDown);
    overTempDetector.mode(PullDown);
    aButton.mode(PullDown);
    bButton.mode(PullDown);
    cButton.mode(PullDown);
    dButton.mode(PullDown);
}

/* The function outputsInit assigns OFF to alarmLED, incorrectCodeLed, 
   and systemBlockedLed.                                                     */     

void outputsInit()
{
    alarmLed = OFF;
    incorrectCodeLed = OFF;
    systemBlockedLed = OFF;
}

/* The function alarmActivationUpdate assigns ON to alarmLED if gasDetector or 
   overTempDetector are active, and also assigns alarmLed with alarmState.   */ 

void alarmActivationUpdate()
{
    if ( gasDetector || overTempDetector ) {
        alarmState = ON;
    }
    alarmLed = alarmState;
}

/* The function alarmDeactivationUpdate assesses the entered code and controls
   the activation of alarmLED, incorrectCodeLed, and systemBlockedLed       */ 

void alarmDeactivationUpdate()
{
    if ( numberOfIncorrectCodes < 5 ) {
        if ( aButton && bButton && cButton && dButton && !enterButton ) {
            incorrectCodeLed = OFF;
        }
        if ( enterButton && !incorrectCodeLed && alarmState ) {
            if ( aButton && bButton && !cButton && !dButton ) {
                alarmState = OFF	;
                numberOfIncorrectCodes = 0;
            } else {
                incorrectCodeLed = ON;
                numberOfIncorrectCodes = numberOfIncorrectCodes + 1;
            }
        }
    } else {
        systemBlockedLed = ON;
    }
}