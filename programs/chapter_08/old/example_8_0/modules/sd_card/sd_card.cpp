//=====[Libraries]=============================================================

#include "mbed.h"
#include "sd_card.h"
#include "FATFileSystem.h"
#include "SDBlockDevice.h"
#include <stdio.h>
#include <errno.h>
/* mbed_retarget.h is included after errno.h so symbols are mapped to
 * consistent values for all toolchains */
#include "platform/mbed_retarget.h"

//=====[Declaration of private defines]========================================

//=====[Declaration of private data types]=====================================

//=====[Declaration and initialization of public global objects]===============

SDBlockDevice sd(MBED_CONF_SD_SPI_MOSI,
                 MBED_CONF_SD_SPI_MISO,
                 MBED_CONF_SD_SPI_CLK,
                 D9);
                 
FATFileSystem fs("sd", &sd);

//=====[Declaration of external public global variables]=======================

//=====[Declaration and initialization of public global variables]=============

//=====[Declaration and initialization of private global variables]============

//=====[Declarations (prototypes) of private functions]========================

static void return_error(int ret_val);
static void errno_error(void *ret_val);

//=====[Implementations of public functions]===================================

void sdMount()
{
    int error = 0;
    printf("Welcome to the filesystem example.\n");

    // Try to mount the filesystem
    printf("Mounting the filesystem... ");

    error = fs.mount(&sd);
    if (error) {
        // Reformat if we can't mount the filesystem
        // this should only happen on the first boot
        printf("No filesystem found, formatting... ");
        error = fs.reformat(&sd);
        return_error(error);
    }
}

void sdWrite()
{
    printf("Opening a new file, numbers.txt.");
    FILE *fd = fopen("/sd/numbers.txt", "w+");
    errno_error(fd);
    for (int i = 0; i < 20; i++) {
        printf("Writing decimal numbers to a file (%d/20)\r", i);
        fprintf(fd, "%d\n", i);
    }
    printf("Writing decimal numbers to a file (20/20) done.\n");

    printf("Closing file.");
    fclose(fd);
    printf(" done.\n");
}

void sdRead()
{
    printf("Re-opening file read-only.");
    FILE *fd = fopen("/sd/numbers.txt", "r");
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
        while (true) {
            __WFI();
        }
    } else {
        printf("done.\n");
    }
}

void errno_error(void *ret_val)
{
    if (ret_val == NULL) {
        printf(" Failure. %d \n", errno);
        while (true) {
            __WFI();
        }
    } else {
        printf(" done.\n");
    }
}