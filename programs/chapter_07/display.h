
// 0123456789012345
// 0123456789012345
// 0123456789012345
// 0123456789012345


// Low level Display API --------------------------------------

// Display Commands

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
} displayCmdDisplayCtrl_t;

// Display Commands Flags for function set
typedef enum {
    DISPLAY_CMDF_8BITMODE = 0x10,
    DISPLAY_CMDF_4BITMODE = 0x00,
    DISPLAY_CMDF_2LINE    = 0x08,
    DISPLAY_CMDF_1LINE    = 0x00,
    DISPLAY_CMDF_5x10DOTS = 0x04,
    DISPLAY_CMDF_5x8DOTS  = 0x00,
} displayCmdFunctionSet_t;


// High level Display API -------------------------------------

typedef enum {
     DISPLAY_CONNECTION_GPIO_4BITS,
     DISPLAY_CONNECTION_GPIO_8BITS,
     DISPLAY_CONNECTION_I2C,
     DISPLAY_CONNECTION_SPI
} displayConnection_t;

typedef enum {
     DISPLAY_TYPE_CHAR_LCD, 
     DISPLAY_TYPE_GLCD
} displayType_t;

typedef enum {
     DISPLAY_MODE_CHAR, 
     DISPLAY_MODE_GRAPHIC
} displayMode_t;

typedef enum {
     DISPLAY_ERR_0,
     DISPLAY_ERR_1,
} displayError_t;

typedef struct {                   //  Char   Graphic

   // Char LCD and GLCD

   displayConnection_t connection; // DISPLAY_CONNECTION_GPIO_4BITS, DISPLAY_CONNECTION_GPIO_8BITS, DISPLAY_CONNECTION_I2C, DISPLAY_CONNECTION_SPI
   
   displayType_t type;             // DISPLAY_TYPE_CHAR_LCD, DISPLAY_TYPE_GLCD
   
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


// Display API Functions ------------------------------------------------------

// Char LCD and GLC

displayError_t displayInit( displayConnection_t connection, displayType_t type,
                            int charsPerLine, int charLines,
                            int charPixelWidth, int charPixelHeight,
                            int glcdPixelWidth, int glcdPixelHeight );
 
displayError_t displayCommandWrite( displayCommand_t command );

displayError_t displayDataWrite( uint8_t data );

displayError_t displayCharPositionWrite( uint8_t charPositionX, 
                                         uint8_t charPositionY );

displayError_t displayStrigWrite( char* str );

// Only GLC
                                         
displayError_t displayModeWrite( displayMode_t mode );

displayError_t displayBitmapWrite( uint8_t* bitmap );

displayError_t displayPixelPositionWrite( int pixelPositionX, 
                                          int pixelPositionY,
                                          bool pixelValue );

// ----------------------------------------------------------------------------

// Recordar!

// Hablar de hal empezando con sensor de temperatura y gas y hablar de capas de software.
