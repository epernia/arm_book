#include "mbed.h"
#include "arm_book_lib.h"
#include "pcf8574_i2c_io_expander.h"

//=====[Declaration of private defines]========================================

#define I2C_BASE_SLAVE_ADDRESS_7BIT 0x20

//=====[Declaration of private data types]=====================================

typedef struct{
    int i2cAddress;
    uint8_t gpioPortDirections;
    uint8_t gpioPortValue;
} pcf8574_t;

//=====[Declaration and initialization of public global objects]===============

I2C i2c( I2C_SDA, I2C_SCL );

//=====[Declaration of external public global variables]=======================

pcf8574_t pcf8574;

//=====[Declaration and initialization of public global variables]=============

//=====[Declaration and initialization of private global variables]============

//=====[Declarations (prototypes) of private functions]========================

//=====[Implementations of public functions]===================================

void pcf8574tInit( bool a0, bool a1, bool a2 )
{
    const uint8_t addr7bit = I2C_BASE_SLAVE_ADDRESS_7BIT | 
                             ((a2)<<2) | ((a1)<<1) | a0;
    pcf8574.i2cAddress = addr7bit << 1; // MbedOS library use 8 bit I2C address

    // Initialize I2C bus

    pcf8574tGpioPortInit( 0x00 );  // Init all GPIOs as outputs
    pcf8574tGpioPortWrite( 0x00 ); // Init all as zeros
}

void pcf8574tGpioPortInit( uint8_t directions ) // bit=0: output, bit=1: input // bit=0: output, bit=1: input
{
   pcf8574.gpioPortDirections = directions;
   char transmitDataBuffer = directions; 
   //i2cWrite( pcf8574tI2cPort, pcf8574tI2cAddress, &directions, 1, true );
   i2c.write( pcf8574.i2cAddress, &transmitDataBuffer, 1, false );
}

void pcf8574tGpioPortWrite( uint8_t portValue )
{
   pcf8574.gpioPortValue = portValue;
   // Or with pcf8574TGpioPortDirections to keep pins initialized as inputs
   char transmitDataBuffer = portValue | pcf8574.gpioPortDirections; 
   //i2cWrite( pcf8574tI2cPort, pcf8574tI2cAddress, &transmitDataBuffer, 1, true );
   i2c.write( pcf8574.i2cAddress, &transmitDataBuffer, 1, false );
}

uint8_t pcf8574tGpioPortRead( void )
{
   char receiveDataBuffer = 0;
   //i2cRead( pcf8574tI2cPort, pcf8574tI2cAddress, &receiveDataBuffer, 1, true );
   i2c.read( pcf8574.i2cAddress, &receiveDataBuffer, 1, false );
   return (uint8_t)receiveDataBuffer;
}

void pcf8574tGpioInit( uint8_t pin, pcf8574t_gpioDirection_t direction )
{
   uint8_t directions = pcf8574.gpioPortDirections;
   if( direction ) {
      directions |= (1<<pin);
   } else {
      directions &= ~(1<<pin);
   }
   pcf8574tGpioPortInit( directions );
}

bool pcf8574tGpioRead( uint8_t pin )
{
   return pcf8574tGpioPortRead() & (1<<pin);
}

void pcf8574tGpioWrite( uint8_t pin, bool value )
{
   uint8_t portValue = pcf8574.gpioPortValue;
   if( value ) {
      portValue |= (1<<pin);
   } else {
      portValue &= ~(1<<pin);
   }
   pcf8574tGpioPortWrite( portValue );
}

//=====[Implementations of private functions]==================================