#include "mbed.h"
#include "arm_book_lib.h"

#include "welcome_message.h"

#include "audio.h"

//=====[Declaration of private defines]========================================

#define AUDIO_SAMPLE_DURATION   125

//=====[Declaration of private data types]=====================================

//=====[Declaration and initialization of public global objects]===============

PwmOut audioOut(PE_6);

//=====[Declaration of external public global variables]=======================

//=====[Declaration and initialization of public global variables]=============

//=====[Declaration and initialization of private global variables]============

//=====[Declarations (prototypes) of private functions]========================

static void welcomeMessage();

//=====[Implementations of public functions]===================================

void audioInit()
{
    welcomeMessage(); 
	return;
}

//=====[Implementations of private functions]==================================

static void welcomeMessage()
{
    float audioDutyCycle = 0.0;

	audioOut.period(0.000025f);

    int i = 0;	
	for( i=1; i<welcomeMessageLength; i++ ) {
		audioDutyCycle = (float) welcomeMessageData[i]/255;
		audioOut.write(audioDutyCycle); 
		wait_us(AUDIO_SAMPLE_DURATION);
    }
	
	return;
}