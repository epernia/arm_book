//=====[#include guards - begin]===============================================

#ifndef _SD_CARD_H_
#define _SD_CARD_H_

//=====[Libraries]=============================================================

//=====[Declaration of public defines]=======================================

//=====[Declaration of public data types]======================================

//=====[Declarations (prototypes) of public functions]=========================

bool sdCardInit();
void sdMount();
bool sdCardWriteFile( const char* fileName, const char* writeBuffer );
void sdCardReadFile( const char * fileName, const char * readBuffer );
bool sdCardListFiles();
void sdDir();

//=====[#include guards - end]=================================================

#endif // _SD_CARD_H_
