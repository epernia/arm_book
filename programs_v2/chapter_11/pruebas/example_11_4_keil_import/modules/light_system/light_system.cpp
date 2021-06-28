//=====[Libraries]=============================================================

#include "arm_book_lib.h"

#include "light_system.h"
#include "bright_control.h"
#include "light_level_control.h"
#include "ldr_sensor.h"

//=====[Declaration of private defines]========================================

//=====[Declaration of private data types]=====================================

//=====[Declaration and initialization of public global objects]===============

//=====[Declaration and initialization of private global objects]===============

//=====[Declaration of external public global variables]=======================

static float lightSystemLoopGain = 0.01;

//=====[Declaration and initialization of public global variables]=============

//=====[Declaration and initialization of private global variables]============

static float dutyCycle = 0.5f;
static bool brightnessRGBLedRedChangeEnabled = true;
static bool brightnessRGBLedGreenChangeEnabled = true;
static bool brightnessRGBLedBlueChangeEnabled = true;

//=====[Implementations of public functions]===================================

void lightSystemInit()
{
    brightControlInit();
}

void lightSystemUpdate()
{
    dutyCycle = dutyCycle + lightSystemLoopGain
                            * (lightLevelControlRead() - ldrSensorRead());

    if ( brightnessRGBLedRedChangeEnabled ) setDutyCycle( RGB_LED_RED, dutyCycle );
    if ( brightnessRGBLedGreenChangeEnabled ) setDutyCycle( RGB_LED_GREEN, dutyCycle );
    if ( brightnessRGBLedBlueChangeEnabled ) setDutyCycle( RGB_LED_BLUE, dutyCycle );
}

void lightSystemBrightnessChangeEnable( lightSystem_t light, bool state )
{
    switch( light ) {
        case RGB_LED_RED:
            brightnessRGBLedRedChangeEnabled = state;
        break;
        case RGB_LED_GREEN:
            brightnessRGBLedGreenChangeEnabled = state;
        break;
        case RGB_LED_BLUE:
            brightnessRGBLedBlueChangeEnabled = state;
        break;
        default:
        break;
    }
}

//=====[Implementations of private functions]==================================
