//=====[Libraries]=============================================================

#include "mbed.h"
#include "arm_book_lib.h"

//=====[Defines]===============================================================

#define KEYPAD_AMOUNT_OF_ROWS   4
#define KEYPAD_AMOUNT_OF_COLS   4

#define KEYPAD_SCAN_TIME       40

//=====[Declaration of public data types]======================================

//=====[Declaration and intitalization of public global objects]===============

DigitalIn enterButton(BUTTON1);
DigitalIn gasDetector(D2);
DigitalIn aButton(D4);
DigitalIn bButton(D5);
DigitalIn cButton(D6);
DigitalIn dButton(D7);

DigitalOut alarmLed(LED1);
DigitalOut incorrectCodeLed(LED3);
DigitalOut systemBlockedLed(LED2);

Serial uartUsb(USBTX, USBRX);

//=====[Declaration and intitalization of public global variables]=============

//   Rows (outputs)    Columns (inputs)
// +---------------+  +----------------+
//   r1  r2  r3  r4     c1  c2  c3  c4
//  D23 D22 D21 D20    D19 D18 D17 D16
//  PB_3 PB_5 PC_7 PA_15    PB_12 PB_13 PB_15 PC_6

DigitalOut keypadRowPins[KEYPAD_AMOUNT_OF_ROWS] = {D23, D22, D21, D20};
DigitalIn keypadColPins[KEYPAD_AMOUNT_OF_COLS]  = {D19, D18, D17, D16};

char keypadIndexToCharArray[] = {
    '1', '2', '3', 'A',
    '4', '5', '6', 'B',
    '7', '8', '9', 'C',
    '*', '0', '#', 'D',
};

//=====[Declarations (prototypes) of public functions]=========================

void keypadInit();
char keypadScan();

//=====[Main function, the program entry point after power on or reset]========

int main()
{
    char keyPressed = '\0'; // NULL
    keypadInit(); 
 
    while (true) {
        keyPressed = keypadScan();         
        if( keyPressed ){
            uartUsb.printf("%c\r\n", keyPressed);   
        }
        delay(KEYPAD_SCAN_TIME);
    }
}

//=====[Implementations of public functions]===================================

void keypadInit()
{
    int pinIndex = 0;
    // Configure internal pull-up resistors for columns (inputs)
    for( pinIndex=0; pinIndex<KEYPAD_AMOUNT_OF_COLS; pinIndex++ ) {
        (keypadColPins[pinIndex]).mode(PullUp);
    }
}

// Return the character of pressed key or NULL
char keypadScan()
{
    int r = 0;
    int c = 0;
    int i = 0;

    // Scan all Rows
    for( r=0; r<KEYPAD_AMOUNT_OF_ROWS; r++ ) {

        // Put all rows in ON state
        for( i=0; i<KEYPAD_AMOUNT_OF_ROWS; i++ ) {
            keypadRowPins[i] = ON;
        }

        // Put in of state the current scanning row
        keypadRowPins[r] = OFF;

        // Check if any column reads OFF state of current row
        for( c=0; c<KEYPAD_AMOUNT_OF_COLS; c++ ) {
            // Check current column reads OFF state (this will be key pressed)
            if( keypadColPins[c] == OFF ) {
                return keypadIndexToCharArray[r*KEYPAD_AMOUNT_OF_ROWS + c];
            }
        }
   }

   return 0;
}