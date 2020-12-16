//=====[Libraries]=============================================================

#include "mbed.h"
#include "arm_book_lib.h"
#include "display.h"
#include "pcf8574_i2c_io_expander.h"

//=====[Declaration of private defines]========================================

#define DISPLAY_POWERUP_WAIT_MS         1000 // Put according to display (more than 40 ms)

#define DISPLAY_INIT_WAIT_1_US          220  // >100us delay
#define DISPLAY_INIT_WAIT_2_US          40   // >37us delay
#define DISPLAY_INIT_WAIT_3_MS          220  // >100 ms delay
#define DISPLAY_INIT_WAIT_4_MS          12   // >10 ms delay
#define DISPLAY_INIT_WAIT_5_MS          1    // >900 us delay
#define DISPLAY_INIT_WAIT_6_MS          3    // >2 ms delay

#define DISPLAY_CMD_WAIT_US             220  // 110 us Wait time for every command 45 us, except:
#define DISPLAY_CMD_CLEAR_WAIT_MS       5    // - 2 ms Clear Display 1.52 ms
#define DISPLAY_CMD_HOME_WAIT_MS        3    // - 2 ms Return Home  1.52 ms
#define DISPLAY_POSITION_WAIT_US        200  // - 100 us

#define DISPLAY_EN_PULSE_HIGH_WAIT_US   50   // 25 us
#define DISPLAY_EN_PULSE_LOW_WAIT_US    50   // 25 us

//=====[Declaration of private data types]=====================================

typedef enum {
    DISPLAY_CMD_CLEAR           = 0x01,
    DISPLAY_CMD_HOME            = 0x02,
    DISPLAY_CMD_ENTRY_MODE_SET  = 0x04,
    DISPLAY_CMD_DISPLAY_CONTROL = 0x08,
    DISPLAY_CMD_CURSOR_SHIFT    = 0x10,
    DISPLAY_CMD_FUNCTION_SET    = 0x20,
    DISPLAY_CMD_SET_CGRAM_ADDR  = 0x40,
    DISPLAY_CMD_SET_DDRAM_ADDR  = 0x80,
} displayCommand_t;

// Display Commands Flags for display entry mode
typedef enum {
    DISPLAY_CMDF_ENTRY_RIGHT     = 0x00,
    DISPLAY_CMDF_ENTRY_LEFT      = 0x02,
    DISPLAY_CMDF_SHIFT_INCREMENT = 0x01,
    DISPLAY_CMDF_SHIFT_DECREMENT = 0x00,
} displayCmdEntry_t;

// Display Commands Flags for display on/off control
typedef enum {
    DISPLAY_CMDF_DISPLAY_ON  = 0x04,
    DISPLAY_CMDF_DISPLAY_OFF = 0x00,
    DISPLAY_CMDF_CURSOR_ON   = 0x02,
    DISPLAY_CMDF_CURSOR_OFF  = 0x00,
    DISPLAY_CMDF_BLINK_ON    = 0x01,
    DISPLAY_CMDF_BLINK_OFF   = 0x00,
} displayCmdDisplayCtrl_t;

// Display Commands Flags for display/cursor shift
typedef enum {
    DISPLAY_CMDF_DISPLAYMOVE = 0x08,
    DISPLAY_CMDF_CURSORMOVE  = 0x00,
    DISPLAY_CMDF_MOVERIGHT   = 0x04,
    DISPLAY_CMDF_MOVELEFT    = 0x00,
} displayCmdDisplayCursor_t;

// Display Commands Flags for function set
typedef enum {
    // LCD and GLCD
    DISPLAY_CMDF_8_BIT_MODE              = 0x10, // DL=1: 8-BIT interface
    DISPLAY_CMDF_4_BIT_MODE              = 0x00, // DL=0: 4-BIT interface
    // LCD only
    DISPLAY_CMDF_2_LINE                  = 0x08, // N=1: 2 lines
    DISPLAY_CMDF_1_LINE                  = 0x00, // N=0: 1 line
    DISPLAY_CMDF_5x10_DOTS               = 0x04, // F=1: 5x10 dots
    DISPLAY_CMDF_5x8_DOTS                = 0x00, // F=0: 5x8 dots
    // GLCD only
    DISPLAY_CMDF_EXTENDED_INSTRUCION_SET = 0x04, // RE=1: extended instruction set
    DISPLAY_CMDF_BASIC_INSTRUCION_SET    = 0x00, // RE=0: basic instruction set
    DISPLAY_CMDF_GRAPHIC_DISPLAY_ON      = 0x02, // G=1: graphic display ON
    DISPLAY_CMDF_GRAPHIC_DISPLAY_OFF     = 0x00, // G=0: graphic display OFF
} displayCmdFunctionSet_t;

// Display pins
typedef enum {
 // DISPLAY_VSS = 1,  // Power source - (GND).
 // DISPLAY_VDD = 2,  // Power source + (5V or 3.3V CC).
 // DISPLAY_VO  = 3,  // Contrast (potentiometer or PWM dimmer).
    DISPLAY_RS  = 4,  // Register select bit.
    DISPLAY_RW  = 5,  // Read/Write bit.
    DISPLAY_EN  = 6,  // Enable bit.
    DISPLAY_D0  = 7,  // Data bit 0.
    DISPLAY_D1  = 8,  // Data bit 1.
    DISPLAY_D2  = 9,  // Data bit 2.
    DISPLAY_D3  = 10, // Data bit 3.
    DISPLAY_D4  = 11, // Data bit 4.
    DISPLAY_D5  = 12, // Data bit 5.
    DISPLAY_D6  = 13, // Data bit 4.
    DISPLAY_D7  = 14, // Data bit 7.
    DISPLAY_A   = 15, // Backlight anode.
 // DISPLAY_K   = 16, // Backlight cathode (GND).
} displayPins_t;

// PCF8574 connected to display pins
typedef enum {
    DISPLAY_RS_PCF8574 = 0, // Register select bit.
    DISPLAY_RW_PCF8574 = 1, // Read/Write bit.
    DISPLAY_EN_PCF8574 = 2, // Enable bit.
    DISPLAY_A_PCF8574  = 3, // Backlight anode.
    DISPLAY_D4_PCF8574 = 4, // Data bit 4.
    DISPLAY_D5_PCF8574 = 5, // Data bit 5.
    DISPLAY_D6_PCF8574 = 6, // Data bit 4.
    DISPLAY_D7_PCF8574 = 7, // Data bit 7.
} displayPcf8574Pins_t;

//=====[Declaration and initialization of public global objects]===============

// Configure display pins connected to GPIOs as Outputs

DigitalOut displayD0( D0 );
DigitalOut displayD1( D1 );
DigitalOut displayD2( D2 );
DigitalOut displayD3( D3 );

DigitalOut displayD4( D4 );
DigitalOut displayD5( D5 );
DigitalOut displayD6( D6 );
DigitalOut displayD7( D7 );

DigitalOut displayRS( D9 );
DigitalOut displayEN( D8 );


DigitalOut spiSs( SPI_CS );
SPI spi(SPI_MOSI, SPI_MISO, SPI_SCK);

//=====[Declaration of external public global variables]=======================

//=====[Declaration and initialization of public global variables]=============

//=====[Declaration and initialization of private global variables]============

// Display scruture
static display_t display;

// GLCD Bitmap buffer
static uint8_t image[ (128 * 64) / 8 ];

//=====[Declarations (prototypes) of private functions]========================

static void displayDelay_us( int microseconds );
static void displayDelay_ms( int miliseconds );

static void displayEnablePulse();

static void displayPinWrite( uint8_t pinName, uint8_t pinValue );
static void displayPortWrite( uint8_t dataByte );
static void displayPortNibbleWrite( uint8_t nibble );

static void displayCommandWrite( uint8_t command );
static void displayDataWrite( uint8_t data );

//=====[Implementations of public functions]===================================

void displayInit( displayType_t type, displayConnection_t connection )
{
    display.type = type;
    display.connection = connection;
    display.mode = DISPLAY_MODE_CHAR;

    switch( display.connection ) {
        case DISPLAY_CONNECTION_I2C_PCF8574_IO_EXPANDER:
            pcf8574tInit( 1, 1, 1 );
        case DISPLAY_CONNECTION_GPIO_8BITS:
            // Set RS, RW, EN and D0 to D7 pins as outputs
        case DISPLAY_CONNECTION_GPIO_4BITS:
            // Set RS, RW, EN and D4 to D7 pins as outputs
            displayPinWrite( DISPLAY_RS, OFF );
            displayPinWrite( DISPLAY_RW, OFF );
            displayPinWrite( DISPLAY_EN, OFF );
            displayPinWrite( DISPLAY_A,  ON  );
        break;
        case DISPLAY_CONNECTION_SPI:
            spi.format( 8, 3 );
            spi.frequency( 1000000 );/*
            spiReset = OFF;           // RESET=0
            delay( 10 );              // wait for 10ms
            spiReset = ON;            // RESET=1*/
            displayDelay_ms( 50 );    // wait for >40 ms
        break;
        default:
        break;
    }

    // Set a display power-up time depending on display vendor
	displayDelay_ms( DISPLAY_POWERUP_WAIT_MS );

    switch( display.connection ) {

        case DISPLAY_CONNECTION_GPIO_8BITS:
        case DISPLAY_CONNECTION_SPI:
            // Sets interface data length (DL), number of display lines (N), 
            // and character font (F). 
            // Some displays need to repeat 2 times this command
            displayCommandWrite( DISPLAY_CMD_FUNCTION_SET | 
                                 DISPLAY_CMDF_8_BIT_MODE | 
                                 DISPLAY_CMDF_2_LINE |
                                 DISPLAY_CMDF_5x8_DOTS );
            displayDelay_us( DISPLAY_INIT_WAIT_1_US );           //  >100us delay

            // Sets interface data length (DL), number of display lines (N), 
            // and character font (F). 
            // Some displays need to repeat 2 times this command
            displayCommandWrite( DISPLAY_CMD_FUNCTION_SET | 
                                 DISPLAY_CMDF_4_BIT_MODE | 
                                 DISPLAY_CMDF_2_LINE |
                                 DISPLAY_CMDF_5x8_DOTS );
            displayDelay_us( DISPLAY_INIT_WAIT_2_US );           // >37us delay
        break;

        case DISPLAY_CONNECTION_GPIO_4BITS:
        case DISPLAY_CONNECTION_I2C_PCF8574_IO_EXPANDER:
            // Sets interface data length (DL), number of display lines (N), 
            // and character font (F). 
            // Some displays need to repeat 2 times this command
            displayCommandWrite( DISPLAY_CMD_FUNCTION_SET | 
                                 DISPLAY_CMDF_4_BIT_MODE | 
                                 DISPLAY_CMDF_2_LINE |
                                 DISPLAY_CMDF_5x8_DOTS );
            displayDelay_us( DISPLAY_INIT_WAIT_1_US );           //  >100us delay

            // Sets interface data length (DL), number of display lines (N), 
            // and character font (F). 
            // Some displays need to repeat 2 times this command
            displayCommandWrite( DISPLAY_CMD_FUNCTION_SET | 
                                 DISPLAY_CMDF_4_BIT_MODE | 
                                 DISPLAY_CMDF_2_LINE |
                                 DISPLAY_CMDF_5x8_DOTS );
            displayDelay_us( DISPLAY_INIT_WAIT_2_US );           // >37us delay
        break;

        default:
        break;
    }

    // Sets entire display (D) on/off, cursor on/off (C), and
    // blinking of cursor position character (B).
    displayCommandWrite( DISPLAY_CMD_DISPLAY_CONTROL |
                         DISPLAY_CMDF_DISPLAY_OFF |      // ON or OFF
                         DISPLAY_CMDF_CURSOR_OFF |       // ON or OFF
                         DISPLAY_CMDF_BLINK_OFF );       // ON or OFF
	displayDelay_ms( DISPLAY_INIT_WAIT_3_MS );           // >100 ms delay

	displayClear();
	displayDelay_ms(DISPLAY_INIT_WAIT_4_MS);             // >10 ms delay

    // Sets cursor move direction and specifies display shift.
    // These operations are performed during data write and read.
	displayCommandWrite( DISPLAY_CMD_ENTRY_MODE_SET |
                         DISPLAY_CMDF_ENTRY_LEFT |       // LEFT or RIGHT
                         DISPLAY_CMDF_SHIFT_DECREMENT ); // ICREMENT or DECREMENT
	displayDelay_ms( DISPLAY_INIT_WAIT_5_MS );           // >900 us delay

    // Sets entire display (D) on/off, cursor on/off (C), and
    // blinking of cursor position character (B).
    displayCommandWrite( DISPLAY_CMD_DISPLAY_CONTROL |
                         DISPLAY_CMDF_DISPLAY_ON |       // ON or OFF
                         DISPLAY_CMDF_CURSOR_OFF |       // ON or OFF
                         DISPLAY_CMDF_BLINK_OFF );       // ON or OFF
    displayDelay_ms( DISPLAY_INIT_WAIT_6_MS );           // >2 ms delay

	displayHome();
}

void displayCharPositionWrite( uint8_t charPositionX, 
                               uint8_t charPositionY )
{
    // Start in (x,y) = (0,0)
    static uint8_t firstCharAdress[4];
    
    char divFactor;

    if( display.type == DISPLAY_TYPE_LCD_HD44780 ) {
        // Char LCD display positions
        firstCharAdress[0] = DISPLAY_CMD_SET_DDRAM_ADDR;               // 0x80
        firstCharAdress[1] = DISPLAY_CMD_SET_DDRAM_ADDR + 0x40;        // 0xC0 
        firstCharAdress[2] = DISPLAY_CMD_SET_DDRAM_ADDR + 0x14;        // 0x94
        firstCharAdress[3] = DISPLAY_CMD_SET_DDRAM_ADDR + 0x40 + 0x14; // 0xD4
        divFactor = 1;
    } else if( display.type == DISPLAY_TYPE_GLCD_ST7920 ) {
        // GLCD display positions
        firstCharAdress[0] = DISPLAY_CMD_SET_DDRAM_ADDR;        // 0x80
        firstCharAdress[1] = DISPLAY_CMD_SET_DDRAM_ADDR + 0x10; // 0x90 
        firstCharAdress[2] = DISPLAY_CMD_SET_DDRAM_ADDR + 0x08; // 0x88
        firstCharAdress[3] = DISPLAY_CMD_SET_DDRAM_ADDR + 0x18; // 0x98
        divFactor = 2;
    }

    displayCommandWrite( firstCharAdress[charPositionY] + charPositionX/devFactor );
    displayDelay_us( DISPLAY_POSITION_WAIT_US );
}

void displayStringWrite( char const * str )
{
    while (*str) {
        displayDataWrite(*str++);
    }
}

void displayClear( void )
{
    // Clears entire display and sets DDRAM address 0 in address counter.
    displayCommandWrite( DISPLAY_CMD_CLEAR );
    displayDelay_ms( DISPLAY_CMD_CLEAR_WAIT_MS );
}

void displayHome( void )
{
    // Sets DDRAM address 0 in address counter. Also returns display from 
    // being shifted to original position. DDRAM contents remain unchanged
    displayCommandWrite( DISPLAY_CMD_HOME );
	displayDelay_ms( DISPLAY_CMD_HOME_WAIT_MS );
}

void displayClearAndHome( void )
{
   displayClear();
   displayCharPositionWrite( 0, 0 ); // displayHome();
}

// GLCD

void displayModeWrite( displayMode_t mode )
{
    if ( mode == DISPLAY_MODE_GRAPHIC )
	{
		displayCommandWrite( DISPLAY_CMD_FUNCTION_SET | 
                             DISPLAY_CMDF_8_BIT_MODE );  // 8 bit mode
		delay(1);
		displayCommandWrite( DISPLAY_CMD_FUNCTION_SET | 
                             DISPLAY_CMDF_8_BIT_MODE | 
                             DISPLAY_CMDF_EXTENDED_INSTRUCION_SET );  // switch to Extended instructions
		delay(1);
		displayCommandWrite( DISPLAY_CMD_FUNCTION_SET | 
                             DISPLAY_CMDF_8_BIT_MODE |
                             DISPLAY_CMDF_EXTENDED_INSTRUCION_SET |
                             DISPLAY_CMDF_GRAPHIC_DISPLAY_ON );  // enable graphics
		delay(1);
	} else if ( mode == DISPLAY_MODE_CHAR ) {
		displayCommandWrite( DISPLAY_CMD_FUNCTION_SET | 
                             DISPLAY_CMDF_8_BIT_MODE );  // 8 bit mode
		delay(1);
	}
}

void displayBitmapWrite( uint8_t* bitmap )
{
    uint8_t x, y;
	for( y=0; y<64; y++ ) {
		if ( y < 32 ) {
			for( x = 0; x < 8; x++ ) {				                   // Draws top half of the screen.
												                       // In extended instruction mode, vertical and horizontal coordinates must be specified before sending data in.
				displayCommandWrite( DISPLAY_CMD_SET_DDRAM_ADDR | y ); // Vertical coordinate of the screen is specified first. (0-31)
				displayCommandWrite( DISPLAY_CMD_SET_DDRAM_ADDR | x) ; // Then horizontal coordinate of the screen is specified. (0-8)
				displayDataWrite( bitmap[2*x + 16*y] );                // Data to the upper byte is sent to the coordinate.
				displayDataWrite( bitmap[2*x+1 + 16*y] );              // Data to the lower byte is sent to the coordinate.
			}
		} else {
			for( x = 0; x < 8; x++ ) {					                    // Draws bottom half of the screen.
													                    	// Actions performed as same as the upper half screen.
				displayCommandWrite( DISPLAY_CMD_SET_DDRAM_ADDR | (y-32) );	// Vertical coordinate must be scaled back to 0-31 as it is dealing with another half of the screen.
				displayCommandWrite( DISPLAY_CMD_SET_DDRAM_ADDR | 0x08 | x );
				displayDataWrite( bitmap[2*x + 16*y] );
				displayDataWrite( bitmap[2*x+1 + 16*y] );
			}
		}
	}
}

//=====[Implementations of private functions]==================================

static void displayDelay_us( int microseconds )
{
   wait_us( microseconds ); // Mbed
}

static void displayDelay_ms( int miliseconds )
{
   delay( miliseconds ); // sAPI
   //wait_ms( miliseconds ); // Mbed, pero tira warning
}

static void displayEnablePulse()
{
   displayPinWrite( DISPLAY_EN, ON );                // EN = 1 for H-to-L pulse
   displayDelay_us( DISPLAY_EN_PULSE_HIGH_WAIT_US ); // Wait to make EN wider
   displayPinWrite( DISPLAY_EN, OFF );               // EN = 0 for H-to-L pulse
}

static void displayPinWrite( uint8_t pinName, uint8_t pinValue )
{
    switch( display.connection ) {
        case DISPLAY_CONNECTION_GPIO_8BITS:
        case DISPLAY_CONNECTION_GPIO_4BITS:
            switch( pinName ) {
                case DISPLAY_RS:   displayRS = pinValue;   break;
                case DISPLAY_RW:   break; // Connected to GND in GPIO connection because 5V pin
                case DISPLAY_EN:   displayEN = pinValue;   break;
                case DISPLAY_D0:   displayD0 = pinValue;   break;
                case DISPLAY_D1:   displayD1 = pinValue;   break;
                case DISPLAY_D2:   displayD2 = pinValue;   break;
                case DISPLAY_D3:   displayD3 = pinValue;   break;
                case DISPLAY_D4:   displayD4 = pinValue;   break;
                case DISPLAY_D5:   displayD5 = pinValue;   break;
                case DISPLAY_D6:   displayD6 = pinValue;   break;
                case DISPLAY_D7:   displayD7 = pinValue;   break;
                default: break;
            }
        break;

        case DISPLAY_CONNECTION_I2C_PCF8574_IO_EXPANDER:
            // Acomodo el numero de pines para el expander I2C
            if( pinName >= DISPLAY_RS && pinName <= DISPLAY_EN ) {
                pinName = pinName - DISPLAY_RS;
            } else if( pinName == DISPLAY_A ) {
                pinName = DISPLAY_A_PCF8574;
            } else if( pinName >= DISPLAY_D4 && pinName <= DISPLAY_D7 ) {
                pinName = pinName - DISPLAY_D4 + DISPLAY_D4_PCF8574;
            }
            pcf8574tGpioWrite( pinName, pinValue );
        break;

        default:
        break;
    }
}

static void displayPortWrite( uint8_t dataByte )
{
    displayPinWrite( DISPLAY_D0, dataByte & 0x01 );
    displayPinWrite( DISPLAY_D1, dataByte & 0x02 );
    displayPinWrite( DISPLAY_D2, dataByte & 0x04 );
    displayPinWrite( DISPLAY_D3, dataByte & 0x08 );
    displayPinWrite( DISPLAY_D4, dataByte & 0x10 );
    displayPinWrite( DISPLAY_D5, dataByte & 0x20 );
    displayPinWrite( DISPLAY_D6, dataByte & 0x40 );
    displayPinWrite( DISPLAY_D7, dataByte & 0x80 );
}

static void displayPortNibbleWrite( uint8_t nibble )
{
    displayPinWrite( DISPLAY_D4, nibble & 0x10 );
    displayPinWrite( DISPLAY_D5, nibble & 0x20 );
    displayPinWrite( DISPLAY_D6, nibble & 0x40 );
    displayPinWrite( DISPLAY_D7, nibble & 0x80 );
}

static void displayCommandWrite( uint8_t command )
{
    switch( display.connection ) {

        case DISPLAY_CONNECTION_GPIO_8BITS:
            displayPortWrite( command );                     // Send command byte to D0-D7
            displayPinWrite( DISPLAY_RS, OFF );              // RS = 0 for command
            displayPinWrite( DISPLAY_RW, OFF );              // RW = 0 for write
            displayEnablePulse();
            displayDelay_us( DISPLAY_EN_PULSE_LOW_WAIT_US ); // Wait
        break;

        case DISPLAY_CONNECTION_GPIO_4BITS:
        case DISPLAY_CONNECTION_I2C_PCF8574_IO_EXPANDER:
            displayPortNibbleWrite( command & 0xF0 );        // Send high nibble to D4-D7
            displayPinWrite( DISPLAY_RS, OFF );              // RS = 0 for command
            displayPinWrite( DISPLAY_RW, OFF );              // RW = 0 for write
            displayEnablePulse();
            displayDelay_us( DISPLAY_EN_PULSE_LOW_WAIT_US ); // Wait
            displayPortNibbleWrite( command << 4 );          // Send low nibble to D4-D7
            displayEnablePulse();
        break;

        case DISPLAY_CONNECTION_SPI:
            spi.lock();
            spiSs = ON;
            spi.write( 0xF8 + (0<<1) );       // send the SYNC + RS(0)
            spi.write( command & 0xF0 );      // send the higher nibble first
            spi.write( (command<<4) & 0xF0 ); // send the lower nibble
            spiSs = OFF;
            spi.unlock();
        break;

        default:
        break;
    }
}

static void displayDataWrite( uint8_t data )
{

    switch( display.connection ) {

        case DISPLAY_CONNECTION_GPIO_8BITS:
            displayPortWrite( data );                // Send data byte to D4-D7
            displayPinWrite( DISPLAY_RS, ON );       // RS = 1 for data
            displayPinWrite( DISPLAY_RW, OFF );      // RW = 0 for write
            displayEnablePulse();
        break;

        case DISPLAY_CONNECTION_GPIO_4BITS:
        case DISPLAY_CONNECTION_I2C_PCF8574_IO_EXPANDER:
            displayPortNibbleWrite( data & 0xF0 );   // Send high nibble to D4-D7
            displayPinWrite( DISPLAY_RS, ON );       // RS = 1 for data
            displayPinWrite( DISPLAY_RW, OFF );      // RW = 0 for write
            displayEnablePulse();
            displayPortNibbleWrite( data << 4 );     // Send low nibble to D4-D7
            displayEnablePulse();
        break;

        case DISPLAY_CONNECTION_SPI:
            spi.lock();
            spiSs = ON;
            spi.write( 0xF8 + (1<<1) );    // send the SYNC + RS(1)
            spi.write( data & 0xF0 );      // send the higher nibble first
            spi.write( (data<<4) & 0xF0 ); // send the lower nibble
            spiSs = OFF;
            spi.unlock();
        break;

        default:
        break;
    }
}