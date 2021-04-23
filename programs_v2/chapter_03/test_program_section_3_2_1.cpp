//=====[Libraries]=============================================================

#include "mbed.h"
#include "arm_book_lib.h"

//=====[Declaration and initialization of public global objects]===============

Serial uartUsb(USBTX, USBRX);

AnalogIn potentiometer(A0);
AnalogIn lm35(A1); // 10 mV/°C
DigitalInOut sirenPin(PE_10);
DigitalIn mq2(PE_12);

//=====[Declaration and initialization of public global variables]=============

bool quit = false;

char receivedChar = '\0';

float lm35Reading = 0.0; // Raw ADC input A0 value
float lm35TempC = 0.0;   // Temperature in Celsius degrees [°C]
float lm35TempF = 0.0;   // Temperature in Fahrenheit degrees [°F]

float potentiometerReading = 0.0;   // Raw ADC input A1 value
float potentiometerScaledToC = 0.0; // Potentiometer value scaled to Celsius degrees [°C]
float potentiometerScaledToF = 0.0; // Potentiometer value scaled to Fahrenheit degrees [°F]

//=====[Declarations (prototypes) of public functions]=========================

void availableCommands();
void uartTask();

float analogReadingScaledWithTheLM35Formula( float analogReading );;
float celsiusToFahrenheit( float tempInCelsiusDegrees );
float analogValueToFahrenheit( float analogValue );
float potentiometerScaledToCelsius( float analogValue );
float potentiometerScaledToFahrenheit( float analogValue );

//=====[Main function, the program entry point after power on or reset]========

int main()
{
    sirenPin.mode(OpenDrain);
    sirenPin.input();
    availableCommands();
    while( true ) {
        uartTask();
    }
}

//=====[Implementations of public functions]===================================

void availableCommands()
{
    uartUsb.printf( "\r\nPress the following keys to continuously print the " );
    uartUsb.printf( "readings until 'q' is pressed:\r\n" );

    uartUsb.printf(" - 'a' the reading at the analog pin A0 (connected to ");
    uartUsb.printf("the potentiometer)\r\n");

    uartUsb.printf(" - 'b' the reading at the analog pin A1 (connected to ");
    uartUsb.printf("the LM35)\r\n");

    uartUsb.printf(" - 'c' the reading of the temperature measured by the ");
    uartUsb.printf("LM35 expressed in ºC\r\n");

    uartUsb.printf(" - 'd' the reading of the temperature measured by the ");
    uartUsb.printf("LM35 expressed in ºF\r\n");

    uartUsb.printf(" - 'e' the reading of the temperature measured by the ");
    uartUsb.printf("LM35 expressed in ºC and the potentiometer reading ");
    uartUsb.printf("scaled by the same factor\r\n");

    uartUsb.printf(" - 'f' the reading of the temperature measured by the ");
    uartUsb.printf("LM35 expressed in ºF and the potentiometer reading ");
    uartUsb.printf("scaled by the same factor\r\n");

    uartUsb.printf(" - 'g' the reading of the DOUT signal of the ");
    uartUsb.printf("MQ-2 gas sensor \r\n");

    uartUsb.printf("\r\nWARNING: The readings are printed continuously ");
    uartUsb.printf("until 'q' or 'Q' are pressed.\r\n\r\n");
}

void uartTask()
{
    if( uartUsb.readable() ) {
        char receivedChar = uartUsb.getc();
        switch (receivedChar) {

        // Print potentiometer ADC input (A1) raw value until pulse 'q' or 'Q'
        case 'a':
        case 'A':
            while( !(receivedChar == 'q' || receivedChar == 'Q') ) {
                potentiometerReading = potentiometer.read();
                uartUsb.printf("Potentiometer reading: %.2f\r\n",
                               potentiometerReading);
                delay( 200 );
                if( uartUsb.readable() ) {
                    receivedChar = uartUsb.getc();
                }
            }
            break;

        // Continuously print LM35 sensor ADC input (A0) raw value until pulse
        // 'q' or 'Q'
        case 'b':
        case 'B':
            while( !(receivedChar == 'q' || receivedChar == 'Q') ) {
                lm35Reading = lm35.read();
                uartUsb.printf("LM35 reading: %.2f\r\n", lm35Reading);
                delay( 200 );
                if( uartUsb.readable() ) {
                    receivedChar = uartUsb.getc();
                }
            }
            break;

        // Continuously print LM35 sensor ADC input (A0) value scaled to
        // Celsius degrees until pulse 'q' or 'Q'
        case 'c':
        case 'C':
            while( !(receivedChar == 'q' || receivedChar == 'Q') ) {             
                lm35Reading = lm35.read();
                lm35TempC = analogReadingScaledWithTheLM35Formula(lm35Reading);
                uartUsb.printf("LM35: %.2f °C\r\n", lm35TempC);
                delay( 200 );
                if( uartUsb.readable() ) {
                    receivedChar = uartUsb.getc();
                }
            }
            break;

        // Continuously print LM35 sensor ADC input (A0) value scaled to
        // Fahrenheit degrees until pulse 'q' or 'Q'
        case 'd':
        case 'D':
            while( !(receivedChar == 'q' || receivedChar == 'Q') ) {
                lm35Reading = lm35.read();
                lm35TempC = analogReadingScaledWithTheLM35Formula(lm35Reading);
                lm35TempF = celsiusToFahrenheit(lm35TempC);
                uartUsb.printf("LM35: %.2f °F\r\n", lm35TempF);
                delay( 200 );
                if( uartUsb.readable() ) {
                    receivedChar = uartUsb.getc();
                }
            }
            break;

        // Continuously print both LM35 sensor and potentiometer values scaled
        // to Celsius degrees until pulse 'q' or 'Q'
        case 'e':
        case 'E':
            while( !(receivedChar == 'q' || receivedChar == 'Q') ) {
                potentiometerReading = potentiometer.read();
                potentiometerScaledToC = potentiometerScaledToCelsius(potentiometerReading);
                lm35Reading = lm35.read();
                lm35TempC = analogReadingScaledWithTheLM35Formula(lm35Reading);
                uartUsb.printf("LM35: %.2f °C, Potentiometer scaled to °C: %.2f\r\n",
                               lm35TempC, potentiometerScaledToC);
                delay( 200 );
                if( uartUsb.readable() ) {
                    receivedChar = uartUsb.getc();
                }
            }
            break;

        // Continuously print both LM35 sensor and potentiometer values scaled
        // to Fahrenheit degrees until pulse 'q' or 'Q'
        case 'f':
        case 'F':
            while( !(receivedChar == 'q' || receivedChar == 'Q') ) {
                potentiometerReading = potentiometer.read();
                potentiometerScaledToF = potentiometerScaledToFahrenheit(potentiometerReading);
                lm35Reading = lm35.read();
                lm35TempC = analogReadingScaledWithTheLM35Formula(lm35Reading);
                lm35TempF = celsiusToFahrenheit(lm35TempC);
                uartUsb.printf("LM35: %.2f °F, Potentiometer scaled to °F: %.2f\r\n",
                               lm35TempF, potentiometerScaledToF);
                delay( 200 );
                if( uartUsb.readable() ) {
                    receivedChar = uartUsb.getc();
                }
            }
            break;

        case 'g':
        case 'G':
            while( !(receivedChar == 'q' || receivedChar == 'Q') ) {
                if ( !mq2 ) {
                    uartUsb.printf( "Gas is being detected\r\n");
                    sirenPin.output();                                     
                    sirenPin = LOW;	                    
                } else {
                    uartUsb.printf( "Gas is not being detected\r\n");
                    sirenPin.input();
                }
                delay( 200 );
                if( uartUsb.readable() ) {
                    receivedChar = uartUsb.getc();
                }
            }
            break;

        default:
            availableCommands();
            break;
        }
    }
}

float analogReadingScaledWithTheLM35Formula( float analogReading )
{
    return analogReading * 330.0;
}

float celsiusToFahrenheit( float tempInCelsiusDegrees )
{
    return 9.0/5.0 * tempInCelsiusDegrees + 32.0;
}

float potentiometerScaledToCelsius( float analogValue )
{
    return 148.0 * analogValue + 2.0;
}

float potentiometerScaledToFahrenheit( float analogValue )
{
    return celsiusToFahrenheit( potentiometerScaledToCelsius(analogValue) );
}