#include "mbed.h"
#include "arm_book_lib.h"
#include "display.h"
#include "sapi.h"

//=====[Declaration of private defines]========================================

#define DISPLAY_EN_PULSE_WAIT_US   25    // 25 us
#define DISPLAY_LOW_WAIT_US        25    // 25 us
#define DISPLAY_HIGH_WAIT_US       100   // 100 us

#define DISPLAY_CMD_WAIT_US        110   // Wait time for every command 45 us, except:
#define DISPLAY_CLR_DISP_WAIT_MS   3     // - Clear Display 1.52 ms
#define DISPLAY_RET_HOME_WAIT_MS   3     // - Return Home  1.52 ms

#define DISPLAY_STARTUP_WAIT_MS    1000  // Put according to display

//=====[Declaration of private data types]=====================================

//=====[Declaration and initialization of public global objects]===============

// Configure display Pins as Outputs
DigitalOut displayEN( D2 );
DigitalOut displayRS( D3 );
DigitalOut displayD4( D4 );
DigitalOut displayD5( D5 );
DigitalOut displayD6( D6 );
DigitalOut displayD7( D7 );

//=====[Declaration of external public global variables]=======================

//=====[Declaration and initialization of public global variables]=============

//=====[Declaration and initialization of private global variables]============

static display_t lcd;

//=====[Declarations (prototypes) of private functions]========================

static void displayEnablePulse();
static void displaySendNibble( uint8_t nibble );

static void displayDelay_us( int microseconds );
static void displayDelay_ms( int miliseconds );

//=====[Implementations of public functions]===================================

displayStatus_t displayCommandWrite( uint8_t command )
{
    displaySendNibble( command & 0xF0 );    // Send high nibble to D7-D4
    displayRS = OFF;                        // RS = 0 for command
    displayEnablePulse();
    displayDelay_us( DISPLAY_LOW_WAIT_US ); // Wait
    displaySendNibble( command << 4 );      // Send low nibble to D7-D4
    displayEnablePulse();

    return DISPLAY_NO_ERR;
}

displayStatus_t displayDataWrite( uint8_t data )
{
    displaySendNibble( data & 0xF0 );   // Send high nibble to D7-D4
    displayRS = ON;                     // RS = 1 for data (RW = 0, becaus connected to GND )
    displayEnablePulse();
    displaySendNibble( data << 4 );     // Send low nibble to D7-D4
    displayEnablePulse();

    return DISPLAY_NO_ERR;
}

displayStatus_t displayInit( int charsPerLine, int charLines ) 
{
	lcd.charsPerLine = charsPerLine;
	lcd.charLines = charLines;
	lcd.charPositionX = 0;
	lcd.charPositionY = 0;

   // Configure LCD for 4-bit mode
    displayRS = OFF;
    displayEN = OFF;
	
	displayDelay_ms(DISPLAY_STARTUP_WAIT_MS); // Depends on display, wait for >40 ms

	displayCommandWrite(0x30);
	displayDelay_us(DISPLAY_CMD_WAIT_US);     //  >100us delay

	displayCommandWrite(0x30);
	displayDelay_us(40);                      // >37us delay

    displayCommandWrite( DISPLAY_CMD_DISPLAY_CONTROL );
	displayDelay_ms(DISPLAY_CMD_WAIT_US);

	displayCommandWrite(DISPLAY_CMD_CLEAR);
	displayDelay_ms(12);                      // >10 ms delay

	displayCommandWrite(0x06);
	displayDelay_ms(1);



	displayCommandWrite(0x0C);
    displayDelay_ms(3);

	displayCommandWrite(DISPLAY_CMD_HOME);
	displayDelay_ms(3);

    return DISPLAY_NO_ERR;                            
}

displayStatus_t displayCharPositionWrite( uint8_t charPositionX, 
                                          uint8_t charPositionY )
{
    if( charPositionX >= lcd.charsPerLine || charPositionY >= lcd.charLines ) {
        return DISPLAY_ERR_INVALID_POSITION;
    }

    uint8_t firstCharAdress[] = { 0x80, 0xC0, 0x94, 0xD4 };                // See table 12-5
    displayCommandWrite( firstCharAdress[charPositionY] + charPositionX ); // Start in (x,y) = (0,0)
    displayDelay_us( DISPLAY_HIGH_WAIT_US );                               // Wait

    lcd.charPositionX = charPositionX;
    lcd.charPositionY = charPositionY;

    return DISPLAY_NO_ERR;
}

displayStatus_t displayStringWrite( char* str )
{
    while (*str) {
        displayDataWrite(*str++);
    }
    return DISPLAY_NO_ERR;
}

//=====[Implementations of private functions]==================================

static void displayEnablePulse()
{
   displayEN = ON;                              // EN = 1 for H-to-L pulse
   displayDelay_us( DISPLAY_EN_PULSE_WAIT_US ); // Wait to make EN wider
   displayEN = OFF;                             // EN = 0 for H-to-L pulse
}

static void displaySendNibble( uint8_t nibble )
{
   displayD7 = nibble & 0x80;
   displayD6 = nibble & 0x40;
   displayD5 = nibble & 0x20;
   displayD4 = nibble & 0x10;
}

static void displayDelay_us( int microseconds )
{
   wait_us( microseconds );
}

static void displayDelay_ms( int miliseconds )
{
   delay( miliseconds );
}