/* Copyright 2015-2018, Eric Pernia.
 * Copyright 2018, Martin Ribelotta.
 * Copyright 2018, Eric Pernia.
 * All rights reserved.
 *
 * This file is part sAPI library for microcontrollers.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

// File creation date: 2015-09-23

#include <sapi_tick.h> // sAPI tick over Mbed implementation

//=====[Declaration of private defines]========================================

//=====[Declaration of private data types]=====================================

//=====[Declaration and initialization of public global objects]===============

//=====[Declaration and initialization of private global objects]==============

static Ticker ticker;

//=====[Declaration of external public global variables]=======================

//=====[Declaration and initialization of public global variables]=============

tick_t tickRateMS = 1; // Used by sapi_delay!!! Default 1ms

//=====[Declaration and initialization of private global variables]============

// This global variable holds the tick count
static volatile tick_t tickCounter;

static callBackFuncPtr_t tickHookFunction = NULL;
static void* callBackFuncParams = NULL;

//=====[Declarations (prototypes) of private functions]========================

static void tickerCallback( void );

//=====[Implementations of public functions]===================================

// Tick Initialization and rate configuration from 1 to 50 ms
bool tickInit( tick_t tickRateMSvalue )
{
    tickRateMS = tickRateMSvalue;
    tickPowerSet( ON );
    return true;  
}

// Enable or disable the peripheral energy and clock
void tickPowerSet( bool power )
{
    if( power ) {
        ticker.attach( tickerCallback, ((float) tickRateMS) / 1000.0 );
    } else {
        ticker.detach();
    }
}

// Read Tick Counter
tick_t tickRead( void )
{
    return tickCounter;
}

// Write Tick Counter
void tickWrite( tick_t ticks )
{
    tickCounter = ticks;
}

// Tick interrupt callback
bool tickCallbackSet( callBackFuncPtr_t tickCallback, void* tickCallbackParams )
{
    bool retVal = true;

    if( tickCallback != NULL ) {
        tickHookFunction = tickCallback;
    } else {
        retVal = false;
    }

    if( tickCallbackParams != NULL ) {
        callBackFuncParams = tickCallbackParams;
    } else {
        retVal &= false;
    }
    return retVal;
}

//=====[Implementations of private functions]==================================

static void tickerCallback( void )   // Before SysTick_Handler
{
   // Increment Tick counters
   tickCounter++;
   // Execute Tick Hook function if pointer is not NULL
   if( (tickHookFunction != NULL) ) {
      (* tickHookFunction )( callBackFuncParams );
   }
}