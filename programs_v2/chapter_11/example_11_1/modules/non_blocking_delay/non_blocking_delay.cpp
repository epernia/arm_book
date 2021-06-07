//=====[Libraries]=============================================================

#include "non_blocking_delay.h"

//=====[Declaration of private defines]======================================

//=====[Declaration of private data types]=====================================

//=====[Declaration and initialization of public global objects]===============

//=====[Declaration of external public global variables]=======================

//=====[Declaration and initialization of public global variables]=============

//=====[Declaration and initialization of private global variables]============

static Ticker ticker;
static volatile tick_t tickCounter;
static tick_t tickRate = 1;

//=====[Declarations (prototypes) of private functions]========================

void tickerCallback( void );
tick_t tickRead( void );

//=====[Implementations of public functions]===================================

void tickInit( tick_t tickRateValue )
{
    tickRate = tickRateValue;
    ticker.attach( tickerCallback, ((float) tickRateValue) / 1000.0 );
}

void nonBlockingDelayInit( nonBlockingDelay_t * delay, tick_t durationValue )
{
   delay->duration = durationValue / tickRate;
   delay->isRunning = false;
}

bool nonBlockingDelayRead( nonBlockingDelay_t * delay )
{
   bool timeArrived = false;

   if( !delay->isRunning ) {
      delay->startTime = tickRead();
      delay->isRunning = true;
   } else {
      if ( (tick_t)(tickRead() - delay->startTime) >= delay->duration ) {
         timeArrived = true;
         delay->isRunning = false;
      }
   }

   return timeArrived;
}

void nonBlockingDelayWrite( nonBlockingDelay_t * delay, tick_t durationValue )
{
   delay->duration = durationValue / tickRate;
}

//=====[Implementations of private functions]==================================

void tickerCallback( void ) 
{
    tickCounter++;
}

tick_t tickRead( void )
{
    return tickCounter;
}