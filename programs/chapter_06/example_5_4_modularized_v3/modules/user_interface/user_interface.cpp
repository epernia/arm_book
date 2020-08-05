//=====[Libraries]=============================================================

#include "mbed.h"
#include "arm_book_lib.h"

#include "user_interface.h"

#include "code.h"
#include "siren.h"
#include "smart_home_system.h"
#include "date_and_time.h"
#include "temperature_sensor.h"
#include "gas_sensor.h"
#include "matrix_keypad.h"

//=====[Declaration of private constants]======================================

//=====[Declaration of private data types]=====================================

//=====[Declaration and intitalization of public global objects]===============

DigitalOut incorrectCodeLed(LED3);
DigitalOut systemBlockedLed(LED2);

//=====[Declaration and intitalization of private global variables]============

static bool incorrectCodeState = OFF;
static bool systemBlockedState = OFF;

//=====[Declarations (prototypes) of private functions]========================



//=====[Implementations of public functions]===================================

void userInterfaceInit()
{
    incorrectCodeLed = OFF;
    systemBlockedLed = OFF;
    matrixKeypadInit( SYSTEM_TIME_INCREMENT_MS );
}

void userInterfaceUpdate()
{
	userInterfaceMatrixKeypadUpdate();
    incorrectCodeIndicatorUpdate();
    systemBlockedIndicatorUpdate();
}

void userInterfaceMatrixKeypadUpdate()
{
    static int numberOfHaskKeyReleased = 0;
    char keyReleased = matrixKeypadUpdate();
    
    // Se me ocurrio que sea una sola funcion esta y la de uart que segun 
    // un parametro revise caracteres de una o la otra

    if( keyReleased != '\0' ) {
        //uartUsb.printf( "%c\r\n", keyReleased );

        if( sirenStateRead() ) {

            if( keyReleased == '#' ) {
                numberOfHaskKeyReleased++;
                if( numberOfHaskKeyReleased >= 2 ) {
                    //uartUsb.printf( "Double Press Hash: keypad code reset\r\n" );
                    numberOfHaskKeyReleased = 0;
            //matrixKeypadCodeIndex = 0;
                    incorrectCodeState = OFF;
                }
                return;
            }
/*
            if( matrixKeypadCodeIndex < CODE_NUMBER_OF_KEYS ) {
                uartUsb.printf( "Start save matrix keypad code\r\n" ); 
                alarmCodeFromMatrixKeypad[matrixKeypadCodeIndex] = keyReleased;
                uartUsb.printf( "  index: %d\r\n", matrixKeypadCodeIndex );
                matrixKeypadCodeIndex++;      
            } else {
                if( !incorrectCodeState ) {
                    matrixKeypadCodeCompleteSaved = true;                    
                }
            }
    */
        }
    }
}

bool incorrectCodeStateRead()
{
    return incorrectCodeState;
}

void incorrectCodeStateWrite( bool state )
{
    incorrectCodeState = state;
}

void incorrectCodeIndicatorUpdate()
{
    incorrectCodeLed = incorrectCodeStateRead();
}

bool systemBlockedStateRead()
{
    return systemBlockedState;
}

void systemBlockedStateWrite( bool state )
{
    systemBlockedState = state;
}

void systemBlockedIndicatorUpdate()
{
    systemBlockedLed = systemBlockedState;
}


//=====[Implementations of private functions]==================================

