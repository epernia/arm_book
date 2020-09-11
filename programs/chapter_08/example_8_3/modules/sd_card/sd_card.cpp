//=====[Libraries]=============================================================

#include "mbed.h"
#include "arm_book_lib.h"

#include "sd_card.h"

#include "FATFileSystem.h"
#include "SDBlockDevice.h"

#include "platform/mbed_retarget.h"

#include "event_log.h"
#include "date_and_time.h"
#include "pc_serial_com.h"

//=====[Declaration of private defines]========================================

// SPI_3
#define SPI3_MOSI   D47   // PC_12
#define SPI3_MISO   D46   // PC_11
#define SPI3_SCK    D45   // PC_10
#define SPI3_CS     PA_4_ALT0

//=====[Declaration of private data types]=====================================

//=====[Declaration and initialization of public global objects]===============

SDBlockDevice sd( SPI3_MOSI, SPI3_MISO, SPI3_SCK, SPI3_CS );

FATFileSystem fs("sd", &sd);

//=====[Declaration of external public global variables]=======================

//=====[Declaration and initialization of public global variables]=============

//=====[Declaration and initialization of private global variables]============

//=====[Declarations (prototypes) of private functions]========================

//=====[Implementations of public functions]===================================

bool sdCardInit()
{
    pcSerialComStringWrite("Looking for a filesystem... \r\n");
    fs.mount(&sd);
    DIR *dir = opendir("/sd/");
    if ( dir != NULL ) {
        pcSerialComStringWrite("Filesystem mounted. \r\n");
        closedir(dir);
        return true;
    } else {
        pcSerialComStringWrite("Filesystem not mounted. \r\n");
        pcSerialComStringWrite("Insert an SD card and ");
        pcSerialComStringWrite("reset the board.\r\n");
        return false;
    }
}

bool sdCardWriteFile( const char* fileName, const char* writeBuffer )
{
    char fileNameSD[80];
    
    fileNameSD[0] = 0;
    strncat( fileNameSD, "/sd/", strlen("/sd/") );
    strncat( fileNameSD, fileName, strlen(fileName) );

    FILE *fd = fopen( fileNameSD, "a" );

    if ( fd != NULL ) {
        fprintf( fd, "%s", writeBuffer );                       
        fclose( fd );
        return true;
    } else {
        return false;
    }
}

bool sdCardReadFile( const char * fileName, char * readBuffer )
{
    char fileNameSD[80];
    int i;
    
    fileNameSD[0] = 0;
    strncat( fileNameSD, "/sd/", strlen("/sd/") );
    strncat( fileNameSD, fileName, strlen(fileName) );
    
    FILE *fd = fopen( fileNameSD, "r" );
    
    if ( fd != NULL ) {
        pcSerialComStringWrite( "Opening file: " );
        pcSerialComStringWrite( fileNameSD );
        pcSerialComStringWrite( "\r\n" );

        i = 0;
        while (!feof(fd)) {
           fread( &readBuffer[i], 1, 1, fd );
           i++;
        }
        fclose( fd );
        return true;
    } else {
        pcSerialComStringWrite( "File not found\r\n" );
        return false;
    }
}

bool sdCardListFiles( char* fileNamesBuffer, int fileNamesBufferSize )
{
    int bufferNumberUsedBytes = 0;
    struct dirent *de;

    DIR *dir = opendir("/sd/");

    if ( dir != NULL ) {
        pcSerialComStringWrite("Printing all filenames:\r\n");
        de = readdir(dir);
        
        while ( ( de != NULL ) && 
                ( bufferNumberUsedBytes + strlen(de->d_name) < 
                    fileNamesBufferSize) ) {
            strncat( fileNamesBuffer, de->d_name, strlen(de->d_name) );
            strncat( fileNamesBuffer, "\r\n", strlen("\r\n") );
            bufferNumberUsedBytes = bufferNumberUsedBytes + strlen(de->d_name);
            de = readdir(dir);
        }
        
        closedir(dir);
        
        return true;
    } else {
        pcSerialComStringWrite("Insert an SD card and ");
        pcSerialComStringWrite("reset the board.\r\n");
        return false;
    }
}