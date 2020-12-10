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

Ticker tickerBrightControl;
tick_t tickRateMSBrightControl = 1;

//=====[Declaration and initialization of private global objects]===============

//=====[Declaration of external public global variables]=======================

//=====[Declaration and initialization of public global variables]=============

//=====[Declaration and initialization of private global variables]============

static int onTime[LEDS_QUANTITY];
static int offTime[LEDS_QUANTITY];

volatile tick_t tickCounter[LEDS_QUANTITY];

static float periodSFloat[LEDS_QUANTITY];

//=====[Declarations (prototypes) of private functions]========================

static void tickerCallbackBrightControl( );

//=====[Implementations of public functions]===================================

void brightControlInit()
{
    tickerBrightControl.attach( tickerCallbackBrightControl, 
                              ( (float) tickRateMSBrightControl) / 1000.0 );

    setPeriod( RGB_LED_RED, 0.01f );
    setPeriod( RGB_LED_GREEN, 0.01f );
    setPeriod( RGB_LED_BLUE, 0.01f );

    setDutyCycle( RGB_LED_RED, 0.5f );
    setDutyCycle( RGB_LED_GREEN, 0.5f );
    setDutyCycle( RGB_LED_BLUE, 0.5f );
}

void setPeriod( lightSystem_t light, float period )
{
    periodSFloat[light] = period;
}

void setDutyCycle( lightSystem_t light, float dutyCycle )
{
    onTime[light] = int ( ( periodSFloat[light] * dutyCycle ) * 1000 );
    offTime[light] = int ( periodSFloat[light] * 1000) - onTime[light];
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
