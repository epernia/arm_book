/*
-------------------------------------------------------------------------------
Programa de prueba de conexionado 4.2.1.
-------------------------------------------------------------------------------

Programa con menu de opciones:
    'a' y 'A' Lee el potenciometro crudo 0 a 1.
    'b' y 'B' Lee el LM35 crudo.
    'c' y 'C' Lee el LM35 escalado en Celsius.
    'd' y 'D' Lee el LM35 escalado en Fahrenheit.
    'e' y 'E' Comparar LM35 y pote ecalado Celsius para que se vea el ruido.
    'f' y 'F' Comparar LM35 y pote ecalado Fahrenheit para que se vea el ruido.

Prueba el potenciometro y el LM35 (se puso cada 57 ms para que con lo que 
tarda en enviar por UART más lo que tarda en el wait() en total sean cerca de 
100ms).

El lector va a mover el potenciómetro de 0.0 a 50.0. Se le puede pedir que
ponga el potenciómetro al mismo valor que la temperatura que marca el LM35.

El sensor de temperatura LM35 si le apoya un dedo (sin tocar los terminales) o
le acerca un fósforo de costado cambia el valor.

Nótese que el sensor de temperatura fluctua mucho a diferencia del
potenciómetro que mantien valores mas estables (tiene menos ruido el pote).

Al variar la temperatura (utilice por ejemplo un secador de pelo en calor),
nótese que la temperatura al variar da ciertos saltos de ruido hasta que se
establece.

Poner un gráfico mostrando los 2 sensores para que los vea graficados 
(si llegamos).

-------------------------------------------------------------------------------

ADC pin:

  0 V <---> adcPin.read() = 0.0 
3.3 V <---> adcPin.read() = 1.0

-------------------------------------------------------------------------------

LM35 output: 10 mV/°C (2°C to 150°C)

  Convert to Celsius degrees:

    adcPin[V] = adcPin.read() * 3.3
    LM35_T[°C] = adcPin[V] * 100.0
    
    ==> T[°C] = adcPin.read() * 330.0  
  
  Convert to Fahrenheit degrees:
  
    T[°F] = 9.0/5.0 * T[°C] + 32.0
*/

//=====[Libraries]=============================================================

#include "mbed.h"
#include "arm_book_lib.h"

//=====[Declaration and intitalization of public global objects]===============

Serial uartUsb(USBTX, USBRX);

AnalogIn potentiometer(A0);
AnalogIn lm35(A1); // 10 mV/°C

//=====[Declaration and intitalization of public global variables]=============

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

    uartUsb.printf("\r\nWARNING: The readings are printed continuously ");
    uartUsb.printf("until "q" or "Q" are pressed.\r\n\r\n");
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
                uartUsb.printf("Potentiometer reading: %f\r\n",
                               potentiometerReading);
                wait_ms( 100 - (7+25) );
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
                uartUsb.printf("LM35 reading: %f\r\n", lm35Reading);
                wait_ms( 100 - (7+16) );
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
                uartUsb.printf("LM35: %f °C\r\n", lm35TempC);
                wait_ms( 100 - (9+11) );
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
                uartUsb.printf("LM35: %f °F\r\n", lm35TempF);
                wait_ms( 100 - (9+11) );
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
                uartUsb.printf("LM35: %f °C, Potentiometer scaled to °C: %f\r\n",
                               lm35TempC, potentiometerScaledToC);
                wait_ms( 100 - (9+9+38) );
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
                uartUsb.printf("LM35: %f °F, Potentiometer scaled to °F: %f\r\n",
                               lm35TempF, potentiometerScaledToF);
                wait_ms( 100 - (9+9+38) );
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