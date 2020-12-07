//=====[#include guards - begin]===============================================

#ifndef _LIGHT_SYSTEM_H_
#define _LIGHT_SYSTEM_H_

//=====[Libraries]=============================================================

//=====[Declaration of public defines]=======================================

//=====[Declaration of public data types]======================================

typedef enum {
    LED_RGB_RED,
    LED_RGB_GREEN,
    LED_RGB_BLUE,
} lightSystem_t;

//=====[Declarations (prototypes) of public functions]=========================

void lightSystemInit();
void lightSystemUpdate();

//=====[#include guards - end]=================================================

#endif // _LIGHT_SYSTEM_H_