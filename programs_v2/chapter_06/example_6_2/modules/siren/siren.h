//=====[#include guards - begin]===============================================

#ifndef _SIREN_H_
#define _SIREN_H_

//=====[Declaration of public defines]=========================================

//=====[Declaration of public data types]======================================

//=====[Declarations (prototypes) of public functions]=========================

void sirenInit();
bool sirenStateRead();
void sirenStateWrite( bool state );
void sirenUpdate( int strobeTime );

//=====[#include guards - end]=================================================

#endif // _SIREN_H_
