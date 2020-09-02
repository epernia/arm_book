//=====[Libraries]=============================================================

#include "mbed.h"
#include "arm_book_lib.h"
#include "sd_card.h"
#include "FATFileSystem.h"
#include "SDBlockDevice.h"
#include "event_log.h"
#include "date_and_time.h"
#include <stdio.h>
#include <errno.h>
/* mbed_retarget.h is included after errno.h so symbols are mapped to
 * consistent values for all toolchains */
#include "platform/mbed_retarget.h"

//=====[Declaration of private defines]========================================

//=====[Declaration of private data types]=====================================

//=====[Declaration and initialization of public global objects]===============

SPI spiB(D22, D25, D23);
SDBlockDevice sd( D22, D25, D23, D7 );
                 
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
    FILE *fd = fopen("/sd/log.txt", "w+");
    errno_error(fd);
    char str[100];
    int i;
    for (i = 0; i < eventLogNumberOfStoredEvents(); i++) {
        eventLogRead( i, str );
        printf("Writing log (%d/%d) to a file\r\n", i,eventLogNumberOfStoredEvents());
        fprintf(fd, "%s", str );                       
    }

    printf("Writing logs to a file done.\r\n");

    printf("Closing file.");
    fclose(fd);
    printf(" done.\r\n");
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