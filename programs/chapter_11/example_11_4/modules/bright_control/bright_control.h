//=====[#include guards - begin]===============================================

#ifndef _BRIGHT_CONTROL_H_
#define _BRIGHT_CONTROL_H_

//=====[Libraries]=============================================================

#include "light_system.h"

//=====[Declaration of public defines]=======================================

//=====[Declaration of public data types]======================================

//=====[Declarations (prototypes) of public functions]=========================

void brighControlInit();
void setPeriod( lightSystem_t light, float period );
void setDutyCycle( lightSystem_t light, float dutyCycle );

//=====[#include guards - end]=================================================

#endif // _BRIGHT_CONTROL_H_