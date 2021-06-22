//=====[#include guards - begin]===============================================

#ifndef _LIGHT_SYSTEM_H_
#define _LIGHT_SYSTEM_H_

//=====[Declaration of public defines]=========================================

//=====[Declaration of public data types]======================================

typedef enum {
    RGB_LED_RED,
    RGB_LED_GREEN,
    RGB_LED_BLUE,
} lightSystem_t;

//=====[Declarations (prototypes) of public functions]=========================

void lightSystemInit();
void lightSystemUpdate();
void lightSystemBrightnessChangeEnable( lightSystem_t light, bool state );

//=====[#include guards - end]=================================================

#endif // _LIGHT_SYSTEM_H_