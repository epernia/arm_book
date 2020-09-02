//=====[#include guards - begin]===============================================

#ifndef _CHAR_LCD_H_
#define _CHAR_LCD_H_

//=====[Libraries]=============================================================

#include "lcd_hd44780_instructions.h"
#include "charLcd_connection.h"
#include "stdint.h"

//=====[Declaration of public defines]=========================================

//=====[Declaration of public data types]======================================

typedef enum{
    DISPLAY_CONNECTION_GPIO_8BITS = 0,
    DISPLAY_CONNECTION_GPIO_4BITS = 1,
    DISPLAY_CONNECTION_I2C        = 2,
    DISPLAY_CONNECTION_SPI        = 2,
} displayConnectionType_t;

//=====[Declarations (prototypes) of public functions]=========================

// Char LCD low level API ---------------------------------------------

static void charLcdCommandWrite( uint8_t command );

static uint8_t charLcdCommandRead( uint8_t command );

static void charLcdDataWrite( uint8_t data );

// Char LCD high level API --------------------------------------------

charLcdStatus_t charLcdInit( charLcdConnectionType_t connectionType,
                             uint32_t charsPerLine, uint32_t charLines,
                             uint32_t charPixelWidth, uint32_t charPixelHeight
                           );

charLcdStatus_t charLcdConnectionSet( void* displayConnection );
void* charLcdConnectionGet();

charLcdStatus_t charLcdPositionWrite( uint8_t positionX, uint8_t positionY );

void charLcdClear();

void charLcdClearAndHome();

charLcdStatus_t charLcdStringWrite( char* str );

//void charLcdCursorMode( charLcdCursorModes_t mode );

// Custom characteres --------------

charLcdStatus_t charLcdCustomCharSave( uint8_t customCharIndex, 
                                       const uint8_t* charBitmap );

charLcdStatus_t displayCustomCharWrite( uint8_t customCharIndex );

//=====[#include guards - end]=================================================

#endif