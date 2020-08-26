//=====[Libraries]=============================================================

#include "mbed.h"
#include "arm_book_lib.h"
#include "display.h"

//=====[Declaration of private defines]========================================

//=====[Declaration of private data types]=====================================

//=====[Declaration and initialization of public global objects]===============

SPI spi(SPI_MOSI, SPI_MISO, SPI_SCK);
DigitalOut spiSS(SPI_CS);
DigitalOut spiReset(D8);

//=====[Declaration of external public global variables]=======================

//=====[Declaration and initialization of public global variables]=============

uint8_t image[(128 * 64)/8];

//=====[Declaration and initialization of private global variables]============

//=====[Declarations (prototypes) of private functions]========================

uint8_t startRow, startCol, endRow, endCol; // coordinates of the dirty rectangle
uint8_t numRows = 64;
uint8_t numCols = 128;

//=====[Implementations of public functions]===================================

displayStatus_t displayCommandWrite( uint8_t command )
{
    
    spi.lock();
    spiSS = 1;
	spi.write(0xf8+(0<<1));  // send the SYNC + RS(0)
	spi.write(command&0xf0);  // send the higher nibble first
	spi.write((command<<4)&0xf0);  // send the lower nibble
    spiSS = 0;
    spi.unlock();
    
    return DISPLAY_NO_ERR;
}
    

displayStatus_t displayDataWrite( uint8_t data )
{
    spi.lock();
    spiSS = 1;
	spi.write(0xf8+(1<<1));  // send the SYNC + RS(1)
	spi.write(data&0xf0);  // send the higher nibble first
	spi.write((data<<4)&0xf0);  // send the lower nibble
    spiSS = 0;
	spi.unlock();
    
    return DISPLAY_NO_ERR;
}

displayStatus_t displayInit( int charsPerLine, int charLines,
                             int charPixelWidth, int charPixelHeight )
{
    
    spi.format(8,3);
    spi.frequency(10000);
    spiReset = OFF;  // RESET=0
	delay(10);   // wait for 10ms
	spiReset = ON;  // RESET=1

	delay(50);   //wait for >40 ms


	displayCommandWrite(0x30);  // 8bit mode
	wait_us(110);  //  >100us delay

	displayCommandWrite(0x30);  // 8bit mode
	wait_us(40);  // >37us delay

	displayCommandWrite(0x08);  // D=0, C=0, B=0
	wait_us(110);  // >100us delay

	displayCommandWrite(0x01);  // clear screen
	delay(12);  // >10 ms delay

	displayCommandWrite(0x06);  // cursor increment right no shift
	delay(1);  // 1ms delay

    //initialization end

	displayCommandWrite(0x0C);  // D=1, C=0, B=0
    delay(3);  // 1ms delay

	displayCommandWrite(0x02);  // return to home
	delay(3);  // 1ms delay
    
    return DISPLAY_NO_ERR;                            
}

displayStatus_t displayCharPositionWrite( uint8_t charPositionX, 
                                         uint8_t charPositionY )
{
    uint8_t firstCharAddress[] = { 0x80, 0x90, 0x88, 0x98 };

    displayCommandWrite( firstCharAddress[charPositionY] + charPositionX/2 );
   
    return DISPLAY_NO_ERR;
}

displayStatus_t displayStringWrite( char* str )
{
    while (*str)
    {
        displayDataWrite(*str++);
    }
    return DISPLAY_NO_ERR;
}

//=====[Implementations of private functions]==================================