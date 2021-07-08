//=====[#include guards - begin]===============================================

#ifndef _SD_CARD_H_
#define _SD_CARD_H_

#define SD_CARD_FILENAME_MAX_LENGTH 32

//=====[Declaration of public defines]=========================================

//=====[Declaration of public data types]======================================

//=====[Declarations (prototypes) of public functions]=========================

bool sdCardInit();
bool sdCardWriteFile( const char* fileName, const char* writeBuffer );
bool sdCardReadFile( const char* fileName, char * readBuffer );
bool sdCardListFiles( char* fileNamesBuffer, int fileNamesBufferSize );


//=====[#include guards - end]=================================================

#endif // _SD_CARD_H_
