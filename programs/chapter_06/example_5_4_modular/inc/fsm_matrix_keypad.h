//=====[#include guards - begin]===============================================

#ifndef __MATRIX_H__
#define __MATRIX_H__

//=====[Libraries]=============================================================

#include "mbed.h"
#include "arm_book_lib.h"
#include "shared.h"

//=====[Declaration of public data types]======================================

typedef enum {
    MATRIX_KEYPAD_SCANNING,
    MATRIX_KEYPAD_DEBOUNCE,
    MATRIX_KEYPAD_KEY_HOLD_PRESSED
} matrixKeypadState_t;

//=====[Declaration of public constants]=======================================

#define KEYPAD_NUMBER_OF_ROWS                    4
#define KEYPAD_NUMBER_OF_COLS                    4

//=====[Declarations (prototypes) of public functions]=========================

void matrixKeypadInit();
char matrixKeypadUpdate();

//=====[#include guards - end]=================================================

#endif