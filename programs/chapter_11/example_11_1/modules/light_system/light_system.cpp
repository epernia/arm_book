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

//=====[Implementations of public functions]===================================

void lightSystemInit()
{
    brighControlInit();
}

void lightSystemUpdate()
{
    dutyCycle = lightLevelControlRead();

    setDutyCycle( LED_RGB_RED, dutyCycle );
    setDutyCycle( LED_RGB_GREEN, dutyCycle );
    setDutyCycle( LED_RGB_BLUE, dutyCycle );
}

//=====[Implementations of private functions]==================================
