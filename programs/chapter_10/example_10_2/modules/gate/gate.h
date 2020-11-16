//=====[#include guards - begin]===============================================

#ifndef _GATE_H_
#define _GATE_H_

//=====[Libraries]=============================================================

//=====[Declaration of public defines]=======================================

//=====[Declaration of public data types]======================================

typedef enum {
    GATE_CLOSE,
    GATE_OPEN,
    GATE_OPPENING,
    GATE_CLOSING,
} gateStatus_t;

//=====[Declarations (prototypes) of public functions]=========================

void gateInit();

void gateOpen();
void gateClose();

void gateLock();
void gateUnlock();

gateStatus_t gateStatus();

//=====[#include guards - end]=================================================

#endif // _GATE_H_
