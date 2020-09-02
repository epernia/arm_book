#include "mbed.h"
#include "SDFileSystem.h"
 
SDFileSystem sd(D11, D12, D13, D9, "sd"); 
 //mosi,miso,sclk,cs,name)
int main() {
    printf("Hello World!\n");   
 
    mkdir("/sd/mydir", 0777);
    
    FILE *fp = fopen("/sd/mydir/sdtest.txt", "w");
    if(fp == NULL) {
        error("Could not open file for write\n");
    }
    fprintf(fp, "Hello fun SD Card World!");
    fclose(fp); 
 
    printf("Goodbye World!\n");
}