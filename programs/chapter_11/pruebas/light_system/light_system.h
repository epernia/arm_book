//=====[#include guards - begin]===============================================

#ifndef _LIGHT_SYSTEM_H_
#define _LIGHT_SYSTEM_H_

//=====[Libraries]=============================================================

//=====[Declaration of public defines]=======================================

//=====[Declaration of public data types]======================================

typedef enum {
    LED_RED,
    LED_GREEN,
    LED_BLUE,
} lightSystem_t;

//=====[Declarations (prototypes) of public functions]=========================

void lightSystemInit();
void lightSystemUpdate();

//=====[#include guards - end]=================================================

#endif // _LIGHT_SYSTEM_H_