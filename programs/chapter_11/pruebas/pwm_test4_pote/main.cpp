//=====[Libraries]=============================================================

#include "bright_system.h"

//=====[Main function, the program entry point after power on or reset]========

int main()
{
    brightSystemInit();
    while (true) {
        brightSystemUpdate();
    }	
}