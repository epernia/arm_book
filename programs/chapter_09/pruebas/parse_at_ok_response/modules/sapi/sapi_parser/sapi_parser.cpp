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

//static Serial uartParser( USBTX, USBRX );

// D42 = PE_8 = UART7_TX
// D41 = PE_7 = UART7_RX
static Serial uartParser( D42, D41 );

//==================[external functions definition]============================

// UART used for the parser ---------------------------------------------------

void parserUartInit()
{
    uartParser.baud(115200);
}

//static bool parserUartReadByte( Serial uart, uint8_t* receivedByte );
bool parserUartByteRead( uint8_t* receivedByte )
{
    if( uartParser.readable() ) {
        *receivedByte = uartParser.getc();
        return true;
    }
    return false;
}

void parserUartRxFlush() //Serial uart )
{
    while( uartParser.readable() ) {
        uartParser.getc();
    }
}

//void parserUartByteWrite( Serial uart, uint8_t sendedByte );
void parserUartByteWrite( uint8_t sendedByte )
{
    uartParser.putc( (char)(sendedByte) );
}

void parserUartStringWrite( char const* str )
{
    while ( *str != NULL ) {
        parserUartByteWrite( (uint8_t)*str );
        str++;
    }
}

// Parser over UART reception functions ---------------------------------------

void parserInit( parser_t* instance, //Serial* uart,
                 char const* stringPattern, uint16_t stringPatternLen, 
                 tick_t timeout )
{
    instance->state = PARSER_STOPPED;
    //instance->uart = uart;
    instance->stringPattern =  stringPattern;
    instance->stringPatternLen = stringPatternLen;
    instance->timeout = timeout;    
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
parserStatus_t parserPatternMatchOrTimeout( parser_t* instance )
{
   uint8_t receiveByte;

   switch( instance->state ) {

   case PARSER_STOPPED:
      break;

   case PARSER_START:
      parserUartRxFlush();// instance->uart );
      delayInit( &(instance->delay), instance->timeout );
      instance->stringIndex = 0;
      instance->state = PARSER_RECEIVING;
      break;

   case PARSER_RECEIVING:
      if( parserUartByteRead( /*instance->uart,*/ &receiveByte ) ) {          
         if( (instance->stringPattern)[(instance->stringIndex)] == receiveByte ) {
            (instance->stringIndex)++;
            if( (instance->stringIndex) == (instance->stringPatternLen - 1) ) {
               instance->state = PARSER_RECEIVED_OK;
            }
         }
      }
      if( delayRead( &(instance->delay) ) ) {
         instance->state = PARSER_TIMEOUT;
      }
      break;

   case PARSER_RECEIVED_OK:
   case PARSER_TIMEOUT:
      //instance->state = PARSER_START; // Eso hacia que se autolance el parser
      break;

   default:
      instance->state = PARSER_STOPPED;
      break;
   }

   return instance->state;
}

// Store bytes until receive a given pattern
parserStatus_t parserSaveBytesUntilPatternMatchOrTimeout( 
    parser_t* instance,
    char* receiveBuffer,
    uint32_t* receiveBufferSize )
{

   uint8_t receiveByte;
   static uint32_t i = 0;

   switch( instance->state ) {

   case PARSER_STOPPED:
      break;

   case PARSER_START:
      parserUartRxFlush();// instance->uart );
      delayInit( &(instance->delay), instance->timeout );
      instance->stringIndex = 0;
      i = 0;
      instance->state = PARSER_RECEIVING;
      break;

   case PARSER_RECEIVING:
      if( parserUartByteRead( /*instance->uart,*/ &receiveByte ) ) {
         if( i < *receiveBufferSize ) {
            receiveBuffer[i] = receiveByte;
            i++;
         } else {
            instance->state = PARSER_FULL_BUFFER;
            *receiveBufferSize = i;
            i = 0;
            return instance->state;
         }
         if( (instance->stringPattern)[(instance->stringIndex)] == receiveByte ) {
            (instance->stringIndex)++;
            if( (instance->stringIndex) == (instance->stringPatternLen - 1) ) {
               instance->state = PARSER_RECEIVED_OK;
               *receiveBufferSize = i;
               i = 0;
            }
         }
      }

      if( delayRead( &(instance->delay) ) ) {
         instance->state = PARSER_TIMEOUT;
         *receiveBufferSize = i;
         i = 0;
      }

      break;

   case PARSER_RECEIVED_OK:
   case PARSER_TIMEOUT:
   case PARSER_FULL_BUFFER:
      //instance->state = PARSER_START; // Eso hacia que se autolance el parser
      break;

   default:
      instance->state = PARSER_STOPPED;
      break;
   }

   return instance->state;
}

//==================[internal functions definition]============================

//==================[end of file]==============================================