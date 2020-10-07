//=====[Libraries]=============================================================

#include "smart_home_system.h"

char ssid[100];
char pass[100];

//=====[Main function, the program entry point after power on or reset]========

int main()
{
    smartHomeSystemInit();
    while (true) {
        smartHomeSystemUpdate();
    }
}