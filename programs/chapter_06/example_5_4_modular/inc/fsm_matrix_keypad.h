#ifndef __MATRIX_H__
#define __MATRIX_H__

//=====[Libraries]=============================================================

#include "shared.h"

//=====[Defines]===============================================================

#define KEYPAD_NUMBER_OF_ROWS                    4
#define KEYPAD_NUMBER_OF_COLS                    4

//=====[Declaration of public data types]======================================

typedef enum {
    MATRIX_KEYPAD_SCANNING,
    MATRIX_KEYPAD_DEBOUNCE,
    MATRIX_KEYPAD_KEY_HOLD_PRESSED
} matrixKeypadState_t;

//=====[Declaration and intitalization of public global objects]===============

//=====[Declaration and intitalization of public global variables]=============

//=====[Declarations (prototypes) of public functions]=========================

void matrixKeypadInit();
char matrixKeypadScan();
char matrixKeypadUpdate();

#endif