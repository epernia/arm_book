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

PwmOut RGBLed[] = {(PB_4), (PA_0), (PD_12)};

Serial uartUsb(USBTX, USBRX);

DigitalIn nextStepButton(BUTTON1);


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
    
    uartUsb.printf( "Press B1 USER button to change the RGB values\r\n" );
    
    while (true) {
        for (float x = 0.00; x < 1.01; x += 0.1) {
            for (float y = 0.00; y < 1.01; y += 0.1) {
                for (float z = 0.00; z < 1.01; z += 0.1) {
                    setDutyCycle( RGB_LED_RED, z );
                    uartUsb.printf( "R: %.1f G:%.1f B:%.1f\r\n",z,y,x );
                    delay(40);
                    while (!nextStepButton);
                }
                setDutyCycle( RGB_LED_GREEN, y );
            }
            setDutyCycle( RGB_LED_BLUE, x );
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