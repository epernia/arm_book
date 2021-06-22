//=====[#include guards - begin]===============================================

#ifndef _CODE_H_
#define _CODE_H_

//=====[Declaration of public defines]=========================================

#define CODE_NUMBER_OF_KEYS   4

//=====[Declaration of public data types]======================================

typedef enum{
    CODE_KEYPAD,
    CODE_PC_SERIAL,
} codeOrigin_t;

//=====[Declarations (prototypes) of public functions]=========================

void codeWrite( char* newCodeSequence );
bool codeMatchFrom( codeOrigin_t codeOrigin );

//=====[#include guards - end]=================================================

#endif // _CODE_H_