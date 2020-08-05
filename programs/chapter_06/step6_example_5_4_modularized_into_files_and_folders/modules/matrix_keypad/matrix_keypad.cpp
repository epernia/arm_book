//=====[Libraries]=============================================================

#include "mbed.h"
#include "arm_book_lib.h"

#include "matrix_keypad.h"

#include "date_and_time.h"

//=====[Declaration of private constants]======================================

#define MATRIX_KEYPAD_NUMBER_OF_ROWS    4
#define MATRIX_KEYPAD_NUMBER_OF_COLS    4
#define DEBOUNCE_BUTTON_TIME_MS        40

//=====[Declaration of private data types]=====================================

typedef enum {
    MATRIX_KEYPAD_SCANNING,
    MATRIX_KEYPAD_DEBOUNCE,
    MATRIX_KEYPAD_KEY_HOLD_PRESSED
} matrixKeypadState_t;

//=====[Declaration and intitalization of public global objects]===============

DigitalOut keypadRowPins[MATRIX_KEYPAD_NUMBER_OF_ROWS] = {D23, D22, D21, D20};
DigitalIn keypadColPins[MATRIX_KEYPAD_NUMBER_OF_COLS]  = {D19, D18, D17, D16};

//=====[Declaration of external public global variables]=======================

//=====[Declaration and intitalization of public global variables]=============

//=====[Declaration and intitalization of private global variables]============

static matrixKeypadState_t matrixKeypadState;
static int timeIncrement_ms = 0;

//=====[Declarations (prototypes) of private functions]========================

static char matrixKeypadScan();
static void matrixKeypadReset();

//=====[Implementations of public functions]===================================

void matrixKeypadInit( int updateTime_ms )
{
    timeIncrement_ms = updateTime_ms;
    matrixKeypadState = MATRIX_KEYPAD_SCANNING;
    int pinIndex = 0;
    for( pinIndex=0; pinIndex<MATRIX_KEYPAD_NUMBER_OF_COLS; pinIndex++ ) {
        (keypadColPins[pinIndex]).mode(PullUp);
    }
}

char matrixKeypadUpdate()
{
    static int accumulatedDebounceMatrixKeypadTime = 0;
    static char matrixKeypadLastKeyPressed = '\0';

    char keyDetected = '\0';
    char keyReleased = '\0';

    switch( matrixKeypadState ) {

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
            } else {
                matrixKeypadState = MATRIX_KEYPAD_SCANNING;
            }
        }
        accumulatedDebounceMatrixKeypadTime =
            accumulatedDebounceMatrixKeypadTime + timeIncrement_ms;
        break;

    case MATRIX_KEYPAD_KEY_HOLD_PRESSED:
        keyDetected = matrixKeypadScan();
        if( keyDetected != matrixKeypadLastKeyPressed ) {
            if( keyDetected == '\0' ) {
                keyReleased = matrixKeypadLastKeyPressed;
            }
            matrixKeypadState = MATRIX_KEYPAD_SCANNING;
        }
        break;

    default:
        matrixKeypadReset();
        break;
    }
    return keyReleased;
}

//=====[Implementations of private functions]==================================

static char matrixKeypadScan()
{
    int r = 0;
    int c = 0;
    int i = 0; 

    char matrixKeypadIndexToCharArray[] = {
        '1', '2', '3', 'A',
        '4', '5', '6', 'B',
        '7', '8', '9', 'C',
        '*', '0', '#', 'D',
    };

    for( r=0; r<MATRIX_KEYPAD_NUMBER_OF_ROWS; r++ ) {

        for( i=0; i<MATRIX_KEYPAD_NUMBER_OF_ROWS; i++ ) {
            keypadRowPins[i] = ON;
        }

        keypadRowPins[r] = OFF;

        for( c=0; c<MATRIX_KEYPAD_NUMBER_OF_COLS; c++ ) {
            if( keypadColPins[c] == OFF ) {
                return matrixKeypadIndexToCharArray[r*MATRIX_KEYPAD_NUMBER_OF_ROWS + c];
            }
        }
    }
    return '\0';
}

static void matrixKeypadReset()
{
    matrixKeypadState = MATRIX_KEYPAD_SCANNING;
}
