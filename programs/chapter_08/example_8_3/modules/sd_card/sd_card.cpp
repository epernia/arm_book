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
    pcSerialComStringWrite("Mounting the filesystem... \r\n");
    fs.mount(&sd);
    DIR *dir = opendir("/sd/");
    if ( dir != NULL ) {
        pcSerialComStringWrite("Filesystem mounted... \r\n");
        closedir(dir);
        return true;
    } else {
        pcSerialComStringWrite("Filesystem not mounted... \r\n");
        return false;
    }
}

bool sdCardWriteFile( const char* fileName, const char* writeBuffer )
{
    char fileNameSD[80];
    
    fileNameSD[0] = 0;
    strncat( fileNameSD, "/sd/", strlen("/sd/") );
    strncat( fileNameSD, fileName, strlen(fileName) );

    pcSerialComStringWrite( fileNameSD );
    pcSerialComStringWrite( "\r\n" );
    
    FILE *fd = fopen( fileNameSD, "a" );

    if ( fd != NULL ) {
        fprintf( fd, "%s", writeBuffer );                       
        fclose( fd );
        return true;
    } else {
        return false;
    }
}

bool sdCardReadFile( const char * fileName, const char * readBuffer )
{
    char fileNameSD[80];
    char buff[16] = {0};
    
    fileNameSD[0] = 0;
    strncat( fileNameSD, "/sd/", strlen("/sd/") );
    strncat( fileNameSD, fileName, strlen(fileName) );
    
    FILE *fd = fopen( fileNameSD, "r" );
    
    if ( fd != NULL ) {
        pcSerialComStringWrite( "Opening file: " );
        pcSerialComStringWrite( fileNameSD );
        pcSerialComStringWrite( "\r\n" );
        pcSerialComStringWrite( "Dumping file to screen.\r\n");

        while (!feof(fd)) {
           fread( &buff[0], 1, 1, fd );
           pcSerialComStringWrite(buff);
        }
        fclose( fd );
        return true;
    } else {
        pcSerialComStringWrite( "File not found\r\n" );
        return false;
    }
}

bool sdCardListFiles()
{
    int error = 0;
    struct dirent *de;

    DIR *dir = opendir("/sd/");

    if ( dir != NULL ) {
        pcSerialComStringWrite("Printing all filenames:\r\n");
        while ((de = readdir(dir)) != NULL) {
            pcSerialComStringWrite ( &(de->d_name)[0]);
            pcSerialComStringWrite("\r\n");
        }
        closedir(dir);
        return true;
    } else {
        pcSerialComStringWrite("Directory not found...\r\n");
        return false;
    }
}