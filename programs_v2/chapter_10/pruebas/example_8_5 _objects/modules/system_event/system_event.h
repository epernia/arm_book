//=====[#include guards - begin]===============================================

#ifndef _SYSTEM_EVENT_H_
#define _SYSTEM_EVENT_H_

//=====[Libraries]=============================================================

//=====[Declaration of public defines]=======================================

#define EVENT_LOG_NAME_MAX_LENGTH    15

//=====[Declaration of public data types]======================================

//=====[Declaration of public classes]======================================

class systemEvent {
    public:
        systemEvent(const char* eventLabel);
        void lastStateUpdate(bool state);
        bool lastStateRead( );
        char* getLabel( );
    private:
        char label[EVENT_LOG_NAME_MAX_LENGTH];
        bool lastState;
};

//=====[Declarations (prototypes) of public functions]=========================

//=====[#include guards - end]=================================================

#endif // _SYSTEM_EVENT_H_
