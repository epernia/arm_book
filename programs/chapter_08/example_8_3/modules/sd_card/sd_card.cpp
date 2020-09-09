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

static void return_error(int ret_val);
static int errno_error(void *ret_val);

//=====[Implementations of public functions]===================================

bool sdCardInit()
{
    int error = 0;

    pcSerialComStringWrite("Mounting the filesystem... \r\n");
    error = fs.mount(&sd);

    if ( error == FR_OK ) {
        pcSerialComStringWrite("Filesystem mounted... \r\n");
        return true;

    } else {
        switch (error) {
            case FR_INVALID_DRIVE:
                pcSerialComStringWrite("Invalid Drive... \r\n");
            break;
            case FR_DISK_ERR:
                pcSerialComStringWrite("Disk Error... \r\n");
            break;
            case FR_NOT_READY:
                pcSerialComStringWrite("Not Ready... \r\n");
            break;
            case FR_NOT_ENABLED:
                pcSerialComStringWrite("Not enabled.. \r\n");
            break;
            case FR_NO_FILESYSTEM:
                pcSerialComStringWrite("No file system... \r\n");
            break;
        }
        return false;
    }
}

bool sdCardWriteFile( const char* fileName, const char* writeBuffer )
{
    char fileNameSD[80];
    int i;
    int error;
    
    fileNameSD[0] = 0;
    strncat( fileNameSD, "/sd/", strlen("/sd/") );
    strncat( fileNameSD, fileName, strlen(fileName) );

    pcSerialComStringWrite( fileNameSD );
    pcSerialComStringWrite( "\r\n" );
    
    FILE *fd = fopen( fileNameSD, "w+" );
    error = errno_error( fd );

    if (!error) {
        fprintf( fd, "%s", writeBuffer );                       
        fclose( fd );
    }
    return true;
}

void sdCardReadFile( const char * fileName, const char * readBuffer )
{
    char fileNameSD[80];
    fileNameSD[0] = 0;
    strncat( fileNameSD, "/sd/", strlen("/sd/") );
    strncat( fileNameSD, fileName, strlen(fileName) );
    
    FILE *fd = fopen( fileNameSD, "r" );
    
    if ( !errno_error( fd ) ) {
        printf( "Opening file: %s\r\n", fileNameSD );
        printf( "Dumping file to screen.\r\n");
        char buff[16] = {0};
        while (!feof(fd)) {
            int size = fread( &buff[0], 1, 15, fd );
            fwrite( &buff[0], 1, size, stdout );
        }
        printf( "EOF.\n" );
        printf( "Closing file." );
        fclose( fd );
        printf( "done.\n" );
    } else {
        pcSerialComStringWrite( "File not found\r\n" );
    }
}

bool sdCardListFiles()
{
    int error = 0;
    printf("Opening root directory.");
    DIR *dir = opendir("/sd/");
 
    struct dirent *de;
    printf("Printing all filenames:\n");
    while ((de = readdir(dir)) != NULL) {
        pcSerialComStringWrite ( &(de->d_name)[0]);
        pcSerialComStringWrite("\r\n");
    }

    printf("Closing root directory. ");
    error = closedir(dir);
    return_error(error);

    return true;
}

//=====[Implementations of private functions]==================================

void return_error(int ret_val)
{
    if (ret_val) {
        printf("Failure. %d\n", ret_val);
        //while (true) {
         //   __WFI();
        //}
    } else {
        printf("done.\n");
    }
}

int errno_error(void *ret_val)
{
    if (ret_val == NULL) {
        printf(" Failure. %d \n", errno);
        //while (true) {
        //    __WFI();
        //}
        return errno;
    } else {
        printf(" done.\n");
        return 0;
    }
    
}