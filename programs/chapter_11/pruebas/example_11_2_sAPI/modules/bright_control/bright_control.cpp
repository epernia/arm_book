//=====[Libraries]=============================================================

#include "arm_book_lib.h"

#include "bright_control.h"

#include "light_level_control.h"
#include "pc_serial_com.h"

//=====[Declaration of private defines]======================================

#define LEDS_QUANTITY 3

//=====[Declaration of private data types]=====================================

//=====[Declaration and initialization of public global objects]===============

DigitalOut RGBLed[] = {(PE_14), (PA_0), (PD_12)};

//=====[Declaration and initialization of private global objects]===============

//=====[Declaration of external public global variables]=======================

//=====[Declaration and initialization of public global variables]=============

//=====[Declaration and initialization of private global variables]============

static delay_t signalTime[LEDS_QUANTITY];

static int onTime[LEDS_QUANTITY];
static int offTime[LEDS_QUANTITY];

static float periodSFloat[LEDS_QUANTITY];

//=====[Declarations (prototypes) of private functions]========================

//=====[Implementations of public functions]===================================

void brightControlInit()
{
    int i;

    for (i = 0 ; i < LEDS_QUANTITY ; i++) {
        onTime[i] = 5;
        offTime[i] = 5;
        periodMS[i] = onTime[i] + offTime[i];

        delayInit( &signalTime[i], onTime[i] );
    }
}

void setDutyCycle( lightSystem_t light, float dutyCycle )
{
    onTime[light] = int ( ( periodMS[light] * dutyCycle ) * 1000 );
    offTime[light] = int ( periodMS[light] * 1000) - onTime[light];

    int i;

    for (i = 0 ; i < LEDS_QUANTITY ; i++) {
        if ( RGBLed[i].read() == ON ) {
            if( tickCounter[i] > onTime[i] ) {
                tickCounter[i] = 1;
                if ( offTime[i] ) RGBLed[i] = OFF;
                
            }
        } else {
            if( tickCounter[i] > offTime[i] ) { 
                tickCounter[i] = 1;
                if ( onTime[i] ) RGBLed[i] = ON;
            }
        }
    }

    /*if (i > 100) {
        i=0;
        pcSerialComStringWrite("ON:");
        pcSerialComIntWrite( onTime[light] );
        pcSerialComStringWrite("\r\n");
        pcSerialComStringWrite("OFF:");
        pcSerialComIntWrite( offTime[light] );
        pcSerialComStringWrite("\r\n");
    }
    i++;*/
}

//=====[Implementations of private functions]==================================

static void tickerCallbackBrightControl( )
{
    int i;

    for (i = 0 ; i < LEDS_QUANTITY ; i++) {
        tickCounter[i]++;
        if ( RGBLed[i].read() == ON ) {
            if( tickCounter[i] > onTime[i] ) {
                tickCounter[i] = 1;
                if ( offTime[i] ) RGBLed[i] = OFF;
                
            }
        } else {
            if( tickCounter[i] > offTime[i] ) { 
                tickCounter[i] = 1;
                if ( onTime[i] ) RGBLed[i] = ON;
            }
        }
    }
}