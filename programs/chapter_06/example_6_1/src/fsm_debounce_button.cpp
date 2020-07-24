//=====[Libraries]=============================================================

#include <fsm_debounce_button.h>

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