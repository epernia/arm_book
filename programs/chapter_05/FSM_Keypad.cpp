// ----------------------------------------------------------------------------
// FSM Matrix Keypad
// ----------------------------------------------------------------------------

//=====[Libraries]=============================================================

#include "mbed.h"
#include "arm_book_lib.h"

//=====[Defines]===============================================================

#define TIME_INCREMENT_MS                       10
#define DEBOUNCE_BUTTON_TIME_MS                 40
#define KEYPAD_NUMBER_OF_ROWS                    4
#define KEYPAD_NUMBER_OF_COLS                    4

//=====[Declaration of public data types]======================================

typedef enum{
    MATRIX_KEYPAD_SCANNING,
    MATRIX_KEYPAD_DEBOUNCE,
    MATRIX_KEYPAD_KEY_HOLD_PRESSED
} matrixKeypadState_t;

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

//   Rows (outputs)    Columns (inputs)
// +---------------+  +----------------+
//   r1  r2  r3  r4     c1  c2  c3  c4
//  D23 D22 D21 D20    D19 D18 D17 D16
//  PB_3 PB_5 PC_7 PA_15    PB_12 PB_13 PB_15 PC_6

DigitalOut keypadRowPins[KEYPAD_NUMBER_OF_ROWS] = {D23, D22, D21, D20};
DigitalIn keypadColPins[KEYPAD_NUMBER_OF_COLS]  = {D19, D18, D17, D16};

//=====[Declaration and intitalization of public global variables]=============

int accumulatedDebounceMatrixKeypadTime = 0;
char matrixKeypadLastKeyPressed  = '\0';
matrixKeypadState_t matrixKeypadState;

char matrixKeypadIndexToCharArray[] = {
    '1', '2', '3', 'A',
    '4', '5', '6', 'B',
    '7', '8', '9', 'C',
    '*', '0', '#', 'D',
};

//=====[Declarations (prototypes) of public functions]=========================

void matrixKeypadInit();
char matrixKeypadScan();
char matrixKeypadUpdate();

//=====[Main function, the program entry point after power on or reset]========

int main()
{
    char keyReleased = '\0'; // NULL
    matrixKeypadInit(); 
    uartUsb.printf("Matrix Keypad Initialized\r\n");   
 
    while (true) {
        keyReleased = matrixKeypadUpdate();         
        if( keyReleased ){
            uartUsb.printf("%c\r\n", keyReleased);   
        }
        delay(TIME_INCREMENT_MS);
    }
}

//=====[Implementations of public functions]===================================

void matrixKeypadInit()
{
    matrixKeypadState = MATRIX_KEYPAD_SCANNING;
    int pinIndex = 0;
    for( pinIndex=0; pinIndex<KEYPAD_NUMBER_OF_COLS; pinIndex++ ) {
        (keypadColPins[pinIndex]).mode(PullUp);
    }
}

char matrixKeypadScan()
{
    int r = 0;
    int c = 0;
    int i = 0;

    for( r=0; r<KEYPAD_NUMBER_OF_ROWS; r++ ) { // Scan all Rows
   
        for( i=0; i<KEYPAD_NUMBER_OF_ROWS; i++ ) { // Put all rows in ON state
            keypadRowPins[i] = ON;
        }

        keypadRowPins[r] = OFF; // Put in of state the current scanning row
       
        for( c=0; c<KEYPAD_NUMBER_OF_COLS; c++ ) { // Check if any column reads OFF state of current row                   
            if( keypadColPins[c] == OFF ) { // Check current column reads OFF state (this will be key pressed)
                return matrixKeypadIndexToCharArray[r*KEYPAD_NUMBER_OF_ROWS + c];
            }
        }
    }
    return '\0';
}

char matrixKeypadUpdate()
{
    char keyDetected = '\0';
    char keyReleased = '\0';

    switch( matrixKeypadState ){

        case MATRIX_KEYPAD_SCANNING:
            keyDetected = matrixKeypadScan();
            if( keyDetected != '\0' ) {
                matrixKeypadLastKeyPressed = keyDetected;
                accumulatedDebounceMatrixKeypadTime = 0;
                matrixKeypadState = MATRIX_KEYPAD_DEBOUNCE;
            }
        break;

        case MATRIX_KEYPAD_DEBOUNCE:
            if( accumulatedDebounceMatrixKeypadTime >= 
                DEBOUNCE_BUTTON_TIME_MS ) {
                keyDetected = matrixKeypadScan();
                if( keyDetected == matrixKeypadLastKeyPressed ) {
                    matrixKeypadState = MATRIX_KEYPAD_KEY_HOLD_PRESSED;
                } else{
                    matrixKeypadState = MATRIX_KEYPAD_SCANNING;
                }
            }
            accumulatedDebounceMatrixKeypadTime = 
                accumulatedDebounceMatrixKeypadTime + TIME_INCREMENT_MS;
        break;

        case MATRIX_KEYPAD_KEY_HOLD_PRESSED:
            keyDetected = matrixKeypadScan();
            if( keyDetected != matrixKeypadLastKeyPressed ) {             
                if( keyDetected == '\0' ) { // Key Released
                    keyReleased = matrixKeypadLastKeyPressed;
                }
                matrixKeypadState = MATRIX_KEYPAD_SCANNING;   
            }
        break;

        default:
            matrixKeypadInit();
        break;
    }
    return keyReleased;
}