//=====[#include guards - begin]===============================================

#ifndef _SYSTEM_EVENT_H_
#define _SYSTEM_EVENT_H_

//=====[Libraries]=============================================================

#include "event_log.h"

//=====[Declaration of public defines]=========================================

//=====[Declaration of public data types]======================================

//=====[Declaration of public classes]=========================================

class systemEvent {
    public:
        systemEvent(const char* eventLabel);
        void stateUpdate( bool state );
        bool lastStateRead( );
        char* getLabel( );  
    private:
        void lastStateUpdate(bool state);
        char label[EVENT_LOG_NAME_MAX_LENGTH];
        bool lastState;
};


//=====[Declarations (prototypes) of public functions]=========================

//=====[#include guards - end]=================================================

#endif // _SYSTEM_EVENT_H_
