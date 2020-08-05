//=====[#include guards - begin]===============================================

#ifndef _USER_INTERFACE_H_
#define _USER_INTERFACE_H_

//=====[Libraries]=============================================================

//=====[Declaration of public constants]=======================================



//=====[Declaration of public data types]======================================

//=====[Declarations (prototypes) of public functions]=========================

void userInterfaceInit();
void userInterfaceUpdate();
void userInterfaceMatrixKeypadUpdate();

bool incorrectCodeStateRead();
void incorrectCodeStateWrite( bool state );
void incorrectCodeIndicatorUpdate();

bool systemBlockedStateRead();
void systemBlockedStateWrite( bool state );
void systemBlockedIndicatorUpdate();

//=====[#include guards - end]=================================================

#endif // _USER_INTERFACE_H_