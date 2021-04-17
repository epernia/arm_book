#include "display.h"
#include "sapi.h"

//=====[Main function, the program entry point after power on or reset]========

int main()
{
    tickInit(1); // 1ms de tick, para que funcione delay() y delay no bloqueante

    displayInit( 20, 4 );

    char number[3];

    while (true) {
        displayStringWrite( "Hola" );
        displayCharPositionWrite( 0, 1 );
        displayStringWrite( "Ariel" );
        displayCharPositionWrite( 5, 3 );
        displayStringWrite( "Como estas?" );

        delay( 1000 );
        displayClearAndHome();

        displayStringWrite( "Ya funciono de " );
        sprintf( number, "%d", 10 );
        displayStringWrite( number );

        delay( 1000 );
        displayClearAndHome();
    }
}