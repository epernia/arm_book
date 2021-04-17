//=====[#include guards - begin]===============================================

#ifndef _TIMER_H_
#define _TIMER_H_

//=====[Declaration of public defines]=======================================

//=====[Declaration of public data types]======================================

typedef struct irrigationTimer {
    int waitedTime;
    int irrigatedTime;
} irrigationTimer_t;

//=====[Declarations (prototypes) of public functions]=========================

void irrigationTimerInit();
void irrigationTimerUpdate();
irrigationTimer_t irrigationTimerRead();

//=====[#include guards - end]=================================================

#endif // _TIMER_H_