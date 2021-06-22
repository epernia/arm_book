//=====[Libraries]=============================================================

#include "non_blocking_delay.h"

//=====[Declaration of private defines]========================================

//=====[Declaration of private data types]=====================================

//=====[Declaration and initialization of public global objects]===============

//=====[Declaration of external public global variables]=======================

//=====[Declaration and initialization of public global variables]=============

//=====[Declaration and initialization of private global variables]============

static Ticker ticker;
static tick_t tickCounter;

//=====[Declarations (prototypes) of private functions]========================

void tickerCallback();
tick_t tickRead();

//=====[Implementations of public functions]===================================

void tickInit()
{
    ticker.attach( tickerCallback, ((float) 0.001 ));
}

void nonBlockingDelayInit( nonBlockingDelay_t * delay, tick_t durationValue )
{
   delay->duration = durationValue;
   delay->isRunning = false;
}

bool nonBlockingDelayRead( nonBlockingDelay_t * delay )
{
   bool timeArrived = false;

   if( !delay->isRunning ) {
      delay->startTime = tickCounter;
      delay->isRunning = true;
   } else {
      if ( (tick_t)( tickCounter - delay->startTime) >= delay->duration ) {
         timeArrived = true;
         delay->isRunning = false;
      }
   }

   return timeArrived;
}

void nonBlockingDelayWrite( nonBlockingDelay_t * delay, tick_t durationValue )
{
   delay->duration = durationValue;
}

//=====[Implementations of private functions]==================================

void tickerCallback( void ) 
{
    tickCounter++;
}