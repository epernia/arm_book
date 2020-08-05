//=====[#include guards - begin]===============================================

#ifndef _SIREN_H_
#define _SIREN_H_

//=====[Libraries]=============================================================

//=====[Declaration of public constants]=======================================

#define SIREN_BLINKING_TIME_GAS               1000
#define SIREN_BLINKING_TIME_OVER_TEMP          500
#define SIREN_BLINKING_TIME_GAS_AND_OVER_TEMP  100

//=====[Declaration of public data types]======================================

//=====[Declarations (prototypes) of public functions]=========================

void sirenInit();
bool sirenStateRead();
void sirenStateWrite( bool state );
void sirenIndicatorUpdate();

//=====[#include guards - end]=================================================

#endif // _SIREN_H_