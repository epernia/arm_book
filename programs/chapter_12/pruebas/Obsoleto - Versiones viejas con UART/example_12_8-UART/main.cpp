//=====[Libraries]=============================================================

#include "display.h"

//=====[Main function, the program entry point after power on or reset]========

int main()
{
    displayInit();
    while (true) {
        displayUpdate();
    }
}