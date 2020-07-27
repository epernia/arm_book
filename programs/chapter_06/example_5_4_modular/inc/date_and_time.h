//=====[#include guards - begin]===============================================

#ifndef _DATE_AND_TIME_H_
#define _DATE_AND_TIME_H_

//=====[Libraries]=============================================================

#include "mbed.h"
#include "arm_book_lib.h"

//=====[Declaration of public data types]======================================



//=====[Declaration of public constants]=======================================



//=====[Declarations (prototypes) of public functions]=========================

char* dateAndTimeStringGet();

void dateAndTimeIndividualIntsSet( int year, int month, int day, 
                                   int hour, int minute, int second );

//=====[#include guards - end]=================================================

#endif // _DATE_AND_TIME_H_