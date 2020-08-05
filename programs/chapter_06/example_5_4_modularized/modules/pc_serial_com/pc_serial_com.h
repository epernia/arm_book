//=====[#include guards - begin]===============================================

#ifndef _PC_SERIAL_COM_H_
#define _PC_SERIAL_COM_H_

//=====[Libraries]=============================================================

//=====[Declaration of public constants]=======================================

//=====[Declaration of public data types]======================================

//=====[Declarations (prototypes) of public functions]=========================

char pcSerialComReadChar();
void pcSerialComWrite( const char* str );
void pcSerialComUpdate();
bool pcSerialComCodeMatch();
bool pcSerialComCodeCompleteRead();
void pcSerialComCodeCompleteWrite( bool state );

//=====[#include guards - end]=================================================

#endif // _PC_SERIAL_COM_H_