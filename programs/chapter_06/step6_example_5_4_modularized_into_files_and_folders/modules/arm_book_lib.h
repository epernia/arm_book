/* Copyright 2020, Eric Pernia, Pablo Gomez and Ariel Lutemberg.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _ARM_BOOK_LIBRARY_H_
#define _ARM_BOOK_LIBRARY_H_

/*==================[inclusions]=============================================*/

#include <cstdint>
#include <mbed.h>

/*==================[c++]====================================================*/
#ifdef __cplusplus
extern "C" {
#endif

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

//  __I Defines 'read only' permissions: volatile const
//  __O Defines 'write only' permissions: volatile
// __IO Defines 'read / write' permissions: volatile

#define HW_REG_8_R(x)     (*((__I  uint8_t *)(x)))
#define HW_REG_16_R(x)    (*((__I uint16_t *)(x)))
#define HW_REG_32_R(x)    (*((__I uint32_t *)(x)))

#define HW_REG_8_W(x)     (*((__O  uint8_t *)(x)))
#define HW_REG_16_W(x)    (*((__O uint16_t *)(x)))
#define HW_REG_32_W(x)    (*((__O uint32_t *)(x)))

#define HW_REG_8_RW(x)    (*((__IO  uint8_t *)(x)))
#define HW_REG_16_RW(x)   (*((__IO uint16_t *)(x)))
#define HW_REG_32_RW(x)   (*((__IO uint32_t *)(x)))

// Example:
// #define REG_NAME   (HW_REG_32_RW(0x4544555))

/*==================[Function-like macros]===================================*/

#define delay(ms)      thread_sleep_for( ms )

/*==================[typedef]================================================*/

// ST Zio connector namings

// CN7
# define D16   PC_6
# define D17   PB_15
# define D18   PB_13
# define D19   PB_12
# define D20   PA_15
# define D21   PC_7
# define D22   PB_5
# define D23   PB_3
# define D24   PA_4
# define D25   PB_4

// CN10
# define D26   PB_6
# define D27   PB_2
# define D28   PD_13
# define D29   PD_12
# define D30   PD_11
# define D31   PE_2
# define D32   PA_0
# define D33   PB_0
# define D34   PE_0
# define D35   PB_11
# define D36   PB_10
# define D37   PE_15
# define D38   PE_14
# define D39   PE_12
# define D40   PE_10
# define D41   PE_7
# define D42   PE_8

// CN8
# define D43   PC_8
# define D44   PC_9
# define D45   PC_10
# define D46   PC_11
# define D47   PC_12
# define D48   PD_1
# define D49   PG_2
# define D50   PG_3

// CN9
# define D51   PD_7
# define D52   PD_6
# define D53   PD_5
# define D54   PD_4
# define D55   PD_3
# define D56   PE_2
# define D57   PE_4
# define D58   PE_5
# define D59   PE_6
# define D60   PE_3
# define D61   PF_8
# define D62   PF_7
# define D63   PF_9
# define D64   PG_1
# define D65   PG_0
# define D66   PD_1
# define D67   PD_0
# define D68   PF_0
# define D69   PF_1
# define D70   PF_2
# define D71   PA_7
# define D72   NC

/*==================[c++]====================================================*/
#ifdef __cplusplus
}
#endif

/*==================[end of file]============================================*/
#endif // _ARM_BOOK_LIBRARY_H_