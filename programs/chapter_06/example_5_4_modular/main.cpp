//=====[Libraries]=============================================================

#include "mbed.h"
#include "arm_book_lib.h"

//=====[Main function, the program entry point after power on or reset]========

int main()
{
    alarmInit();
    while (true) {
        alarmActivationUpdate();
        alarmDeactivationUpdate();
        uartTask();
        eventLogUpdate();
        delay(TIME_INCREMENT_MS);
    }
}