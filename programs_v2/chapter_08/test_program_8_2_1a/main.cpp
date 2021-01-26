//=====[Libraries]=============================================================

#include "mbed.h"
#include "arm_book_lib.h"

//=====[Defines]=============================================================

//=====[Declaration of public data types]======================================

typedef enum {
    RGB_LED_RED,
    RGB_LED_GREEN,
    RGB_LED_BLUE,
} lightSystem_t;

//=====[Declaration and initialization of public global objects]===============

//DigitalOut ledGreen(LED1);
//DigitalOut ledBlue(LED2);
//DigitalOut ledRed(LED3);

PwmOut RGBLed[] = {(PB_4), (PA_0), (PD_12)};

//=====[Declaration and initialization of public global variables]=============

//=====[Declarations (prototypes) of public functions]=========================

void setPeriod( lightSystem_t light, float period );
void setDutyCycle( lightSystem_t light, float dutyCycle );

//=====[Main function, the program entry point after power on or reset]========

int main()
{
    setPeriod( RGB_LED_RED, 0.01f );
    setPeriod( RGB_LED_GREEN, 0.01f );
    setPeriod( RGB_LED_BLUE, 0.01f );

    setDutyCycle( RGB_LED_RED, 0.0f );
    setDutyCycle( RGB_LED_GREEN, 0.0f );
    setDutyCycle( RGB_LED_BLUE, 0.0f );
    
    while (true) {
        for (float x = 0.00; x < 0.99; x += 0.005) {
            for (float y = 0.00; y < 0.99; y += 0.005) {
                for (float z = 0.00; z < 0.99; z += 0.005) {
                    setDutyCycle( RGB_LED_RED, z );
                    delay(1);
                }
                setDutyCycle( RGB_LED_GREEN, y );
                delay(1);
            }
            setDutyCycle( RGB_LED_BLUE, x );
            delay(1);
        }
    }
}

//=====[Implementations of public functions]===================================

void setDutyCycle( lightSystem_t light, float dutyCycle )
{
    RGBLed[light].write(dutyCycle);
}

//=====[Implementations of private functions]==================================

void setPeriod( lightSystem_t light, float period )
{
    RGBLed[light].period(period);
}