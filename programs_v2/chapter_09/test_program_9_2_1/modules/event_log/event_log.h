//=====[#include guards - begin]===============================================

#ifndef _EVENT_LOG_H_
#define _EVENT_LOG_H_

//=====[Declaration of public defines]=========================================

#define EVENT_LOG_MAX_STORAGE       100
#define EVENT_LOG_NAME_MAX_LENGTH    13
#define EVENT_HEAD_STR_LENGTH         9
#define NEW_LINE_STR_LENGTH           4
#define DATE_AND_TIME_STR_LENGTH     17
#define CTIME_STR_LENGTH             26
#define EVENT_STR_LENGTH              EVENT_HEAD_STR_LENGTH + \
                                      EVENT_LOG_NAME_MAX_LENGTH + \
                                      NEW_LINE_STR_LENGTH  + \
                                      DATE_AND_TIME_STR_LENGTH + \
                                      CTIME_STR_LENGTH

//=====[Declaration of public data types]======================================

//=====[Declarations (prototypes) of public functions]=========================

void eventLogUpdate();
int eventLogNumberOfStoredEvents();
void eventLogRead( int index, char* str );
void eventLogWrite( bool currentState, const char* elementName );
bool eventLogSaveToSdCard();

//=====[#include guards - end]=================================================

#endif // _EVENT_LOG_H_
