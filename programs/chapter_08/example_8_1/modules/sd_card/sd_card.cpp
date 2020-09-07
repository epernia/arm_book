//=====[Libraries]=============================================================

#include "mbed.h"
#include "arm_book_lib.h"

#include "sd_card.h"

#include "FATFileSystem.h"
#include "SDBlockDevice.h"
#include <stdio.h>
#include <errno.h>
// mbed_retarget.h is included after errno.h so symbols are mapped to
// consistent values for all toolchains
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

/*SDBlockDevice sd(MBED_CONF_SD_SPI_MOSI,
                 MBED_CONF_SD_SPI_MISO,
                 MBED_CONF_SD_SPI_CLK,
                 MBED_CONF_SD_SPI_CS);*/

// Use SPI_3
SDBlockDevice sd( SPI3_MOSI, SPI3_MISO, SPI3_SCK, SPI3_CS );

FATFileSystem fs("sd", &sd);

//=====[Declaration of external public global variables]=======================

//=====[Declaration and initialization of public global variables]=============

//=====[Declaration and initialization of private global variables]============

//=====[Declarations (prototypes) of private functions]========================

static void return_error(int ret_val);
static void errno_error(void *ret_val);

//=====[Implementations of public functions]===================================

void sdCardInit()
{
    int error = 0;

    pcSerialComStringWrite("Mounting the filesystem... \r\n");
    error = fs.mount(&sd);
    
    if (error) {
        // Reformat if we can't mount the filesystem
        // this should only happen on the first boot
        pcSerialComStringWrite("No filesystem found, formatting... \r\n");
        error = fs.reformat(&sd);
        return_error(error);
    }
}

void sdMount()
{
    int error = 0;
    pcSerialComStringWrite("Welcome to the filesystem example.\r\n");

    // Try to mount the filesystem
    printf("Mounting the filesystem... \r\n");

    error = fs.mount(&sd);
    if (error) {
        // Reformat if we can't mount the filesystem
        // this should only happen on the first boot
        printf("No filesystem found, formatting... \r\n");
        error = fs.reformat(&sd);
        return_error(error);
    }
}

void sdWrite()
{

    char fileName[40];
    char eventStr[100];
    int i;
    time_t seconds;

    strncat( fileName, "log | ", strlen("log | ") );

    strncat( fileName, ctime(&seconds),
             strlen(ctime(&seconds)) );

    strncat( fileName, ".txt", strlen(".txt") );

    pcSerialComStringWrite("Creating log file...\r\n");

    FILE *fd = fopen(fileName, "w+");

    errno_error(fd);

    for (i = 0; i < eventLogNumberOfStoredEvents(); i++) {
        eventLogRead( i, eventStr );
        fprintf(fd, "%s", eventStr );                       
    }

    pcSerialComStringWrite("Writing logs to file done.\r\n");

    pcSerialComStringWrite("Closing file.\r\n");
    fclose(fd);
    pcSerialComStringWrite(" done.\r\n");
}

void sdRead()
{
    printf("Re-opening file read-only.");
    FILE *fd = fopen("/sd/log.txt", "r");
    errno_error(fd);

    printf("Dumping file to screen.\n");
    char buff[16] = {0};
    while (!feof(fd)) {
        int size = fread(&buff[0], 1, 15, fd);
        fwrite(&buff[0], 1, size, stdout);
    }
    printf("EOF.\n");

    printf("Closing file.");
    fclose(fd);
    printf(" done.\n");
}

void sdDir()
{
    int error = 0;
    printf("Opening root directory.");
    DIR *dir = opendir("/sd/");
    

    struct dirent *de;
    printf("Printing all filenames:\n");
    while ((de = readdir(dir)) != NULL) {
        printf("  %s\n", &(de->d_name)[0]);
    }

    printf("Closing root directory. ");
    error = closedir(dir);
    return_error(error);
    printf("Filesystem Demo complete.\n");
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

void errno_error(void *ret_val)
{
    if (ret_val == NULL) {
        printf(" Failure. %d \n", errno);
        //while (true) {
        //    __WFI();
        //}
    } else {
        printf(" done.\n");
    }
}