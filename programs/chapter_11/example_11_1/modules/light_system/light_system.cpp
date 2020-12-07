//=====[Libraries]=============================================================

#include "arm_book_lib.h"

#include "light_system.h"
#include "bright_control.h"
#include "light_level_control.h"

//=====[Declaration of private defines]======================================

//=====[Declaration of private data types]=====================================

//=====[Declaration and initialization of public global objects]===============

//=====[Declaration and initialization of private global objects]===============

//=====[Declaration of external public global variables]=======================

//=====[Declaration and initialization of public global variables]=============

//=====[Declaration and initialization of private global variables]============

static float dutyCycle = 0.5;
static bool changeBrightnessLedRgbRed = true;
static bool changeBrightnessLedRgbGreen = true;
static bool changeBrightnessLedRgbBlue = true;

//=====[Implementations of public functions]===================================

void lightSystemInit()
{
    brighControlInit();
}

void lightSystemUpdate()
{
	dutyCycle = lightLevelControlRead();

    if ( changeBrightnessLedRgbRed ) setDutyCycle( LED_RGB_RED, dutyCycle );
    if ( changeBrightnessLedRgbGreen ) setDutyCycle( LED_RGB_GREEN, dutyCycle );
    if ( changeBrightnessLedRgbBlue ) setDutyCycle( LED_RGB_BLUE, dutyCycle );
}

void lightSystemSetBrightness( lightSystem_t light, bool state )
{
    switch( light ) {
        case LED_RGB_RED:
            changeBrightnessLedRgbRed = state;
        break;
        case LED_RGB_GREEN:
            changeBrightnessLedRgbGreen = state;
        break;
        case LED_RGB_BLUE:
            changeBrightnessLedRgbBlue = state;
        break;
        default:
        break;
    }
}

//=====[Implementations of private functions]==================================