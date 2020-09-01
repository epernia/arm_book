

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

// Display low level API ---------------------------------------------

displayStatus_t displayCommandWrite( displayCommand_t command );
uint8_t displayCommandRead( displayCommand_t command );

displayStatus_t displayDataWrite( uint8_t data );

// Display high level API --------------------------------------------

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

// Example 7-4 GLCD por SPI usando modo texto ---------------------------------

displayStatus_t displayInit( display_t* display,
                             displayType_t type,
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


/*

Funcionalidad Smart Home System hasta Chapter 6 -------------------------------

https://docs.google.com/document/d/1hEsRCKBs3JFHS4ISZnKhub_Fq3hCdSUAFROuOshLWu0/edit

LINKS LCD y GLCD --------------------------------------------------------------

CAPSE Char LCD:

https://docs.google.com/presentation/d/1FGUuhsB7eZqShnjHx24m0v6nBdULeNI2GgjxYNIFRGg/edit#slide=id.g2a10af01df_0_48


Crear caracteres propios:

https://maxpromer.github.io/LCD-Character-Creator/


GLCD conexión arduino:

https://saber.patagoniatec.com/2014/11/lcd-spi-128x64-serie-arduino-argentina-ptec-12864-grafico-glcd/

https://www.instructables.com/id/Interface-GLCD-with-Arduino/

GLCD Teoría y otros MCUs:

https://monarcaelectronica.com.ar/productos/display-lcd-grafico-128x64-glcd-st7920-arduino-mona/?gclid=CjwKCAjwm_P5BRAhEiwAwRzSO_ZjrO-Ovpht7SSkjWkINKuntHiCVFhWpebjLM_qYfoE6PbswPiMPBoCfDAQAvD_BwE

https://www.buydisplay.com/8051-microcontroller-development-board-for-graphic-lcd-erm12864-6

https://www.electronicwings.com/8051/glcd-128x64-interfacing-with-at89s51-microcontroller

https://www.electronicwings.com/avr-atmega/graphical-lcd-128x64-interfacing-with-atmega1632

https://exploreembedded.com/wiki/Interfacing_GLCD(128x64)_with_PIC16F877A

https://circuitdigest.com/microcontroller-projects/graphical-lcd-interfacing-with-arduino


Teoria posicion de los caracteres en el GLCD:

https://www.onetransistor.eu/2018/03/code-text-mode-st7920-arduino.html


Libs Nucleo:

https://github.com/cbm80amiga/ST7920_SPI

https://controllerstech.com/glcd-128x64-st7920-interfacing-with-stm32/

https://os.mbed.com/questions/52443/Using-Text-LCD-with-Nucleo-F401RE/


Lib multiplataforma:

https://github.com/olikraus/u8g2/wiki/Porting-to-new-MCU-platform#hardware-spi-communication


Lib Arduino:



*/



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
