//=====[Libraries]=============================================================

#include "alarm.h"

//=====[Main function, the program entry point after power on or reset]========

int main()
{
    alarmInit();
    while (true) {
        alarmUpdate();
    }
}
