//=====[#include guards - begin]===============================================

#ifndef _SD_CARD_H_
#define _SD_CARD_H_

//=====[Libraries]=============================================================

//=====[Declaration of public defines]=======================================

//=====[Declaration of public data types]======================================

//=====[Declarations (prototypes) of public functions]=========================

void sdCardInit();
void sdMount();
void sdWrite();
void sdRead();
void sdReadFile( char * fileName);
void sdDir();

//=====[#include guards - end]=================================================

#endif // _SD_CARD_H_
