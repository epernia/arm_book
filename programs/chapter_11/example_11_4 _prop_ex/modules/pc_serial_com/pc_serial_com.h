//=====[#include guards - begin]===============================================

#ifndef _PC_SERIAL_COM_H_
#define _PC_SERIAL_COM_H_

//=====[Libraries]=============================================================

//=====[Declaration of public defines]=======================================

#define PC_SERIAL_COM_BAUD_RATE   115200

//=====[Declaration of public data types]======================================

//=====[Declarations (prototypes) of public functions]=========================

void pcSerialComInit();
char pcSerialComCharRead();
void pcSerialComCharWrite( char c );
void pcSerialComStringWrite( const char* str );
void pcSerialComIntWrite( int number );
void pcSerialComFloatWrite( float number );
void pcSerialComUpdate();
bool pcSerialComCodeCompleteRead();
void pcSerialComCodeCompleteWrite( bool state );

//=====[#include guards - end]=================================================

#endif // _PC_SERIAL_COM_H_
