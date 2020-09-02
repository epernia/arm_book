//=====[#include guards - begin]===============================================

#ifndef _DISPLAY_CHAR_LCD_CONNECTION_H_
#define _DISPLAY_CHAR_LCD_CONNECTION_H_

//=====[Libraries]=============================================================

//=====[Declaration of public defines]=========================================

//=====[Declaration of public data types]======================================

// Char LCD possible connections -------------------------

// GPIO 8-bit connection
typedef struct{
    displayConnectionType_t connectionType;
    displayGpio_t rs;  // RS GPIO pin.
    displayGpio_t rw;  // R/W GPIO pin.
    displayGpio_t en;  // EN (Enable) GPIO pin.
    displayGpio_t d0;  // Data bit 0 GPIO pin.
    displayGpio_t d1;  // Data bit 1 GPIO pin.
    displayGpio_t d2;  // Data bit 2 GPIO pin.
    displayGpio_t d3;  // Data bit 3 GPIO pin.
    displayGpio_t d4;  // Data bit 4 GPIO pin.
    displayGpio_t d5;  // Data bit 5 GPIO pin.
    displayGpio_t d6;  // Data bit 6 GPIO pin.
    displayGpio_t d7;  // Data bit 7 GPIO pin.
    displayPwm_t bla;  // BLA (Backlight anode) PWM pin.
} charLcdGpio8BitConnection_t;

// GPIO 4-bit connection
typedef struct{
    displayGpio_t rs;  // RS GPIO pin.
    displayGpio_t rw;  // R/W GPIO pin.
    displayGpio_t en;  // EN (Enable) GPIO pin.
    displayGpio_t d4;  // Data bit 4 GPIO pin.
    displayGpio_t d5;  // Data bit 5 GPIO pin.
    displayGpio_t d6;  // Data bit 6 GPIO pin.
    displayGpio_t d7;  // Data bit 7 GPIO pin.
    displayPwm_t bla;  // BLA (Backlight anode) PWM pin.
} charLcdGpio4BitConnection_t;

// I2C connection (by adding a PCF8574T I/O expander)
typedef struct{
    displayConnectionType_t connectionType;
    displayI2c_t i2c;  // I2C port (connect RS, R/W, EN, D4 to D7 and BLA).
} charLcdI2cConnection_t;

//=====[Declarations (prototypes) of public functions]=========================

//=====[#include guards - end]=================================================

#endif