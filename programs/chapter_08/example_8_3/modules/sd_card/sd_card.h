//=====[#include guards - begin]===============================================

#ifndef _SD_CARD_H_
#define _SD_CARD_H_

//=====[Libraries]=============================================================

//=====[Declaration of public defines]=======================================

//=====[Declaration of public data types]======================================

//=====[Declarations (prototypes) of public functions]=========================

bool sdCardInit();
bool sdCardWriteFile( const char* fileName, const char* writeBuffer );
bool sdCardReadFile( const char * fileName, char * readBuffer );
bool sdCardListFiles();

//=====[#include guards - end]=================================================

#endif // _SD_CARD_H_
