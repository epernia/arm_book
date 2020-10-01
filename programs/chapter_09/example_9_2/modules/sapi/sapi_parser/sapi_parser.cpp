/* Copyright 2016, Eric Pernia.
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

// File creation date: 2016-03-01

//==================[inclusions]===============================================

#include "sapi_parser.h"
#include "string.h"

//==================[internal functions declaration]===========================

//==================[external functions definition]============================

void parserInit( parser_t* instance,
                 char const* stringPattern, uint16_t stringPatternLen, 
                 tick_t timeout )
{
    instance->state            = PARSER_STOPPED;
    instance->stringPattern    = stringPattern;
    instance->stringPatternLen = stringPatternLen;
    instance->timeout          = timeout;
}

void parserStart( parser_t* instance )
{
    instance->state = PARSER_START;
}

void parserStop( parser_t* instance )
{
    instance->state = PARSER_STOPPED;
}

// Check for Receive a given pattern
parserStatus_t parserPatternMatchOrTimeout(
    parser_t* instance, char const receivedChar )
{
   switch( instance->state ) {

   // Initial state
   case PARSER_STOPPED:
   // Final states
   case PARSER_PATTERN_MATCH:
   case PARSER_TIMEOUT:
      break;

   case PARSER_START:
      delayInit( &(instance->delay), instance->timeout );
      instance->stringIndex = 0;
      instance->state = PARSER_RECEIVING;
      break;

   case PARSER_RECEIVING:
      if( (instance->stringPattern)[(instance->stringIndex)] == receivedChar ) {
         (instance->stringIndex)++;
         if( (instance->stringIndex) == (instance->stringPatternLen - 1) ) {
            instance->state = PARSER_PATTERN_MATCH;
         }
      }
      if( delayRead( &(instance->delay) ) ) {
         instance->state = PARSER_TIMEOUT;
      }
      break;

   default:
      instance->state = PARSER_STOPPED;
      break;
   }

   return instance->state;
}

//==================[internal functions definition]============================

//==================[end of file]==============================================
