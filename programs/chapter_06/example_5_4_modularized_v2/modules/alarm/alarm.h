//=====[#include guards - begin]===============================================

#ifndef _ALARM_H_
#define _ALARM_H_

//=====[Libraries]=============================================================

//=====[Declaration of public constants]=======================================

#define ALARM_CODE_NUMBER_OF_KEYS   4

//=====[Declaration of public data types]======================================

//=====[Declarations (prototypes) of public functions]=========================

void alarmInit();
void alarmUpdate();

bool alarmGasDetectorReadState();
bool alarmOverTempDetectorReadState();
bool alarmReadState();
bool alarmIncorrectCodeReadState();
bool alarmSystemBlockedReadState();

void alarmCodeWrite( char* newCodeSequence );
bool alarmCodeMatch( char* codeToCompare );

//=====[#include guards - end]=================================================

#endif // _ALARM_H_