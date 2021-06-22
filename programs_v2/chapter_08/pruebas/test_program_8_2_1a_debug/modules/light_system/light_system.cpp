//=====[Libraries]=============================================================

#include "arm_book_lib.h"

#include "light_system.h"
#include "bright_control.h"
#include "light_level_control.h"
#include "ldr_sensor.h"
#include "pc_serial_com.h"

//=====[Declaration of private defines]========================================

//=====[Declaration of private data types]=====================================

//=====[Declaration and initialization of public global objects]===============

DigitalOut ledGreen(LED1);
DigitalOut ledBlue(LED2);
DigitalOut ledRed(LED3);

//=====[Declaration and initialization of private global objects]===============

//=====[Declaration of external public global variables]=======================

static float lightSystemLoopGain = 0.01;

//=====[Declaration and initialization of public global variables]=============

//=====[Declaration and initialization of private global variables]============

static float dutyCycle = 0.0f;
static bool brightnessRGBLedRedChangeEnabled = true;
static bool brightnessRGBLedGreenChangeEnabled = false;
static bool brightnessRGBLedBlueChangeEnabled = false;
static int cuenta = 0;

//=====[Implementations of public functions]===================================

void lightSystemInit()
{
    brightControlInit();
    ledRed = ON;
    ledGreen = OFF;
    ledBlue = OFF;
}

void lightSystemUpdate()
{
    char number[10];
    cuenta++;
    if (cuenta > 13) {
        cuenta = 0;
        sprintf( number, "%0.4f", dutyCycle );
        pcSerialComStringWrite(number);
        pcSerialComStringWrite("\r\n");
    }
    
    if ( brightnessRGBLedRedChangeEnabled ) setDutyCycle( RGB_LED_RED, dutyCycle );
    if ( brightnessRGBLedGreenChangeEnabled ) setDutyCycle( RGB_LED_GREEN, dutyCycle );
    if ( brightnessRGBLedBlueChangeEnabled ) setDutyCycle( RGB_LED_BLUE, dutyCycle );

    dutyCycle = dutyCycle + 0.005f;
    
    if ( dutyCycle >= 0.99f ) {
        dutyCycle = 0.0f;
        if ( brightnessRGBLedRedChangeEnabled ) {
            brightnessRGBLedRedChangeEnabled = false;
            brightnessRGBLedGreenChangeEnabled = true;
            brightnessRGBLedBlueChangeEnabled = false;
            ledRed = OFF;
            ledGreen = ON;
            ledBlue = OFF;
            pcSerialComStringWrite("Verde\r\n");
        } else if ( brightnessRGBLedGreenChangeEnabled ) {
            brightnessRGBLedRedChangeEnabled = false;
            brightnessRGBLedGreenChangeEnabled = false;
            brightnessRGBLedBlueChangeEnabled = true;
            ledRed = OFF;
            ledGreen = OFF;
            ledBlue = ON;
            pcSerialComStringWrite("Azul\r\n");
        } else if ( brightnessRGBLedBlueChangeEnabled ) {
            brightnessRGBLedRedChangeEnabled = true;
            brightnessRGBLedGreenChangeEnabled = false;
            brightnessRGBLedBlueChangeEnabled = false;
            ledRed = ON;
            ledGreen = OFF;
            ledBlue = OFF;
            pcSerialComStringWrite("Rojo\r\n");
        }
    }
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
