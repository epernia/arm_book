//=====[#include guards - begin]===============================================

#ifndef _EVENT_LOG_H_
#define _EVENT_LOG_H_

//=====[Libraries]=============================================================

//=====[Declaration of public constants]=======================================

//=====[Declaration of public data types]======================================

//=====[Declarations (prototypes) of public functions]=========================

void eventLogUpdate();
int eventLogNumberOfStoredEvents();
void eventLogReadStringAtIndex( int index, char* str );
void eventLogWrite( bool currentState, const char* elementName );

//=====[#include guards - end]=================================================

#endif // _EVENT_LOG_H_