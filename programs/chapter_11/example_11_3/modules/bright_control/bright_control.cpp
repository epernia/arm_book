//=====[Libraries]=============================================================

#include "arm_book_lib.h"

#include "bright_control.h"

#include "light_level_control.h"

//=====[Declaration of private defines]======================================

//=====[Declaration of private data types]=====================================

//=====[Declaration and initialization of public global objects]===============

PwmOut ledRBG[] = {(PE_14), (PA_0), (PD_12)};

//=====[Declaration and initialization of private global objects]===============

//=====[Declaration of external public global variables]=======================

//=====[Declaration and initialization of public global variables]=============

//=====[Declaration and initialization of private global variables]============

//=====[Implementations of public functions]===================================

void brighControlInit()
{
    setPeriod( LED_RGB_RED, 0.01f );
    setPeriod( LED_RGB_GREEN, 0.01f );
    setPeriod( LED_RGB_BLUE, 0.01f );

    setDutyCycle( LED_RGB_RED, 0.5 );
    setDutyCycle( LED_RGB_GREEN, 0.5 );
    setDutyCycle( LED_RGB_BLUE, 0.5 );
}

void setPeriod( lightSystem_t light, float period )
{
    ledRBG[light].period(period);
}

void setDutyCycle( lightSystem_t light, float dutyCycle )
{
    ledRBG[light].write(dutyCycle);
}

//=====[Implementations of private functions]==================================