
// Low level Display API ======================================================

// Instruction flags ----------------------------------------------------------

// Entry mode set flags
#define DISPLAY_CMDF_ID(value) ((value)<<1) // I/D=1: Increment.
                                            // I/D=0: Decrement.
#define DISPLAY_CMDF_S(value)  ((value)<<0) // S=1: Accompanies display shift.

// Display on/off control flags
#define DISPLAY_CMDF_D(value)  ((value)<<2) // D: Sets entire display on/off.
#define DISPLAY_CMDF_C(value)  ((value)<<1) // C: cursor on/off.
#define DISPLAY_CMDF_B(value)  ((value)<<0) // B: Blinking of cursor position
                                            // character.

// Cursor or display shift flags
#define DISPLAY_CMDF_SC(value) ((value)<<3) // S/C=1: Display shift. 
                                            // S/C=0: Cursor move.
#define DISPLAY_CMDF_RL(value) ((value)<<2) // R/L=1: Shift to the right.
                                            // R/L=0: Shift to the left.

// Function set and extended function set flags
#define DISPLAY_CMDF_DL(value) ((value)<<4) // DL=1: 8 bits. DL=0: 4 bits.

#define DISPLAY_CMDF_N(value)  ((value)<<3) // N=1: 2 lines. N=0: 1 line.
#define DISPLAY_CMDF_F(value)  ((value)<<2) // F=1: 5x10 dots. F=0: 5x8 dots.

#define DISPLAY_CMDF_RE(value) ((value)<<2) // RE=1: extended instruction
#define DISPLAY_CMDF_G(value)  ((value)<<1) // RE=0: basic instruction

// Read busy flag (BF) and address counter (AC) flags
#define DISPLAY_CMDF_BF(value) (((value)&(1<<7))>>7) // BF=1: Internally
                                                     // operating. 
                                                     // BF=0: Instructions
                                                     // acceptable.
#define DISPLAY_CMDF_AC(value) ((value)&0x7F) // AC: Address counter used for
                                              // RAM addresses.

// Scroll or RAM address select flags
#define DISPLAY_CMDF_SR(value) ((value)<<0) // SR=1: the vertical scroll
                                            // address set is enabled.
                                            // SR=0: the IRAM address set
                                            // (extended instruction) and
                                            // CGRAM address set(basic
                                            // instruction) is enabled.

// Reverse (by line) flags
#define DISPLAY_CMDF_R1(value) ((value)<<1)
#define DISPLAY_CMDF_R0(value) ((value)<<0)
// R1 R0  Description
//  0  0  First line normal or reverse
//  0  1  Second line normal or reverse
//  1  0  Third line normal or reverse
//  1  1  Fourth line normal or reverse


// DDRAM: Display data RAM
// CGRAM: Character generator RAM
// ACG: CGRAM address
// ADD: DDRAM address (corresponds to cursor address)
// AC: Address counter used for both DD and CGRAM addresses

// Instructions RS RW DB7 DB6 DB5 DB4 DB3 DB2 DB1 DB0 -------------------------

typedef enum {

// CHAR LCD HD44780 and GLCD ST7920 128x64  basic 8-bit instuctions 
// (RE=0, RS=0, RW=0)

    // Clear display
    DISPLAY_CMD_CLEAR          = 0x01, // Clears entire display and sets DDRAM 
                                       // address counter (AC) to 0.
    // Return home
    DISPLAY_CMD_HOME           = 0x02, // Sets DDRAM address counter (AC) to 0. 
                                       // Also returns display from being 
                                       // shifted to original position. 
                                       // DDRAM contents remain unchanged.
    // Entry mode set
    DISPLAY_CMD_ENTRY_MODE_SET = 0x04, // Sets cursor move direction and
                                       // specifies display shift.
                                       // These operations are performed during
                                       // data write and read.
    // Display on/off control
    DISPLAY_CMD_ON_OFF_CTRL    = 0x08, // D=1: display on, D=0 display off.
                                       // C=1: cursor on, c=0 cursor off.
                                       // B=1: Blinking of cursor position 
                                       // character, B=0 not.
    // Cursor or display shift
    DISPLAY_CMD_CURSOR_MOVE_OR_DISPLAY_SHIFT = 0x10, // Moves cursor and shifts
                                                     // display without changing
                                                     // DDRAM contents
    // Function set
    DISPLAY_CMD_FUNCTION_SET   = 0x20, // In both LCDs:
                                       //    DL=1: 8-BIT interface data length
                                       //    DL=0: 4-BIT interface data length
                                       // In Char LCD:
                                       //    N=1: 2-line display
                                       //    N=0: 1-line display
                                       //    F=1: 5x7 dot character font
                                       //    F=0: 5x8 dot character font
                                       // In GLCD:
                                       //    RE=1: extended instruction
                                       //    RE=0: basic instruction
    // Set GCRAM address
    DISPLAY_CMD_SET_GCRAM_ADDR = 0x40, // Set CGRAM address to address counter 
                                       // （AC）. 
                                       // CGRAM data is sent and received after
                                       // this setting.
                                       // In GLCD: Make sure that in extended
                                       // instruction SR=0 (scroll or RAM 
                                       // address select)
    // Set DDRAM address
    DISPLAY_CMD_SET_DDRAM_ADDR = 0x80, // Set DDRAM address to address counter 
                                       // （AC）. 
                                       // DDRAM data is sent and received after 
                                       // this setting.
                                       // Note that AC6 is fixed to 0.

// CHAR LCD HD44780 and GLCD ST7920 128x64 basic 10-bit instuctions 
// (RE=0, RS!=0 or RW!=0)

    // Read busy flag (BF) and address counter (AC)
    DISPLAY_CMD_READ_BF_AND_AC = 0x100, // Reads busy flag (BF) indicating
                                        // internal operation is being
						                // performed and reads address counter
                                        // (AC) value.
    // Write data to RAM
    DISPLAY_CMD_WRITE_RAM      = 0x200, // Write data to internal RAM:
                                        //  - In Char LCD: from DDRAM or CGRAM,
						                //  - In GLCD: from DDRAM, CGRAM, IRAM
                                        //    or GDRAM.
    // Read from RAM
    DISPLAY_CMD_READ_RAM       = 0x300, // Read data from internal RAM:
                                        //  - In Char LCD: from DDRAM or CGRAM.
						                //  - In GLCD: from DDRAM, CGRAM, IRAM
                                        //    or GDRAM.

// GLCD ST7920 128x64 Extended 8-bit instuctions (RE=1, RS=0, RW=0)

    // Standby
    DISPLAY_CMD_STANDBY                   = 0x01, // Enter stand by mode, any
                                                  // other instruction can
                                                  // terminate (Com1..32
                                                  //  halted, only Com33 ICON
                                                  // can display)
    // Scroll or RAM address select
    DISPLAY_CMD_SCROLL_OR_RAM_ADDR_SELECT = 0x02, // SR=1: enable vertical
                                                  // scroll position.
                                                  // SR=0: enable IRAM address
                                                  // (extended instruction).
                                                  // SR=0: enable CGRAM address
                                                  // (basic instruction).
    // Reverse (by line)
    DISPLAY_CMD_REVERSE                   = 0x04, // Select 1 out of 4 line
                                                  // (in DDRAM) and decide
                                                  // whether to reverse the
                                                  // display by toggling this
                                                  // instruction R1, R0 initial 
                                                  // value is 0, 0.
    // Extended function set
    DISPLAY_CMD_EXTENDED_FUNCTION_SET     = 0x20, // DL=1: 8-BIT interface.
                                                  // DL=0: 4-BIT interface.
                                                  // RE=1: extended instruction
                                                  // set.
                                                  // RE=0: basic instruction 
                                                  // set.
                                                  // G=1: graphic display ON.
                                                  // G=0: graphic display OFF.
    // Set IRAM or scroll address
    DISPLAY_CMD_SET_IRAM_OR_SCROLL_ADDR   = 0x40, // R=1: AC5~AC0 the address
                                                  // of vertical scroll.
                                                  // SR=0: AC3~AC0 the address
                                                  // of ICON RAM.
    // Set grephic display RAM address   
    DISPLAY_CMD_SET_GRAPHIC_RAM_ADDR      = 0x80, // Set GDRAM address to
                                                  // address counter （AC）. 
                                                  // First set vertical address
                                                  // and thehorizontal address
                                                  // by consecutive writing:
                                                  // Vertical address range
                                                  // AC6...AC0
                                                  // Horizontal address range 
                                                  // AC3...AC0.
} displayCommand_t;

// ----------------------------------------------------------------------------

// High level Display API =====================================================

typedef enum {
     DISPLAY_CONNECTION_GPIO_4BITS,
     DISPLAY_CONNECTION_GPIO_8BITS,
     DISPLAY_CONNECTION_I2C,
     DISPLAY_CONNECTION_SPI
} displayConnection_t;

typedef enum {
     DISPLAY_TYPE_LCD_HD44780, 
     DISPLAY_TYPE_GLCD_ST7920,
} displayType_t;

typedef enum {
     DISPLAY_MODE_CHAR, 
     DISPLAY_MODE_GRAPHIC,
} displayMode_t;

typedef enum {
     DISPLAY_ERR_0,
     DISPLAY_ERR_1,
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



display_t display1 = {
    DISPLAY_CONNECTION_SPI,
    DISPLAY_TYPE_GLCD_ST7920,
    16, 4,
    0, 0,
    5, 8,
    DISPLAY_MODE_GRAPHIC,
    128, 64,
    0, 0,
};

display_t display2 = {
    DISPLAY_CONNECTION_GPIO_4BITS,
    DISPLAY_TYPE_LCD_HD44780,
    16, 4,
    0, 0,
    5, 8,
    DISPLAY_MODE_CHAR,
    0, 0,
    0, 0,
};

// Display API Functions ======================================================

// Example 7-1 Char LCD por GPIOs (8 bit) -------------------------------------

// Internas
displayStatus_t displayCommandWrite( displayCommand_t command );
uint8_t displayCommandRead( displayCommand_t command );

displayStatus_t displayDataWrite( uint8_t data );

// Externas
displayStatus_t displayInit( int charsPerLine, int charLines,
                             int charPixelWidth, int charPixelHeight );

displayStatus_t displayCharPositionWrite( uint8_t charPositionX, 
                                         uint8_t charPositionY );

displayStatus_t displayStrigWrite( char* str );

// Example 7-2 Char LCD por I2C (el LCD internamente se configura como 4 bit) -

displayStatus_t displayInit( displayConnection_t connection,
                             int charsPerLine, int charLines,
                             int charPixelWidth, int charPixelHeight );

// Example 7-3 Char LCD por I2C con custom characteres ------------------------

displayStatus_t displayInit( displayConnection_t connection,
                             int charsPerLine, int charLines,
                             int charPixelWidth, int charPixelHeight );

displayStatus_t displayCustomCharSave( uint8_t customCharIndex, 
                                       const char* charBitmap );

displayStatus_t displayCustomCharWrite( uint8_t customCharIndex );

// Over temperature - Termometer full scale
const char overTempCustomCharBitmap[] = {
    0b01110,
    0b01010,
    0b01010,
    0b01110,
    0b01110,
    0b10111,
    0b11111,
    0b01110
};

// Temperature normal level icon - Termometer low scale
const char normalTempCustomCharBitmap[] = {
    0b01110,
    0b01010,
    0b01010,
    0b01010,
    0b10001,
    0b11111,
    0b01110,
    0b00000
};

// Gas detected icon - Smoke
const char gasDetectedCustomCharBitmap[] = {
    0b01111,
    0b11100,
    0b01110,
    0b00011,
    0b00110,
    0b00100,
    0b01110,
    0b00000
};

// Gas not detected icon - No smoke
const char gasNotDetectedCustomCharBitmap[] = {
    0b00000,
    0b00000,
    0b00000,
    0b00000,
    0b00000,
    0b00000,
    0b01110,
    0b00000
};

// Fire Alarm icon - Fire
const char fireAlarmCustomCharBitmap[] = {
    0b00100,
    0b00110,
    0b01110,
    0b10110,
    0b11101,
    0b11111,
    0b01110,
    0b00000
};

// Example 7-4 GLCD por SPI usando modo texto ---------------------------------

displayStatus_t displayInit( displayType_t type,
                             displayConnection_t connection,
                             int charsPerLine, int charLines,
                             int charPixelWidth, int charPixelHeight,
                             int glcdPixelWidth, int glcdPixelHeight );

// Example 7-5 GLCD por SPI usando modo gráfico -------------------------------

displayStatus_t displayBitmapWrite( uint8_t* bitmap );

displayStatus_t displayPixelPositionWrite( int pixelPositionX, 
                                          int pixelPositionY,
                                          bool pixelValue );

displayStatus_t displayModeWrite( displayMode_t mode );


// Over temperature - Termometer full scale
const char overTempBitmap[] = {
};

// Temperature normal level icon - Termometer low scale
const char normalTempBitmap[] = {
};

// Gas detected icon - Smoke
const char gasDetectedBitmap[] = {
};

// Gas not detected icon - No smoke
const char gasNotDetectedBitmap[] = {
};

// Fire Alarm icon - Fire
const char fireAlarmBitmap0[] = {
};

// ----------------------------------------------------------------------------
