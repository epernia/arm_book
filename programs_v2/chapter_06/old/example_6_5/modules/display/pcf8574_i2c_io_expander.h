//=====[#include guards - begin]===============================================

#ifndef _PCF8574_H_
#define _PCF8574_H_

//=====[Libraries]=============================================================

#include "mbed.h"

//=====[Declaration of public defines]=======================================

//=====[Declaration of public data types]======================================

typedef enum{
   PCF8574_GPIO_INPUT = 1,
   PCF8574_GPIO_OUTPUT = 0,
} pcf8574t_gpioDirection_t;

//=====[Declarations (prototypes) of public functions]=========================

void pcf8574tInit( bool a0, bool a1, bool a2 );

void pcf8574tGpioPortInit( uint8_t directions ); // bit=0: output, bit=1: input // bit=0: output, bit=1: input
void pcf8574tGpioPortWrite( uint8_t portValue );
uint8_t pcf8574tGpioPortRead( void );

void pcf8574tGpioInit( uint8_t pin, pcf8574t_gpioDirection_t direction );
bool pcf8574tGpioRead( uint8_t pin );
void pcf8574tGpioWrite( uint8_t pin, bool value );

//=====[#include guards - end]=================================================

#endif // _PCF8574_H_