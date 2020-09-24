//=====[#include guards - begin]===============================================

#ifndef _EMBEDDED_SERVER_H_
#define _EMBEDDED_SERVER_H_

//=====[Libraries]=============================================================

//=====[Declaration of public defines]=========================================

//=====[Declaration of public data types]======================================

//=====[Declarations (prototypes) of public functions]=========================

void pcSerialComInit();
char pcSerialComCharRead();
void pcSerialComCharWrite( char c );
void pcSerialComStringWrite( const char* str );
void pcSerialComUpdate();
bool pcSerialComCodeCompleteRead();
void pcSerialComCodeCompleteWrite( bool state );

//=====[#include guards - end]=================================================

#endif // _EMBEDDED_SERVER_H_