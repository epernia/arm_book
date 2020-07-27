//=====[#include guards - begin]===============================================

#ifndef _EVENT_LOG_H_
#define _EVENT_LOG_H_

//=====[Libraries]=============================================================

#include "mbed.h"
#include "arm_book_lib.h"

//=====[Declaration of public data types]======================================



//=====[Declaration of public constants]=======================================



//=====[Declarations (prototypes) of public functions]=========================

void eventLogUpdate();

void systemElementStateUpdate( bool lastState,
                               bool currentState,
                               const char* elementName );

//=====[#include guards - end]=================================================

#endif // _EVENT_LOG_H_