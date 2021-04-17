#include "mbed.h"
#include "arm_book_lib.h"
#include "display.h"

//=====[Declaration of private defines]========================================

#define DISPLAY_IR_CLEAR_DISPLAY   0b00000001
#define DISPLAY_IR_ENTRY_MODE_SET  0b00000100
#define DISPLAY_IR_DISPLAY_CONTROL 0b00001000
#define DISPLAY_IR_FUNCTION_SET    0b00100000
#define DISPLAY_IR_SET_DDRAM_ADDR  0b10000000

#define DISPLAY_IR_ENTRY_MODE_SET_INCREMENT 0b00000010
#define DISPLAY_IR_ENTRY_MODE_SET_DECREMENT 0b00000000
#define DISPLAY_IR_ENTRY_MODE_SET_SHIFT     0b00000001
#define DISPLAY_IR_ENTRY_MODE_SET_NO_SHIFT  0b00000000

#define DISPLAY_IR_DISPLAY_CONTROL_DISPLAY_ON  0b00000100
#define DISPLAY_IR_DISPLAY_CONTROL_DISPLAY_OFF 0b00000000
#define DISPLAY_IR_DISPLAY_CONTROL_CURSOR_ON   0b00000010
#define DISPLAY_IR_DISPLAY_CONTROL_CURSOR_OFF  0b00000000
#define DISPLAY_IR_DISPLAY_CONTROL_BLINK_ON    0b00000001
#define DISPLAY_IR_DISPLAY_CONTROL_BLINK_OFF   0b00000000

#define DISPLAY_IR_FUNCTION_SET_8BITS    0b00010000
#define DISPLAY_IR_FUNCTION_SET_4BITS    0b00000000
#define DISPLAY_IR_FUNCTION_SET_2LINES   0b00001000
#define DISPLAY_IR_FUNCTION_SET_1LINE    0b00000000
#define DISPLAY_IR_FUNCTION_SET_5x10DOTS 0b00000100
#define DISPLAY_IR_FUNCTION_SET_5x8DOTS  0b00000000

#define DISPLAY_20x4_LINE1_FIRST_CHARACTER_ADDRESS 0
#define DISPLAY_20x4_LINE2_FIRST_CHARACTER_ADDRESS 64
#define DISPLAY_20x4_LINE3_FIRST_CHARACTER_ADDRESS 20
#define DISPLAY_20x4_LINE4_FIRST_CHARACTER_ADDRESS 84

#define DISPLAY_RS_INSTRUCTION 0
#define DISPLAY_RS_DATA        1

#define DISPLAY_RW_WRITE 0
#define DISPLAY_RW_READ  1

#define DISPLAY_PIN_RS  4
#define DISPLAY_PIN_RW  5
#define DISPLAY_PIN_EN  6
#define DISPLAY_PIN_D0  7  
#define DISPLAY_PIN_D1  8  
#define DISPLAY_PIN_D2  9  
#define DISPLAY_PIN_D3 10
#define DISPLAY_PIN_D4 11
#define DISPLAY_PIN_D5 12 
#define DISPLAY_PIN_D6 13 
#define DISPLAY_PIN_D7 14 

#define DISPLAY_PIN_A_PCF8574 3

#define I2C1_SDA PB_9
#define I2C1_SCL PB_8

#define PCF8574_I2C_BUS_8BIT_WRITE_ADDRESS 78

#define SPI1_MOSI PA_7
#define SPI1_SCK  PA_5
#define SPI1_CS   PA_6
#define SPI1_MISO PA_6

#define DISPLAY_IR_SET_GDRAM_ADDR  0b10000000

#define DISPLAY_IR_FUNCTION_SET_EXTENDED_INSTRUCION_SET 0b00000100
#define DISPLAY_IR_FUNCTION_SET_BASIC_INSTRUCION_SET    0b00000000
#define DISPLAY_IR_FUNCTION_SET_GRAPHIC_DISPLAY_ON      0b00000010
#define DISPLAY_IR_FUNCTION_SET_GRAPHIC_DISPLAY_OFF     0b00000000

#define DISPLAY_ST7920_LINE1_FIRST_CHARACTER_ADDRESS 0
#define DISPLAY_ST7920_LINE2_FIRST_CHARACTER_ADDRESS 16
#define DISPLAY_ST7920_LINE3_FIRST_CHARACTER_ADDRESS 8
#define DISPLAY_ST7920_LINE4_FIRST_CHARACTER_ADDRESS 24

#define ST9720_SPI_SYNCHRONIZING_BIT_STRING 0b11111000

#define ST9720_SPI_RS_INSTRUCTION 0b000000000
#define ST9720_SPI_RS_DATA        0b000000010

#define ST9720_SPI_RW_WRITE 0b000000000
#define ST9720_SPI_RW_READ  0b000000100

//=====[Declaration of private data types]=====================================

typedef struct{
    int address;
    char data;
    bool displayPin_RS;
    bool displayPin_RW; 
    bool displayPin_EN;
    bool displayPin_A;
    bool displayPin_D4;
    bool displayPin_D5;
    bool displayPin_D6;
    bool displayPin_D7;
} pcf8574_t;

//=====[Declaration and initialization of public global objects]===============

DigitalOut displayD0( D0 );
DigitalOut displayD1( D1 );
DigitalOut displayD2( D2 );
DigitalOut displayD3( D3 );
DigitalOut displayD4( D4 );
DigitalOut displayD5( D5 );
DigitalOut displayD6( D6 );
DigitalOut displayD7( D7 );
DigitalOut displayRS( D8 );
DigitalOut displayEN( D9 );

I2C I2C_PCF8574( I2C1_SDA, I2C1_SCL ); 

DigitalOut SPI_ST9720_Chip_Select(SPI1_CS);
SPI SPI_ST9720(SPI1_MOSI, SPI1_MISO, SPI1_SCK);

//=====[Declaration of external public global variables]=======================

//=====[Declaration and initialization of public global variables]=============

//=====[Declaration and initialization of private global variables]============

static display_t display;
static pcf8574_t pcf8574;
static bool initial8BitCommunicationIsCompleted;

static uint8_t image[ (128 * 64) / 8 ];

//=====[Declarations (prototypes) of private functions]========================

static void displayPinWrite( uint8_t pinName, int value );
static void displayDataBusWrite( uint8_t dataByte );
static void displayCodeWrite( bool type, uint8_t dataBus );

//=====[Implementations of public functions]===================================

void displayInit( displayType_t type, displayConnection_t connection )
{
    display.type = type;
    display.connection = connection;
    display.mode = DISPLAY_MODE_CHAR;
	
	if( display.connection == DISPLAY_CONNECTION_I2C_PCF8574_IO_EXPANDER) {
		pcf8574.address = PCF8574_I2C_BUS_8BIT_WRITE_ADDRESS;
		pcf8574.data = 0b00000000;
		I2C_PCF8574.frequency(100000);
        displayPinWrite( DISPLAY_PIN_A_PCF8574,  ON );
    }

	if( display.connection == DISPLAY_CONNECTION_SPI) {
        SPI_ST9720.format( 8, 3 );
        SPI_ST9720.frequency( 1000000 );
    }    
	
	initial8BitCommunicationIsCompleted = FALSE;    

    delay( 50 );
    
    displayCodeWrite( DISPLAY_RS_INSTRUCTION, 
                      DISPLAY_IR_FUNCTION_SET | 
                      DISPLAY_IR_FUNCTION_SET_8BITS );
    delay( 5 );
            
    displayCodeWrite( DISPLAY_RS_INSTRUCTION, 
                      DISPLAY_IR_FUNCTION_SET | 
                      DISPLAY_IR_FUNCTION_SET_8BITS );
    delay( 1 ); 

    displayCodeWrite( DISPLAY_RS_INSTRUCTION, 
                      DISPLAY_IR_FUNCTION_SET | 
                      DISPLAY_IR_FUNCTION_SET_8BITS );
    delay( 1 );  

    switch( display.connection ) {
        case DISPLAY_CONNECTION_GPIO_8BITS:
        case DISPLAY_CONNECTION_SPI:
            displayCodeWrite( DISPLAY_RS_INSTRUCTION, 
                              DISPLAY_IR_FUNCTION_SET | 
                              DISPLAY_IR_FUNCTION_SET_8BITS | 
                              DISPLAY_IR_FUNCTION_SET_2LINES |
                              DISPLAY_IR_FUNCTION_SET_5x8DOTS );
            delay( 1 );         
        break;
        
        case DISPLAY_CONNECTION_GPIO_4BITS:
        case DISPLAY_CONNECTION_I2C_PCF8574_IO_EXPANDER:
            displayCodeWrite( DISPLAY_RS_INSTRUCTION, 
                              DISPLAY_IR_FUNCTION_SET | 
                              DISPLAY_IR_FUNCTION_SET_4BITS );
            delay( 1 );  

            initial8BitCommunicationIsCompleted = TRUE;  

            displayCodeWrite( DISPLAY_RS_INSTRUCTION, 
                              DISPLAY_IR_FUNCTION_SET | 
                              DISPLAY_IR_FUNCTION_SET_4BITS | 
                              DISPLAY_IR_FUNCTION_SET_2LINES |
                              DISPLAY_IR_FUNCTION_SET_5x8DOTS );
            delay( 1 );                                      
        break;
    }

    displayCodeWrite( DISPLAY_RS_INSTRUCTION, 
                      DISPLAY_IR_DISPLAY_CONTROL |
                      DISPLAY_IR_DISPLAY_CONTROL_DISPLAY_OFF |      
                      DISPLAY_IR_DISPLAY_CONTROL_CURSOR_OFF |       
                      DISPLAY_IR_DISPLAY_CONTROL_BLINK_OFF );       
    delay( 1 );          

    displayCodeWrite( DISPLAY_RS_INSTRUCTION, 
                      DISPLAY_IR_CLEAR_DISPLAY );       
    delay( 1 ); 

	displayCodeWrite( DISPLAY_RS_INSTRUCTION, 
	                  DISPLAY_IR_ENTRY_MODE_SET |
	                  DISPLAY_IR_ENTRY_MODE_SET_INCREMENT |       
	                  DISPLAY_IR_ENTRY_MODE_SET_NO_SHIFT );                  
    delay( 1 );           

    displayCodeWrite( DISPLAY_RS_INSTRUCTION, 
                      DISPLAY_IR_DISPLAY_CONTROL |
	                  DISPLAY_IR_DISPLAY_CONTROL_DISPLAY_ON |      
	                  DISPLAY_IR_DISPLAY_CONTROL_CURSOR_OFF |    
	                  DISPLAY_IR_DISPLAY_CONTROL_BLINK_OFF );    
    delay( 1 );  
}

void displayCharPositionWrite( uint8_t charPositionX, uint8_t charPositionY )
{    
    if( display.type == DISPLAY_TYPE_LCD_HD44780 ) {
        switch( charPositionY ) {
            case 0:
                displayCodeWrite( DISPLAY_RS_INSTRUCTION, 
                                  DISPLAY_IR_SET_DDRAM_ADDR |
                                  ( DISPLAY_20x4_LINE1_FIRST_CHARACTER_ADDRESS +
                                    charPositionX ) );
                delay( 1 );         
            break;
        
            case 1:
                displayCodeWrite( DISPLAY_RS_INSTRUCTION, 
                                  DISPLAY_IR_SET_DDRAM_ADDR |
                                  ( DISPLAY_20x4_LINE2_FIRST_CHARACTER_ADDRESS +
                                    charPositionX ) );
                delay( 1 );         
            break;
        
            case 2:
                displayCodeWrite( DISPLAY_RS_INSTRUCTION, 
                                  DISPLAY_IR_SET_DDRAM_ADDR |
                                  ( DISPLAY_20x4_LINE3_FIRST_CHARACTER_ADDRESS +
                                    charPositionX ) );
                delay( 1 );         
            break;

            case 3:
                displayCodeWrite( DISPLAY_RS_INSTRUCTION, 
                                  DISPLAY_IR_SET_DDRAM_ADDR |
                                  ( DISPLAY_20x4_LINE4_FIRST_CHARACTER_ADDRESS +
                                    charPositionX ) );
                delay( 1 );         
            break;
         }
    } else if( display.type == DISPLAY_TYPE_GLCD_ST7920 ) {         
        switch( charPositionY ) {
            case 0:
                displayCodeWrite( DISPLAY_RS_INSTRUCTION, 
                                  DISPLAY_IR_SET_DDRAM_ADDR |
                                  ( DISPLAY_ST7920_LINE1_FIRST_CHARACTER_ADDRESS +
                                    charPositionX/2 ) );
                delay( 1 );         
            break;
        
            case 1:
                displayCodeWrite( DISPLAY_RS_INSTRUCTION, 
                                  DISPLAY_IR_SET_DDRAM_ADDR |
                                  ( DISPLAY_ST7920_LINE2_FIRST_CHARACTER_ADDRESS +
                                    charPositionX/2 ) );
                delay( 1 );         
            break;
        
            case 2:
                displayCodeWrite( DISPLAY_RS_INSTRUCTION, 
                                  DISPLAY_IR_SET_DDRAM_ADDR |
                                  ( DISPLAY_ST7920_LINE3_FIRST_CHARACTER_ADDRESS +
                                    charPositionX/2 ) );
                delay( 1 );         
            break;

            case 3:
                displayCodeWrite( DISPLAY_RS_INSTRUCTION, 
                                  DISPLAY_IR_SET_DDRAM_ADDR |
                                  ( DISPLAY_ST7920_LINE4_FIRST_CHARACTER_ADDRESS +
                                    charPositionX/2 ) );
                delay( 1 );         
            break;
         }
    }
}

void displayStringWrite( char const * str )
{
    while (*str) {
        displayCodeWrite(DISPLAY_RS_DATA, *str++);
    }
}

void displayClear( void )
{
    displayCodeWrite( DISPLAY_RS_INSTRUCTION, 
                      DISPLAY_IR_CLEAR_DISPLAY );
    delay( 2 ); 
}

void displayModeWrite( displayMode_t mode )
{
    if ( mode == DISPLAY_MODE_GRAPHIC )
	{
		displayCodeWrite( DISPLAY_RS_INSTRUCTION, 
		                  DISPLAY_IR_FUNCTION_SET  | 
                          DISPLAY_IR_FUNCTION_SET_8BITS |
                          DISPLAY_IR_FUNCTION_SET_EXTENDED_INSTRUCION_SET );
		delay(1);
		displayCodeWrite( DISPLAY_RS_INSTRUCTION, 
		                  DISPLAY_IR_FUNCTION_SET  | 
                          DISPLAY_IR_FUNCTION_SET_8BITS |
                          DISPLAY_IR_FUNCTION_SET_EXTENDED_INSTRUCION_SET |
                          DISPLAY_IR_FUNCTION_SET_GRAPHIC_DISPLAY_ON  );
		delay(1);
	} else if ( mode == DISPLAY_MODE_CHAR ) {
		displayCodeWrite( DISPLAY_RS_INSTRUCTION, 
		                  DISPLAY_IR_FUNCTION_SET | 
                          DISPLAY_IR_FUNCTION_SET_8BITS |
                          DISPLAY_IR_FUNCTION_SET_BASIC_INSTRUCION_SET |
                          DISPLAY_IR_FUNCTION_SET_GRAPHIC_DISPLAY_OFF);
		delay(1);
	}
}

void displayBitmapWrite( uint8_t* bitmap )
{
    uint8_t x, y;
	for( y=0; y<64; y++ ) {
		if ( y < 32 ) {
			for( x = 0; x < 8; x++ ) {				                   
                displayCodeWrite( DISPLAY_RS_INSTRUCTION,
                                  DISPLAY_IR_SET_GDRAM_ADDR | 
                                  y );
                displayCodeWrite( DISPLAY_RS_INSTRUCTION,
                                  DISPLAY_IR_SET_GDRAM_ADDR | 
                                  x );
                displayCodeWrite(DISPLAY_RS_DATA, 
                                 bitmap[2*x + 16*y] );
                displayCodeWrite(DISPLAY_RS_DATA, 
                                 bitmap[2*x+1 + 16*y] );                                 
                }
		} else {
			for( x = 0; x < 8; x++ ) {					                    
                displayCodeWrite( DISPLAY_RS_INSTRUCTION,
                                  DISPLAY_IR_SET_GDRAM_ADDR | 
                                  (y-32) );
                displayCodeWrite( DISPLAY_RS_INSTRUCTION,
                                  DISPLAY_IR_SET_GDRAM_ADDR | 
                                  (x+8) );
                displayCodeWrite(DISPLAY_RS_DATA, 
                                 bitmap[2*x + 16*y]);
                displayCodeWrite(DISPLAY_RS_DATA, 
                                 bitmap[2*x+1 + 16*y]);                                 
                }
		}
	}
}

//=====[Implementations of private functions]==================================

static void displayCodeWrite( bool type, uint8_t dataBus )
{
    switch( display.connection ) {
        case DISPLAY_CONNECTION_GPIO_8BITS:
        case DISPLAY_CONNECTION_GPIO_4BITS:
        case DISPLAY_CONNECTION_I2C_PCF8574_IO_EXPANDER:    
            if ( type == DISPLAY_RS_INSTRUCTION )
                displayPinWrite( DISPLAY_PIN_RS, DISPLAY_RS_INSTRUCTION);
                else
                displayPinWrite( DISPLAY_PIN_RS, DISPLAY_RS_DATA);
            displayPinWrite( DISPLAY_PIN_RW, DISPLAY_RW_WRITE );
            displayDataBusWrite( dataBus );
        break;
  
        case DISPLAY_CONNECTION_SPI:
            SPI_ST9720.lock();
            SPI_ST9720_Chip_Select = ON;
            if ( type == DISPLAY_RS_INSTRUCTION )           
                SPI_ST9720.write( ST9720_SPI_SYNCHRONIZING_BIT_STRING |
                                  ST9720_SPI_RW_WRITE |
                                  ST9720_SPI_RS_INSTRUCTION );                              
                else
                SPI_ST9720.write( ST9720_SPI_SYNCHRONIZING_BIT_STRING |
                                  ST9720_SPI_RW_WRITE |
                                  ST9720_SPI_RS_DATA );               
            SPI_ST9720.write( dataBus & 0b11110000 );      
            SPI_ST9720.write( (dataBus<<4) & 0b11110000 );
            SPI_ST9720_Chip_Select = OFF;
            SPI_ST9720.unlock();
        break;
    }    
}

static void displayPinWrite( uint8_t pinName, int value )
{
    switch( display.connection ) {
        case DISPLAY_CONNECTION_GPIO_8BITS:
            switch( pinName ) {
                case DISPLAY_PIN_D0: displayD0 = value;   break;
                case DISPLAY_PIN_D1: displayD1 = value;   break;
                case DISPLAY_PIN_D2: displayD2 = value;   break;
                case DISPLAY_PIN_D3: displayD3 = value;   break;
                case DISPLAY_PIN_D4: displayD4 = value;   break;
                case DISPLAY_PIN_D5: displayD5 = value;   break;
                case DISPLAY_PIN_D6: displayD6 = value;   break;
                case DISPLAY_PIN_D7: displayD7 = value;   break;
                case DISPLAY_PIN_RS: displayRS = value;   break;
                case DISPLAY_PIN_EN: displayEN = value;   break;
                case DISPLAY_PIN_RW: break; 
                default: break;
            }
        case DISPLAY_CONNECTION_GPIO_4BITS:
            switch( pinName ) {
                case DISPLAY_PIN_D4: displayD4 = value;   break;
                case DISPLAY_PIN_D5: displayD5 = value;   break;
                case DISPLAY_PIN_D6: displayD6 = value;   break;
                case DISPLAY_PIN_D7: displayD7 = value;   break;
                case DISPLAY_PIN_RS: displayRS = value;   break;
                case DISPLAY_PIN_EN: displayEN = value;   break;
                case DISPLAY_PIN_RW: break; 
                default: break;
            }
        break;

        case DISPLAY_CONNECTION_I2C_PCF8574_IO_EXPANDER:
           if ( value ) {
                switch( pinName ) {
                    case DISPLAY_PIN_D4: pcf8574.displayPin_D4 = ON; break;
                    case DISPLAY_PIN_D5: pcf8574.displayPin_D5 = ON; break;
                    case DISPLAY_PIN_D6: pcf8574.displayPin_D6 = ON; break;
                    case DISPLAY_PIN_D7: pcf8574.displayPin_D7 = ON; break;
                    case DISPLAY_PIN_RS: pcf8574.displayPin_RS = ON; break;
                    case DISPLAY_PIN_EN: pcf8574.displayPin_EN = ON; break;
                    case DISPLAY_PIN_RW: pcf8574.displayPin_RW = ON; break;
                    case DISPLAY_PIN_A_PCF8574: pcf8574.displayPin_A = ON; break;
                    default: break;
                }
            }
            else {
                switch( pinName ) {
                    case DISPLAY_PIN_D4: pcf8574.displayPin_D4 = OFF; break;
                    case DISPLAY_PIN_D5: pcf8574.displayPin_D5 = OFF; break;
                    case DISPLAY_PIN_D6: pcf8574.displayPin_D6 = OFF; break;
                    case DISPLAY_PIN_D7: pcf8574.displayPin_D7 = OFF; break;
                    case DISPLAY_PIN_RS: pcf8574.displayPin_RS = OFF; break;
                    case DISPLAY_PIN_EN: pcf8574.displayPin_EN = OFF; break;
                    case DISPLAY_PIN_RW: pcf8574.displayPin_RW = OFF; break;
                    case DISPLAY_PIN_A_PCF8574: pcf8574.displayPin_A = OFF; break;
                    default: break;
                }
            }     
            pcf8574.data = 0b00000000;
            if ( pcf8574.displayPin_RS ) pcf8574.data |= 0b00000001; 
            if ( pcf8574.displayPin_RW ) pcf8574.data |= 0b00000010; 
            if ( pcf8574.displayPin_EN ) pcf8574.data |= 0b00000100; 
            if ( pcf8574.displayPin_A  ) pcf8574.data |= 0b00001000; 
            if ( pcf8574.displayPin_D4 ) pcf8574.data |= 0b00010000; 
            if ( pcf8574.displayPin_D5 ) pcf8574.data |= 0b00100000; 
            if ( pcf8574.displayPin_D6 ) pcf8574.data |= 0b01000000; 
            if ( pcf8574.displayPin_D7 ) pcf8574.data |= 0b10000000;                                     
            I2C_PCF8574.write( pcf8574.address, &pcf8574.data, 1);    
        break; 
                        	
        case DISPLAY_CONNECTION_SPI:
        break;
    }
}

static void displayDataBusWrite( uint8_t dataBus )
{
    displayPinWrite( DISPLAY_PIN_EN, OFF );
    displayPinWrite( DISPLAY_PIN_D7, dataBus & 0b10000000 );
    displayPinWrite( DISPLAY_PIN_D6, dataBus & 0b01000000 );
    displayPinWrite( DISPLAY_PIN_D5, dataBus & 0b00100000 );
    displayPinWrite( DISPLAY_PIN_D4, dataBus & 0b00010000 );
    switch( display.connection ) {
        case DISPLAY_CONNECTION_GPIO_8BITS:
            displayPinWrite( DISPLAY_PIN_D3, dataBus & 0b00001000 );
            displayPinWrite( DISPLAY_PIN_D2, dataBus & 0b00000100 );  
            displayPinWrite( DISPLAY_PIN_D1, dataBus & 0b00000010 );      
            displayPinWrite( DISPLAY_PIN_D0, dataBus & 0b00000001 );
        break; 
              
        case DISPLAY_CONNECTION_GPIO_4BITS:
        case DISPLAY_CONNECTION_I2C_PCF8574_IO_EXPANDER:
            if ( initial8BitCommunicationIsCompleted == TRUE) {
            	displayPinWrite( DISPLAY_PIN_EN, ON );         
            	delay( 1 );
            	displayPinWrite( DISPLAY_PIN_EN, OFF );              
            	delay( 1 );        
                displayPinWrite( DISPLAY_PIN_D7, dataBus & 0b00001000 );
                displayPinWrite( DISPLAY_PIN_D6, dataBus & 0b00000100 );  
                displayPinWrite( DISPLAY_PIN_D5, dataBus & 0b00000010 );      
                displayPinWrite( DISPLAY_PIN_D4, dataBus & 0b00000001 );                
            }
        break;
        
        case DISPLAY_CONNECTION_SPI:
        break;
    }
    displayPinWrite( DISPLAY_PIN_EN, ON );              
    delay( 1 );
    displayPinWrite( DISPLAY_PIN_EN, OFF );  
    delay( 1 );                   
}