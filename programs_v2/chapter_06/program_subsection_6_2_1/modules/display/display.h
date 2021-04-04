//=====[#include guards - begin]===============================================

#ifndef _DISPLAY_H_
#define _DISPLAY_H_

//=====[Libraries]=============================================================

#include "mbed.h"

//=====[Declaration of public defines]=======================================

//=====[Declaration of public data types]======================================

//=====[Declarations (prototypes) of public functions]=========================

void displayInit( void );
 
void displayCharPositionWrite( uint8_t charPositionX, uint8_t charPositionY );

void displayStringWrite( char const * str );

void displayClear( void );

//=====[#include guards - end]=================================================

#endif // _DISPLAY_H_