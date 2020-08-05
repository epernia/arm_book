//=====[#include guards - begin]===============================================

#ifndef _PC_SERIAL_COM_H_
#define _PC_SERIAL_COM_H_

//=====[Libraries]=============================================================

//=====[Declaration of public constants]=======================================

//=====[Declaration of public data types]======================================

//=====[Declarations (prototypes) of public functions]=========================

char pcSerialComReadChar();
void pcSerialComWrite( const char* str );
void pcSerialComCommandUpdate();
bool pcSerialComCodeMatch();
void pcSerialComCommandUpdateEnableWrite( bool enable );

//=====[#include guards - end]=================================================

#endif // _PC_SERIAL_COM_H_