//=====[#include guards - begin]===============================================

#ifndef _DISPLAY_H_
#define _DISPLAY_H_

//=====[Declaration of public defines]=======================================

//=====[Declaration of public data types]======================================

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
    DISPLAY_CMDF_8BITMODE = 0x10,
    DISPLAY_CMDF_4BITMODE = 0x00,
    DISPLAY_CMDF_2LINE    = 0x08,
    DISPLAY_CMDF_1LINE    = 0x00,
    DISPLAY_CMDF_5x10DOTS = 0x04,
    DISPLAY_CMDF_5x8DOTS  = 0x00,
} displayCmdFunctionSet_t;

typedef enum {
    DISPLAY_ERR_INVALID_POSITION,
    DISPLAY_NO_ERR,
} displayStatus_t;

typedef struct {
    int charsPerLine;
    int charLines;
    int charPositionX;
    int charPositionY;
} display_t;

//=====[Declarations (prototypes) of public functions]=========================

displayStatus_t displayCommandWrite( uint8_t command );

displayStatus_t displayDataWrite( uint8_t data );

displayStatus_t displayInit( int charsPerLine, int charLines );
 
displayStatus_t displayCharPositionWrite( uint8_t charPositionX, 
                                          uint8_t charPositionY );

displayStatus_t displayStringWrite( char* str );

displayStatus_t displayUpdate();

//=====[#include guards - end]=================================================

#endif // _DISPLAY_H_