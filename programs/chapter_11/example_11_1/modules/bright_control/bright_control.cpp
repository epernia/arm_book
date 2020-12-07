//=====[Libraries]=============================================================

#include "arm_book_lib.h"

#include "bright_control.h"

#include "light_level_control.h"
#include "pc_serial_com.h"

//=====[Declaration of private defines]======================================

#define LEDS_QUANTITY 3

//=====[Declaration of private data types]=====================================

//=====[Declaration and initialization of public global objects]===============

DigitalOut ledRBG[] = {(PE_14), (PA_0), (PD_12)};

Ticker tickerBrightControl;
tick_t tickRateMSBrightControl = 1;

//=====[Declaration and initialization of private global objects]===============

//=====[Declaration of external public global variables]=======================

//=====[Declaration and initialization of public global variables]=============

//=====[Declaration and initialization of private global variables]============

static delay_t signalTime1[LEDS_QUANTITY];

static int onTime[LEDS_QUANTITY];
static int offTime[LEDS_QUANTITY];

static int previousOnTime[LEDS_QUANTITY];
static int previousOffTime[LEDS_QUANTITY];

volatile tick_t tickCounter[LEDS_QUANTITY];

static float periodSFloat[LEDS_QUANTITY];

//=====[Declarations (prototypes) of private functions]========================

static void tickerCallbackBrightControl( );

//=====[Implementations of public functions]===================================

void brighControlInit()
{
    tickerBrightControl.attach( tickerCallbackBrightControl, 
                              ( (float) tickRateMSBrightControl) / 1000.0 );

    setPeriod( LED_RGB_RED, 0.01f );
    setPeriod( LED_RGB_GREEN, 0.01f );
    setPeriod( LED_RGB_BLUE, 0.01f );

    setDutyCycle( LED_RGB_RED, 0.5 );
    setDutyCycle( LED_RGB_GREEN, 0.5 );
    setDutyCycle( LED_RGB_BLUE, 0.5 );
}

void setPeriod( lightSystem_t light, float period )
{
    periodSFloat[light] = period;
}

void setDutyCycle( lightSystem_t light, float dutyCycle )
{
    //static int i = 0;

    onTime[light] = int ( ( periodSFloat[light] * dutyCycle ) * 1000 );
    offTime[light] = int ( periodSFloat[light] * 1000) - onTime[light];
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
        if ( ledRBG[i].read() == ON ) {
            if( tickCounter[i] > onTime[i] ) {
                tickCounter[i] = 1;
                if ( offTime[i] ) ledRBG[i] = OFF;
                
            }
        } else {
            if( tickCounter[i] > offTime[i] ) { 
                tickCounter[i] = 1;
                if ( onTime[i] ) ledRBG[i] = ON;
            }
        }
    }
}