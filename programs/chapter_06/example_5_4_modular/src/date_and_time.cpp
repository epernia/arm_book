//=====[Libraries]=============================================================

#include "date_and_time.h"

//=====[Declaration of private data types]=====================================



//=====[Declaration of private constants]======================================



//=====[Declaration of external public global objects]=========================



//=====[Declaration and intitalization of public global objects]===============



//=====[Declaration and intitalization of private global objects]==============



//=====[Declaration of external public global variables]=======================



//=====[Declaration and intitalization of public global variables]=============



//=====[Declaration and intitalization of private global variables]============



//=====[Declarations (prototypes) of private functions]========================



//=====[Implementations of public functions]===================================

char* dateAndTimeStringGet()
{
    time_t epochSeconds;
    epochSeconds = time(NULL);
    return ctime(&epochSeconds);    
}

void dateAndTimeIndividualIntsSet( int year, int month, int day, 
                                   int hour, int minute, int second )
{
    struct tm rtcTime;

    rtcTime.tm_year = year - 1900;
    rtcTime.tm_mon  = month - 1;
    rtcTime.tm_mday = day;
    rtcTime.tm_hour = hour;
    rtcTime.tm_min  = minute;
    rtcTime.tm_sec  = second;

    rtcTime.tm_isdst = -1;

    set_time( mktime( &rtcTime ) );
}

//=====[Implementations of private functions]==================================

