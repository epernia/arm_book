#ifndef __BUTTON_H__
#define __BUTTON_H__

//=====[Libraries]=============================================================

#include "shared.h"

//=====[Defines]===============================================================

//=====[Declaration of public data types]======================================

typedef enum {
    BUTTON_UP,
    BUTTON_DOWN,
    BUTTON_FALLING,
    BUTTON_RISING
} buttonState_t;

//=====[Declaration and intitalization of public global objects]===============

//=====[Declaration and intitalization of public global variables]=============

//=====[Declarations (prototypes) of public functions]=========================

void debounceButtonInit();
bool debounceButtonUpdate();

#endif