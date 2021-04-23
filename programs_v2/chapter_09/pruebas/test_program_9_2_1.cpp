//=====[Libraries]=============================================================

#include "mbed.h"
#include "arm_book_lib.h"

#include "FATFileSystem.h"
#include "SDBlockDevice.h"

#include "platform/mbed_retarget.h"

//=====[Defines]=============================================================

#define SPI3_MOSI   PC_12
#define SPI3_MISO   PC_11
#define SPI3_SCK    PC_10
#define SPI3_CS     PA_4_ALT0

//=====[Declaration of public data types]======================================

//=====[Declaration and initialization of public global objects]===============

SDBlockDevice sd( SPI3_MOSI, SPI3_MISO, SPI3_SCK, SPI3_CS );

FATFileSystem fs("sd", &sd);

Serial uartUsb(USBTX, USBRX);

//=====[Declaration and initialization of public global variables]=============

//=====[Declarations (prototypes) of public functions]=========================

//=====[Main function, the program entry point after power on or reset]========

int main()
{
    uartUsb.printf("Looking for a filesystem in the SD card... \r\n");
    fs.mount(&sd);
    DIR *dir = opendir("/sd/");
    if ( dir != NULL ) {
        uartUsb.printf("Filesystem found in the SD card. \r\n");
        closedir(dir);
        return true;
    } else {
        uartUsb.printf("Filesystem not mounted. \r\n");
        uartUsb.printf("Insert an SD card and ");
        uartUsb.printf("reset the NUCLEO board.\r\n");
        return false;
    }
    
    while (true) {
        
        if( uartUsb.readable() ) {
            char receivedChar = uartUsb.getc();
            switch (receivedChar) {
            case 'w': {
            
                char fileNameSD[80];
                
                fileNameSD[0] = 0;
                strncat( fileNameSD, "/sd/", strlen("/sd/") );
                strncat( fileNameSD, "Hello.txt", strlen("Hello.txt") );

                FILE *fd = fopen( fileNameSD, "a" );

                if ( fd != NULL ) {
                    fprintf( fd, "%s", "Hello World!" );                       
                    fclose( fd );
                    return true;
                } else {
                    return false;
                }
                
                break;
            }
            case 'l': {
            
                int NumberOfUsedBytesInBuffer = 0;
                struct dirent *de;

                DIR *dir = opendir("/sd/");

                if ( dir != NULL ) {
                    uartUsb.printf("Printing all filenames:\r\n");
                    de = readdir(dir);
                    
                    while ( de != NULL ) {
                                    
                        uartUsb.printf( de->d_name );
                        uartUsb.printf("\r\n");

                        de = readdir(dir);
                    }
                    
                    closedir(dir);
                    uartUsb.printf("\r\n");
                    
                    return true;
                } else {
                    uartUsb.printf("Insert an SD card and ");
                    uartUsb.printf("reset the NUCLEO board.\r\n");
                    return false;
                }
                break;
            }
            default:

                break;
                
            }
        }
    }
}

//=====[Implementations of public functions]===================================