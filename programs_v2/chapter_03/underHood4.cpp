//=====[Libraries]=============================================================

#include "mbed.h"
#include "arm_book_lib.h"

//=====[Defines]===============================================================

#define NUMBER_OF_BITS 12
#define MAX_RESOLUTION 4095.0

//=====[Declaration and intitalization of public global objects]===============

DigitalIn nextStepButton(BUTTON1);

AnalogIn potentiometer(A0);

DigitalOut startOfConversionLed(LED1);
DigitalOut stepOfConversionLed(LED2);
DigitalOut endOfConversionLed(LED3);

Serial uartUsb(USBTX, USBRX);

//=====[Declaration and intitalization of public global variables]=============

bool comparatorOutput = 0;
bool DACInput[NUMBER_OF_BITS];
bool digitalOutput[NUMBER_OF_BITS];

int conversionStep = 0;

float analogInput;
float digitalOutputScaledIntoRange0to1 = 0;
float DACOutput = 0;

//=====[Declarations (prototypes) of public functions]=========================

void inputsInit();
void outputsInit();

void startOfConversion();
bool analogComparator( );
bool iterativeConversionControllerStep( );
float digitalToAnalogConverter( );
void resetIterativeConversionController( );
void showConversionStatus( );
void endOfConversion( );

//=====[Main function, the program entry point after power on or reset]========

int main()
{
    inputsInit();
    outputsInit();
    while (true) {
        startOfConversion();
        resetIterativeConversionController();
        for ( conversionStep = 1;
              conversionStep <= NUMBER_OF_BITS;
              conversionStep++) {

            DACOutput = digitalToAnalogConverter( );

            comparatorOutput = analogComparator( );

            DACInput[NUMBER_OF_BITS - conversionStep] =
                iterativeConversionControllerStep( );

            showConversionStatus( );
        }

        endOfConversion( );
    }
}

//=====[Implementations of public functions]===================================

void inputsInit()
{
    //not neccesary
}

void outputsInit()
{
    startOfConversionLed = OFF;
    stepOfConversionLed = OFF;
    endOfConversionLed = OFF;
}

void startOfConversion()
{
    uartUsb.printf( "Please press Next Step Button (B1)\r\n\r\n" );
    while (!nextStepButton) {};
    uartUsb.printf( "Conversion started\r\n\r\n" );
    analogInput = potentiometer.read();
    startOfConversionLed = ON;
    delay(1000);
    startOfConversionLed = OFF;
}                                                                              

void resetIterativeConversionController( )
{
    int i;
    for ( i = 0; i < NUMBER_OF_BITS; i++) {
        DACInput[i]=0;
    }
    uartUsb.printf( "DACinput reseted\r\n\r\n" );
}                                                                              

bool iterativeConversionControllerStep( )
{
    if (comparatorOutput == 1) {
        return 1;
    } else {
        return 0;
    }
}                                                                              

float digitalToAnalogConverter( )
{
    float output = 0;
    int power = 1;
    int i;

    for (i=0; i<NUMBER_OF_BITS; i++) {
        if (i == ( NUMBER_OF_BITS - conversionStep ) ) {
            output += 1*power;
        } else {
            output += DACInput[i]*power;
        }
        power *= 2;
    }

    return output / MAX_RESOLUTION;
}                                                                              

bool analogComparator( )
{
    if ( analogInput >= DACOutput ) {
        return 1;
    } else {
        return 0;
    }
}                                                                              

void showConversionStatus( )
{
    int i;
    uartUsb.printf( "Conversion step: %i\r\n", conversionStep );
    uartUsb.printf( "Analog Input: %.3f\r\n", analogInput );
    uartUsb.printf( "DAC Output: %.3f\r\n", DACOutput );
    uartUsb.printf( "DAC Input: " );

    for (i=1; i<=NUMBER_OF_BITS; i++) {
        if (DACInput[NUMBER_OF_BITS-i] == 1) {
            uartUsb.printf( "1" );
        } else {
            uartUsb.printf( "0" );
        }
    }
    uartUsb.printf( "\r\n\r\n" );
    delay(1000);
    stepOfConversionLed = ON;
    while (!nextStepButton);
    stepOfConversionLed = OFF;
}                                                                              

void endOfConversion( )
{
    int i;
    uartUsb.printf( "End of conversion\r\n\r\n" );
    uartUsb.printf( "Analog Input: %.3f\r\n", analogInput );
    uartUsb.printf( "DAC Output: %.3f\r\n", DACOutput );

    endOfConversionLed = ON;
    delay(1000);
    endOfConversionLed = OFF;
}                                                                              
