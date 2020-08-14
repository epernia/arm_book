#include "mbed.h"
 
SPI spi(SPI_MOSI, SPI_MISO, SPI_SCK); // mosi, miso, sclk
DigitalOut cs(SPI_CS);
 
int main() {
// Select the device by setting chip select low
    cs = 0;
 
    // Send 0x8f, the command to read the WHOAMI register
    spi.write(0x8F);
 
    // Send a dummy byte to receive the contents of the WHOAMI register
    int whoami = spi.write(0x00);
    printf("WHOAMI register = 0x%X\n", whoami);
 
    // Deselect the device
    cs = 1;
}