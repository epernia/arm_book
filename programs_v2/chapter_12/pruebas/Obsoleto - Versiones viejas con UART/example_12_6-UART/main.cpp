#include "irrigation_system.h"

//=====[Main function, the program entry point after power on or reset]========

int main()
{
    irrigationSystemInit();
    while (true) {
        irrigationSystemUpdate();
    }
}