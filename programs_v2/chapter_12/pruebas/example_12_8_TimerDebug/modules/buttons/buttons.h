//=====[#include guards - begin]===============================================

#ifndef _BUTTONS_H_
#define _BUTTONS_H_

//=====[Declaration of public defines]=========================================

#define HOW_OFTEN_INCREMENT   1
#define HOW_OFTEN_MIN         1
#define HOW_OFTEN_MAX         24
#define HOW_LONG_INCREMENT    10
#define HOW_LONG_MIN          0
#define HOW_LONG_MAX          90
#define MOISTURE_INCREMENT    5
#define MOISTURE_MIN          0
#define MOISTURE_MAX          95

//=====[Declaration of public data types]======================================

typedef struct buttonsStatus {
    bool changeMode;
    int howOften;
    int howLong;
    int moisture;
} buttonsStatus_t;

//=====[Declarations (prototypes) of public functions]=========================

void buttonsInit();
void buttonsUpdate();
buttonsStatus_t buttonsRead();

//=====[#include guards - end]=================================================

#endif // _BUTTONS_H_