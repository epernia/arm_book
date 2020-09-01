//=====[#include guards - begin]===============================================

#ifndef _DISPLAY_H_
#define _DISPLAY_H_

//=====[Libraries]=============================================================

#include "mbed.h"
#include "display_commands.h"

//=====[Declaration of public defines]=======================================

//=====[Declaration of public data types]======================================

typedef enum {
     DISPLAY_TYPE_LCD_HD44780, 
     DISPLAY_TYPE_GLCD_ST7920,
} displayType_t;

typedef enum {
     DISPLAY_CONNECTION_GPIO_8BITS,
     DISPLAY_CONNECTION_GPIO_4BITS,
     DISPLAY_CONNECTION_I2C,
     DISPLAY_CONNECTION_SPI
} displayConnection_t;

typedef enum {
     DISPLAY_MODE_CHAR, 
     DISPLAY_MODE_GRAPHIC
} displayMode_t;

typedef enum {
     DISPLAY_STATUS_OK,
     DISPLAY_STATUS_ERR,
} displayStatus_t;

typedef struct {                   //  Char   Graphic

   // Char LCD and GLCD
   
   displayType_t type;             // DISPLAY_TYPE_CHAR_LCD, DISPLAY_TYPE_GLCD

   displayConnection_t connection; // DISPLAY_CONNECTION_GPIO_4BITS, DISPLAY_CONNECTION_GPIO_8BITS, DISPLAY_CONNECTION_I2C, DISPLAY_CONNECTION_SPI
   
   int charsPerLine;               //     0    16
   int charLines;                  //     4     4
                                   
   int charPositionX;              //     0     0 (default)
   int charPositionY;              //     0     0 (default)
                                   
   int charPixelWidth;             //     5     8
   int charPixelHeight;            //     8    16 
   
   // GLCD
   
   displayMode_t mode;             // DISPLAY_MODE_CHAR (default), DISPLAY_MODE_GRAPHIC
                                   
   int glcdPixelWidth;             //     -   128
   int glcdPixelHeight;            //     -    64

   int pixelPositionX;             //     -     0 (default)
   int pixelPositionY;             //     -     0 (default)
} display_t;

//=====[Declarations (prototypes) of public functions]=========================

// Display low level API ---------------------------------------------

displayStatus_t displayCommandWrite( displayCommand_t command );

uint8_t displayCommandRead( displayCommand_t command );

displayStatus_t displayDataWrite( uint8_t data );

// Display high level API --------------------------------------------

displayStatus_t displayInit( displayType_t type,
                             displayConnection_t connection,
                             int charsPerLine, int charLines,
                             int charPixelWidth, int charPixelHeight,
                             int glcdPixelWidth, int glcdPixelHeight );

displayStatus_t displayModeWrite( displayMode_t mode );

// Char mode ---------------------------------

displayStatus_t displayCharPositionWrite( uint8_t charPositionX, 
                                          uint8_t charPositionY );

displayStatus_t displayStringWrite( char* str );

// Char mode Custom characteres --------------

displayStatus_t displayCustomCharSave( uint8_t customCharIndex, 
                                       const char* charBitmap );

displayStatus_t displayCustomCharWrite( uint8_t customCharIndex );

// Graphic mode-------------------------------

displayStatus_t displayBitmapWrite( uint8_t* bitmap );

displayStatus_t displayPixelPositionWrite( int pixelPositionX, 
                                           int pixelPositionY,
                                           bool pixelValue );

//=====[#include guards - end]=================================================

#endif // _DISPLAY_H_