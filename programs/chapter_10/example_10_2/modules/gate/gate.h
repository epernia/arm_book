//=====[#include guards - begin]===============================================

#ifndef _GATE_H_
#define _GATE_H_

//=====[Libraries]=============================================================

//=====[Declaration of public defines]=======================================

//=====[Declaration of public data types]======================================

typedef enum {
    GATE_CLOSED,
    GATE_OPEN,
    GATE_OPENING,
    GATE_CLOSING,
} gateStatus_t;

//=====[Declarations (prototypes) of public functions]=========================

void gateInit();

void gateOpen();
void gateClose();

void gateLock();
void gateUnlock();

gateStatus_t gateStatusRead();

//=====[#include guards - end]=================================================

#endif // _GATE_H_
