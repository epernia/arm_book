//=====[Libraries]=============================================================

#include "arm_book_lib.h"

#include "bright_control.h"

#include "light_level_control.h"

//=====[Declaration of private defines]======================================

#define LEDS_QUANTITY 3

//=====[Declaration of private data types]=====================================

//=====[Declaration and initialization of public global objects]===============

DigitalOut RGBLed[] = {(PE_14), (PA_0), (PD_12)};

//=====[Declaration and initialization of private global objects]===============





//=====[Declaration of external public global variables]=======================

//=====[Declaration and initialization of public global variables]=============

//=====[Declaration and initialization of private global variables]============

static delay_t signalTime1[LEDS_QUANTITY];

static int onTime[LEDS_QUANTITY];
static int offTime[LEDS_QUANTITY];

static int previousOnTime[LEDS_QUANTITY];
static int previousOffTime[LEDS_QUANTITY];

//=====[Implementations of public functions]===================================

void brightControlInit()
{
    delayInit( &signalTime1, onTime );
}

void setPeriod( lightSystem_t light, float period )
{
    RGBLed[light].period(period);
}

void setDutyCycle( lightSystem_t light, float dutyCycle )
{
    int i;

    for (i = 0 ; i < LEDS_QUANTITY ; i++) {
        if( delayRead( &signalTime[i] ) ) {
            if (RGBLed[i].read() == ON) {
                if ( previousOnTime[i] != int (onTime[i] + 0.5) ) {
                    previousOnTime[i] = int (onTime[i] + 0.5);
                }
                delayWrite( &signalTime[i], offTime[i] );
                RGBLed[i] = OFF;
                
            } else {
                delayWrite( &signalTime[i], onTime[i] );
                RGBLed[i] = ON;
            }
        }
    }
}

//=====[Implementations of private functions]==================================