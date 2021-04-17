#include "mbed.h"
#include "arm_book_lib.h"

#include "motor.h"

//=====[Declaration of private defines]======================================

#define MOTOR_UPDATE_TIME 9

//=====[Declaration of private data types]=====================================

//=====[Declaration and initialization of public global objects]===============

DigitalInOut motorM1Pin(PF_2);
DigitalInOut motorM2Pin(PE_3);

//=====[Declaration of external public global variables]=======================

//=====[Declaration and initialization of public global variables]=============

MotorDirection_t motorDirection;
MotorDirection_t motorState;

//=====[Declaration and initialization of private global variables]============

//=====[Declarations (prototypes) of private functions]========================

//=====[Implementations of public functions]===================================

void motorControlInit()
{
    motorM1Pin.mode(OpenDrain);
    motorM2Pin.mode(OpenDrain);
    
    motorM1Pin.input();
    motorM2Pin.input();

    motorDirection = STOPPED;
    motorState = STOPPED;
}

MotorDirection_t motorDirectionRead()
{
    return motorDirection;
}

void motorDirectionWrite( MotorDirection_t Direction )
{
    motorDirection = Direction;
}

void motorControlUpdate()
{
    static int motorUpdateCounter = 0;
    
    motorUpdateCounter++;
    
    if ( motorUpdateCounter > MOTOR_UPDATE_TIME ) {
        
        motorUpdateCounter = 0;
        
        switch ( motorState ) {
            case DIRECTION_1:
                if ( motorDirection == DIRECTION_2 || 
                     motorDirection == STOPPED ) {
                    motorM1Pin.input();
                    motorM2Pin.input();
                    motorState = STOPPED;
                }
            break;
    
            case DIRECTION_2:
                if ( motorDirection == DIRECTION_1 || 
                     motorDirection == STOPPED ) {
                    motorM1Pin.input();
                    motorM2Pin.input();
                    motorState = STOPPED;
                }
            break;
    
            case STOPPED:
            default:
                if ( motorDirection == DIRECTION_1 ) {
                    motorM2Pin.input();
                    motorM1Pin.output();
                    motorM1Pin = LOW;
                    motorState = DIRECTION_1;
                }
                
                if ( motorDirection == DIRECTION_2 ) {
                    motorM1Pin.input();
                    motorM2Pin.output();
                    motorM2Pin = LOW;
                    motorState = DIRECTION_2;
                }
            break;
        }
    }        
}

//=====[Implementations of private functions]==================================
