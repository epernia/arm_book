#ifndef _ARM_BOOK_LIBRARY_H_
#define _ARM_BOOK_LIBRARY_H_

/*==================[Libraries]=============================================*/

#include <cstdint>
#include <mbed.h> 

/*==================[macros]=================================================*/

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

/*==================[Function-like macros]===================================*/

#define delay(ms)      thread_sleep_for( ms )

/*==================[end of file]============================================*/

#endif // _ARM_BOOK_LIBRARY_H_