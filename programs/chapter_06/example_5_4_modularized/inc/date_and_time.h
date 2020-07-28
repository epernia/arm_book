//=====[#include guards - begin]===============================================

#ifndef _DATE_AND_TIME_H_
#define _DATE_AND_TIME_H_

//=====[Libraries]=============================================================

//=====[Declaration of public constants]=======================================

//=====[Declaration of public data types]======================================

#define SYSTEM_TIME_INCREMENT_MS   10

//=====[Declarations (prototypes) of public functions]=========================

char* dateAndTimeReadString();

void dateAndTimeWriteIndividualValues( int year, int month, int day, 
                                       int hour, int minute, int second );

//=====[#include guards - end]=================================================

#endif // _DATE_AND_TIME_H_