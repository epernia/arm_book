//=====[Libraries]=============================================================

#include "mbed.h"
#include "arm_book_lib.h"
#include <shared.h>

//=====[Defines]===============================================================

//=====[Declaration of public data types]======================================

typedef enum {
    BUTTON_UP,
    BUTTON_DOWN,
    BUTTON_FALLING,
    BUTTON_RISING
} buttonState_t;

//=====[Declaration and intitalization of public global objects]===============

DigitalIn enterButton(BUTTON1);

//=====[Declaration and intitalization of public global variables]=============

int accumulatedDebounceButtonTime     = 0;
int numberOfEnterButtonReleasedEvents = 0;
buttonState_t enterButtonState;

//=====[Declarations (prototypes) of public functions]=========================

void debounceButtonInit();
bool debounceButtonUpdate();