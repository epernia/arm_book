
// ESTE MODULO NO SE USA EN EL 5_4 !!!!

//=====[Libraries]=============================================================

#include "shared.h"
#include "debounce_button.h"

//=====[Declaration of private data types]=====================================

typedef enum {
    BUTTON_UP,
    BUTTON_DOWN,
    BUTTON_FALLING,
    BUTTON_RISING
} buttonState_t;

//=====[Declaration of private constants]======================================



//=====[Declaration of external public global objects]=========================



//=====[Declaration and intitalization of public global objects]===============



//=====[Declaration and intitalization of private global objects]==============

DigitalIn enterButton(BUTTON1);

//=====[Declaration of external public global variables]=======================

static int accumulatedDebounceButtonTime     = 0;
static int numberOfEnterButtonReleasedEvents = 0;
static buttonState_t enterButtonState;

//=====[Declaration and intitalization of public global variables]=============



//=====[Declaration and intitalization of private global variables]============



//=====[Declarations (prototypes) of private functions]========================



//=====[Implementations of public functions]===================================

void debounceButtonInit()
{
    if( enterButton ) {
        enterButtonState = BUTTON_DOWN;
    } else {
        enterButtonState = BUTTON_UP;
    }
}

bool debounceButtonUpdate()
{
    bool enterButtonReleasedEvent = false;
    switch( enterButtonState ) {

    case BUTTON_UP:
        if( enterButton ) {
            enterButtonState = BUTTON_FALLING;
            accumulatedDebounceButtonTime = 0;
        }
        break;

    case BUTTON_FALLING:
        if( accumulatedDebounceButtonTime >= DEBOUNCE_BUTTON_TIME_MS ) {
            if( enterButton ) {
                enterButtonState = BUTTON_DOWN;
            } else {
                enterButtonState = BUTTON_UP;
            }
        }
        accumulatedDebounceButtonTime = accumulatedDebounceButtonTime +
                                        TIME_INCREMENT_MS;
        break;

    case BUTTON_DOWN:
        if( !enterButton ) {
            enterButtonState = BUTTON_RISING;
            accumulatedDebounceButtonTime = 0;
        }
        break;

    case BUTTON_RISING:
        if( accumulatedDebounceButtonTime >= DEBOUNCE_BUTTON_TIME_MS ) {
            if( !enterButton ) {
                enterButtonState = BUTTON_UP;
                enterButtonReleasedEvent = true;
            } else {
                enterButtonState = BUTTON_DOWN;
            }
        }
        accumulatedDebounceButtonTime = accumulatedDebounceButtonTime +
                                        TIME_INCREMENT_MS;
        break;

    default:
        debounceButtonInit();
        break;
    }
    return enterButtonReleasedEvent;
}

//=====[Implementations of private functions]==================================

