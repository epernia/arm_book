//=====[#include guards - begin]===============================================

#ifndef _NON_BLOCKING_DELAY_H_
#define _NON_BLOCKING_DELAY_H_

//==================[Inclusions]===============================================

#include "mbed.h"

//=====[Declaration of public defines]=======================================

//=====[Declaration of public data types]======================================

typedef uint64_t tick_t;

typedef struct{
   tick_t startTime;
   tick_t duration;
   bool isRunning;
} nonBlockingDelay_t;

//=====[Declarations (prototypes) of public functions]=========================

void tickInit();

void nonBlockingDelayInit( nonBlockingDelay_t* delay, tick_t durationValue );
bool nonBlockingDelayRead( nonBlockingDelay_t* delay );
void nonBlockingDelayWrite( nonBlockingDelay_t* delay, tick_t durationValue );

//=====[#include guards - end]=================================================

#endif // _NON_BLOCKING_DELAY_H_