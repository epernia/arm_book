//=====[#include guards - begin]===============================================

#ifndef _ARM_BOOK_LIB_H_
#define _ARM_BOOK_LIB_H_

//=====[Libraries]=============================================================

#include <cstdint>
#include <mbed.h> 

//=====[Declaration of public defines]=========================================

// Functional states
#ifndef OFF
#define OFF    0
#endif
#ifndef ON
#define ON     (!OFF)
#endif

// Electrical states
#ifndef LOW
#define LOW    0
#endif
#ifndef HIGH
#define HIGH   (!LOW)
#endif

// Logical states
#ifndef FALSE
#define FALSE  0
#endif
#ifndef TRUE
#define TRUE   (!FALSE)
#endif

#ifndef false
#define false  0
#endif
#ifndef true
#define true   (!false)
#endif

#define delay(ms)      thread_sleep_for( ms )

//=====[#include guards - end]=================================================

#endif // _ARM_BOOK_LIB_H_