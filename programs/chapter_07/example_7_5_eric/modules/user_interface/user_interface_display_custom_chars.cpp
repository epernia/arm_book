//=====[Libraries]=============================================================

#include "mbed.h"

//=====[Declaration of private defines]======================================

//=====[Declaration of private data types]=====================================

//=====[Declaration and initialization of public global objects]===============

//=====[Declaration of external public global variables]=======================

//=====[Declaration and initialization of public global variables]=============

// Over temperature - Termometer full scale
const uint8_t overTempCustomCharBitmap[] = {
    0b01110,
    0b01010,
    0b01010,
    0b01110,
    0b10111,
    0b11111,
    0b01110,
    0b00000
};

// Temperature normal level icon - Termometer low scale
const uint8_t normalTempCustomCharBitmap[] = {
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
const uint8_t gasDetectedCustomCharBitmap[] = {
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
const uint8_t gasNotDetectedCustomCharBitmap[] = {
    0b00000,
    0b00000,
    0b00000,
    0b00000,
    0b00000,
    0b00000,
    0b01110,
    0b00000
};

// Fire Alarm icon 0 - Fire
const uint8_t fireAlarmCustomCharBitmap0[] = {
    0b00000,
    0b00100,
    0b01110,
    0b01101,
    0b10111,
    0b11110,
    0b01110,
    0b00000,
};

// Fire Alarm icon 1 - Fire
const uint8_t fireAlarmCustomCharBitmap1[] = {
    0b00100,
    0b00110,
    0b01100,
    0b10110,
    0b11101,
    0b11111,
    0b01110,
    0b00000,
};

// Fire Alarm icon 2 - Fire
const uint8_t fireAlarmCustomCharBitmap2[] = {
    0b00100,
    0b00000,
    0b10101,
    0b01111,
    0b01110,
    0b11111,
    0b01110,
    0b00000,
};

//=====[Declaration and initialization of private global variables]============

//=====[Declarations (prototypes) of private functions]========================

//=====[Implementations of public functions]===================================

//=====[Implementations of private functions]==================================