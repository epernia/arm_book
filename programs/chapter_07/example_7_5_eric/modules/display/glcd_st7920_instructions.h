//=====[#include guards - begin]===============================================

#ifndef _DISPLAY_GLCD_ST7920_INSTURCTIONS_H_
#define _DISPLAY_GLCD_ST7920_INSTURCTIONS_H_

//=====[Libraries]=============================================================

//=====[Declaration of public defines]=========================================

// Command flags --------------------------------------------------------------

// Extended function set flags
#define DISPLAY_CMDF_RE(value) ((value)<<2) // RE=1: extended instruction
#define DISPLAY_CMDF_G(value)  ((value)<<1) // RE=0: basic instruction

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

//=====[Declaration of public data types]======================================

// Display commands RS RW DB7 DB6 DB5 DB4 DB3 DB2 DB1 DB0 ---------------------

typedef enum {
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
} GLCD_ST7920_ExtendedInstructions_t;

//=====[Declarations (prototypes) of public functions]=========================

//=====[#include guards - end]=================================================

#endif // _DISPLAY_GLCD_ST7920_INSTURCTIONS_H_