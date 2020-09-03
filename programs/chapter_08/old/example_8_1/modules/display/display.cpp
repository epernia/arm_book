//=====[Libraries]=============================================================

#include "mbed.h"
#include "arm_book_lib.h"
#include "display.h"

//=====[Declaration of private defines]========================================

//=====[Declaration of private data types]=====================================

//=====[Declaration and initialization of public global objects]===============
SPI spiDisplay(SPI_MOSI, SPI_MISO, SPI_SCK);
//SPI spiDisplay(D22, D25, D23);
DigitalOut spiDisplaySS(SPI_CS);
//SPI spiDisplay(D22, D25, D23);
//DigitalOut spiDisplaySS(D7);
DigitalOut spiDisplayReset(D8);

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
    
    spiDisplay.lock();
    spiDisplaySS = 1;
	spiDisplay.write(0xf8+(0<<1));  // send the SYNC + RS(0)
	spiDisplay.write(command&0xf0);  // send the higher nibble first
	spiDisplay.write((command<<4)&0xf0);  // send the lower nibble
    spiDisplaySS = 0;
    spiDisplay.unlock();
    
    return DISPLAY_NO_ERR;
}
    

displayStatus_t displayDataWrite( uint8_t data )
{
    spiDisplay.lock();
    spiDisplaySS = 1;
	spiDisplay.write(0xf8+(1<<1));  // send the SYNC + RS(1)
	spiDisplay.write(data&0xf0);  // send the higher nibble first
	spiDisplay.write((data<<4)&0xf0);  // send the lower nibble
    spiDisplaySS = 0;
	spiDisplay.unlock();
    
    return DISPLAY_NO_ERR;
}

displayStatus_t displayInit( displayType_t type, displayConnection_t connection,
                            int charsPerLine, int charLines,
                            int charPixelWidth, int charPixelHeight,
                            int glcdPixelWidth, int glcdPixelHeight ) 
{
    
    spiDisplay.format(8,3);
    spiDisplay.frequency(1000000);
    //spiDisplayReset = OFF;  // RESET=0
	delay(10);   // wait for 10ms
	//spiDisplayReset = ON;  // RESET=1

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

displayStatus_t displayModeWrite( displayMode_t mode )
{
    if ( mode == DISPLAY_MODE_GRAPHIC )
	{
		displayCommandWrite(0x30);  // 8 bit mode
		delay (1);
		displayCommandWrite(0x34);  // switch to Extended instructions
		delay (1);
		displayCommandWrite(0x36);  // enable graphics
		delay (1);
	}

	else if ( mode == DISPLAY_MODE_CHAR )
	{
		displayCommandWrite(0x30);  // 8 bit mode
		delay (1);
	}
    
    return DISPLAY_NO_ERR;
}

displayStatus_t displayBitmapWrite( uint8_t* bitmap )
{
    uint8_t x, y;
	for(y = 0; y < 64; y++)
	{
		if(y < 32)
		{
			for(x = 0; x < 8; x++)							// Draws top half of the screen.
			{												// In extended instruction mode, vertical and horizontal coordinates must be specified before sending data in.
				displayCommandWrite(0x80 | y);				// Vertical coordinate of the screen is specified first. (0-31)
				displayCommandWrite(0x80 | x);				// Then horizontal coordinate of the screen is specified. (0-8)
				displayDataWrite(bitmap[2*x + 16*y]);		// Data to the upper byte is sent to the coordinate.
				displayDataWrite(bitmap[2*x+1 + 16*y]);	// Data to the lower byte is sent to the coordinate.
			}
		}
		else
		{
			for(x = 0; x < 8; x++)							// Draws bottom half of the screen.
			{												// Actions performed as same as the upper half screen.
				displayCommandWrite(0x80 | (y-32));			// Vertical coordinate must be scaled back to 0-31 as it is dealing with another half of the screen.
				displayCommandWrite(0x88 | x);
				displayDataWrite(bitmap[2*x + 16*y]);
				displayDataWrite(bitmap[2*x+1 + 16*y]);
			}
		}

	}
    
    return DISPLAY_NO_ERR;
}

displayStatus_t displayPixelPositionWrite( int pixelPositionX, 
                                          int pixelPositionY,
                                          bool pixelValue )
{

  if (pixelPositionY < numRows && pixelPositionX < numCols)
  {
    uint8_t *p = image + ((pixelPositionY * (numCols/8)) + (pixelPositionX/8));
    *p |= 0x80u >> (pixelPositionX%8);

    *image = *p;

    // Change the dirty rectangle to account for a pixel being dirty (we assume it was changed)
    if (startRow > pixelPositionY) { startRow = pixelPositionY; }
    if (endRow <= pixelPositionY)  { endRow = pixelPositionY + 1; }
    if (startCol > pixelPositionX) { startCol = pixelPositionX; }
    if (endCol <= pixelPositionX)  { endCol = pixelPositionX + 1; }
  }

  return DISPLAY_NO_ERR;
}

//=====[Implementations of private functions]==================================