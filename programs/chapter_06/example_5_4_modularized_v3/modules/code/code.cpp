//=====[Libraries]=============================================================

#include "mbed.h"
#include "arm_book_lib.h"

#include "code.h"

#include "user_interface.h"
#include "pc_serial_com.h"
#include "date_and_time.h"
#include "temperature_sensor.h"
#include "gas_sensor.h"
#include "matrix_keypad.h"

//=====[Declaration of private constants]======================================

//=====[Declaration of private data types]=====================================

typedef struct{
    bool codeCompleteSaved;
    int codeIndex;
    char code[CODE_NUMBER_OF_KEYS];
} code_t;

//=====[Declaration and intitalization of public global objects]===============

//=====[Declaration and intitalization of private global variables]============

static int numberOfIncorrectCodes = 0;
static char codeSequence[CODE_NUMBER_OF_KEYS]   = { '1', '8', '0', '5' };

static code_t codeFromMatrixKeypad;
static code_t codeFromPcSerialCom;

//=====[Declarations (prototypes) of private functions]========================

static void codeDeactivate();

//=====[Implementations of public functions]===================================

void codeWrite( char* newCodeSequence )
{
    for ( int i = 0; i < CODE_NUMBER_OF_KEYS; i++) {
        codeSequence[i] = newCodeSequence[i];
    }
}

bool codeMatch( char* codeToCompare )
{
    int i;
    for (i = 0; i < CODE_NUMBER_OF_KEYS; i++) {
        if ( codeSequence[i] != codeToCompare[i] ) {
            return false;
        }
    }
    return true;
}

bool codeMatchFrom( codeOrigin_t codeOrigin )
{
    bool codeIsCorrect = false;
    
/*
    codeFromMatrixKeypad.codeCompleteSaved; // bool
    codeFromMatrixKeypad.codeIndex; // int
    codeFromMatrixKeypad.code[0]; // char []

    codeFromPcSerialCom.codeCompleteSaved; // bool
    codeFromPcSerialCom.codeIndex; // int
    codeFromPcSerialCom.code[0]; // char [] 
*/
    switch (codeOrigin) {
        case CODE_KEYPAD:
        break;
        case CODE_PC_SERIAL:
        break;
        default:
        break;
    }
    /*/
    if( codeCompleteSaved ) {
        codeCompleteSaved = false;
        codeIsCorrect = codeMatch(alarmCodeFromMatrixKeypad);
        if( codeIsCorrect ) {
            //uartUsb.printf( "Code is correct!!\r\n" );
        } else {
            //uartUsb.printf( "Code is incorrect.\r\n" );
            incorrectCodeStateWrite(ON);
            numberOfIncorrectCodes++;
        }
    }*/
    return codeIsCorrect;
}

bool codeFromPcSerialComMatch()
{
    return pcSerialComCodeMatch();
}


//=====[Implementations of private functions]==================================

static void codeDeactivate()
{
    systemBlockedStateWrite(OFF);
    incorrectCodeStateWrite(OFF);
    numberOfIncorrectCodes = 0;
    //matrixKeypadCodeIndex  = 0;
}