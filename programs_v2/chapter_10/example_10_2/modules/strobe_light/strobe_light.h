//=====[#include guards - begin]===============================================

#ifndef _STROBE_LIGHT_H_
#define _STROBE_LIGHT_H_

//=====[Declaration of public defines]=========================================

//=====[Declaration of public data types]======================================

//=====[Declarations (prototypes) of public functions]=========================

void strobeLightInit();
bool strobeLightStateRead();
void strobeLightStateWrite( bool state );
void strobeLightUpdate( int strobeTime );

//=====[#include guards - end]=================================================

#endif // _STROBE_LIGHT_H_