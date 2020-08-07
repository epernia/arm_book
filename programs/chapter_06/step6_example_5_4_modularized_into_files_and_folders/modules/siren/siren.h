//=====[#include guards - begin]===============================================

#ifndef _SIREN_H_
#define _SIREN_H_

//=====[Libraries]=============================================================

//=====[Declaration of public constants]=======================================

//=====[Declaration of public data types]======================================

//=====[Declarations (prototypes) of public functions]=========================

void sirenInit();
bool sirenStateRead();
void sirenStateWrite( bool state );
void sirenIndicatorUpdate( int blinkTime );

//=====[#include guards - end]=================================================

#endif // _SIREN_H_