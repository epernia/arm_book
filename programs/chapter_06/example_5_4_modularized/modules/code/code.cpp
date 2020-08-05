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

//=====[Declaration and intitalization of public global objects]===============

//=====[Declaration and intitalization of extern global variables]=============

extern char codeSequenceFromUserInterface[CODE_NUMBER_OF_KEYS];
extern char codeSequenceFromPcSerialCom[CODE_NUMBER_OF_KEYS];

//=====[Declaration and intitalization of private global variables]============

static int numberOfIncorrectCodes = 0;
static char codeSequence[CODE_NUMBER_OF_KEYS] = { '1', '8', '0', '5' };

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
    switch (codeOrigin) {
        case CODE_KEYPAD:
            if( userInterfaceCodeCompleteRead() ) {
                codeIsCorrect = codeMatch(codeSequenceFromUserInterface);
                userInterfaceCodeCompleteWrite(false);
                if ( codeIsCorrect ) {
                    codeDeactivate();
                } else {
                    incorrectCodeStateWrite(ON);
                    numberOfIncorrectCodes++;
                }
            }


        break;
        case CODE_PC_SERIAL:
            if( pcSerialComCodeCompleteRead() ) {
                codeIsCorrect = codeMatch(codeSequenceFromPcSerialCom);
                pcSerialComCodeCompleteWrite(false);
                if ( codeIsCorrect ) {
                    codeDeactivate();
                    pcSerialComWrite( "\r\nThe code is correct\r\n\r\n" );
                } else {
                    incorrectCodeStateWrite(ON);
                    numberOfIncorrectCodes++;
                    pcSerialComWrite( "\r\nThe code is incorrect\r\n\r\n" );
                }
            }

        break;
        default:
        break;
    }

    if ( numberOfIncorrectCodes >= 5 ) {
        systemBlockedStateWrite(ON);
    }

    return codeIsCorrect;
}

//=====[Implementations of private functions]==================================

static void codeDeactivate()
{
    systemBlockedStateWrite(OFF);
    incorrectCodeStateWrite(OFF);
    numberOfIncorrectCodes = 0;
}