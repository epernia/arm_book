//=====[#include guards - begin]===============================================

#ifndef _BRIGHT_CONTROL_H_
#define _BRIGHT_CONTROL_H_

//=====[Libraries]=============================================================

//=====[Declaration of public defines]=======================================

#define TO_SECONDS   10
#define TO_HOURS     36000

//=====[Declaration of public data types]======================================

typedef enum {
    INITIAL_MODE_ASSESSMENT, 
    CONTINUOUS_MODE_IRRIGATING, 
    PROGRAMMED_MODE_WAITING_TO_IRRIGATE, 
    PROGRAMMED_MODE_IRRIGATION_SKIPPED,
    PROGRAMMED_MODE_IRRIGATING
} irrigationState_t;
	
typedef struct irrigationControlStatus {
    irrigationState_t irrigationState;
    bool waitedTimeMustBeReset;
    bool irrigatedTimeMustBeReset;
} irrigationControlStatus_t;

//=====[Declarations (prototypes) of public functions]=========================

void brightControlInit();
void brightControlUpdate();

//=====[#include guards - end]=================================================

#endif // _BRIGHT_CONTROL_H_