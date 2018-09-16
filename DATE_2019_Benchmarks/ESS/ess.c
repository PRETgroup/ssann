/* sscc : C CODE OF SORTED EQUATIONS ess - INLINE MODE */

/* AUXILIARY DECLARATIONS */

#ifndef STRLEN
#define STRLEN 81
#endif
#define _COND(A,B,C) ((A)?(B):(C))
#ifdef TRACE_ACTION
#include <stdio.h>
#endif
#ifndef NULL
#define NULL ((char*)0)
#endif

#ifndef __EXEC_STATUS_H_LOADED
#define __EXEC_STATUS_H_LOADED

typedef struct {
unsigned int start:1;
unsigned int kill:1;
unsigned int active:1;
unsigned int suspended:1;
unsigned int prev_active:1;
unsigned int prev_suspended:1;
unsigned int exec_index;
unsigned int task_exec_index;
void (*pStart)();
void (*pRet)();
} __ExecStatus;

#endif
#define __ResetExecStatus(status) {\
   status.prev_active = status.active; \
   status.prev_suspended = status.suspended; \
   status.start = status.kill = status.active = status.suspended = 0; }
#define __DSZ(V) (--(V)<=0)
#define BASIC_TYPES_DEFINED
typedef int boolean;
typedef int integer;
typedef char* string;
#define _true 1
#define _false 0
#define __ess_GENERIC_TEST(TEST) return TEST;
typedef void (*__ess_APF)();
static __ess_APF *__ess_PActionArray;

#include "ess.h"

/* EXTERN DECLARATIONS */

#ifndef _NO_EXTERN_DEFINITIONS
#ifndef _NO_FUNCTION_DEFINITIONS
#ifndef _getFuturePrice_DEFINED
#ifndef getFuturePrice
extern float getFuturePrice();
#endif
#endif
#ifndef _getPreviousDemand_DEFINED
#ifndef getPreviousDemand
extern float getPreviousDemand();
#endif
#endif
#ifndef _getBatteryWatts_DEFINED
#ifndef getBatteryWatts
extern float getBatteryWatts(float);
#endif
#endif
#endif
#ifndef _NO_PROCEDURE_DEFINITIONS
#ifndef _runQLayer1_DEFINED
#ifndef runQLayer1
extern void runQLayer1(float ,float ,float ,float ,float);
#endif
#endif
#ifndef _runQLayer2_DEFINED
#ifndef runQLayer2
extern void runQLayer2();
#endif
#endif
#endif
#endif

/* INITIALIZED CONSTANTS */

/* MEMORY ALLOCATION */

static float __ess_V0;
static boolean __ess_V1;
static float __ess_V2;
static boolean __ess_V3;
static float __ess_V4;
static boolean __ess_V5;
static float __ess_V6;
static float __ess_V7;
static float __ess_V8;


/* INPUT FUNCTIONS */

void ess_I_price (float __V) {
__ess_V0 = __V;
__ess_V1 = _true;
}
void ess_I_demand (float __V) {
__ess_V2 = __V;
__ess_V3 = _true;
}
void ess_I_soc (float __V) {
__ess_V4 = __V;
__ess_V5 = _true;
}

/* ACTIONS */

/* PREDEFINED ACTIONS */

/* PRESENT SIGNAL TESTS */

#define __ess_A1 \
__ess_V1
#define __ess_A2 \
__ess_V3
#define __ess_A3 \
__ess_V5

/* OUTPUT ACTIONS */

#define __ess_A4 \
ess_O_watts(__ess_V6)

/* ASSIGNMENTS */

#define __ess_A5 \
__ess_V1 = _false
#define __ess_A6 \
__ess_V3 = _false
#define __ess_A7 \
__ess_V5 = _false
#define __ess_A8 \
__ess_V7 = getFuturePrice()
#define __ess_A9 \
__ess_V8 = getPreviousDemand()
#define __ess_A10 \
__ess_V6 = getBatteryWatts(__ess_V2)

/* PROCEDURE CALLS */

#define __ess_A11 \
runQLayer1(__ess_V7,__ess_V8,__ess_V0,__ess_V2,__ess_V4)
#define __ess_A12 \
runQLayer2()

/* CONDITIONS */

/* DECREMENTS */

/* START ACTIONS */

/* KILL ACTIONS */

/* SUSPEND ACTIONS */

/* ACTIVATE ACTIONS */

/* WRITE ARGS ACTIONS */

/* RESET ACTIONS */

#define __ess_A13 \

#define __ess_A14 \

#define __ess_A15 \

#define __ess_A16 \

#define __ess_A17 \

#define __ess_A18 \


/* ACTION SEQUENCES */

/* FUNCTIONS RETURNING NUMBER OF EXEC */

int ess_number_of_execs () {
return (0);
}


/* AUTOMATON (STATE ACTION-TREES) */



static void __ess__reset_input () {
__ess_V1 = _false;
__ess_V3 = _false;
__ess_V5 = _false;
}

/* REDEFINABLE BIT TYPE */

#ifndef __SSC_BIT_TYPE_DEFINED
typedef char __SSC_BIT_TYPE;
#endif

/* REGISTER VARIABLES */

static __SSC_BIT_TYPE __ess_R[4] = {_true,
 _false,
 _false,
 _false};

/* AUTOMATON ENGINE */

int ess () {
/* AUXILIARY VARIABLES */

static __SSC_BIT_TYPE E[9];
E[0] = __ess_R[0]&&!((
#ifdef TRACE_ACTION
fprintf(stderr, "test 1\n"),
#endif
__ess_A1));
if (E[0]) {
__ess_A13;
#ifdef TRACE_ACTION
fprintf(stderr, "__ess_A13\n");
#endif
}
E[1] = __ess_R[0]&&!((
#ifdef TRACE_ACTION
fprintf(stderr, "test 2\n"),
#endif
__ess_A2));
if (E[1]) {
__ess_A14;
#ifdef TRACE_ACTION
fprintf(stderr, "__ess_A14\n");
#endif
}
E[2] = __ess_R[0]&&!((
#ifdef TRACE_ACTION
fprintf(stderr, "test 3\n"),
#endif
__ess_A3));
if (E[2]) {
__ess_A15;
#ifdef TRACE_ACTION
fprintf(stderr, "__ess_A15\n");
#endif
}
if (__ess_R[0]) {
__ess_A16;
#ifdef TRACE_ACTION
fprintf(stderr, "__ess_A16\n");
#endif
}
E[3] = __ess_R[2]&&!(__ess_R[0]);
if (E[3]) {
__ess_A12;
#ifdef TRACE_ACTION
fprintf(stderr, "__ess_A12\n");
#endif
}
if (E[3]) {
__ess_A10;
#ifdef TRACE_ACTION
fprintf(stderr, "__ess_A10\n");
#endif
}
if (E[3]) {
__ess_A4;
#ifdef TRACE_ACTION
fprintf(stderr, "__ess_A4\n");
#endif
}
E[4] = !(_true);
if (__ess_R[0]) {
__ess_A17;
#ifdef TRACE_ACTION
fprintf(stderr, "__ess_A17\n");
#endif
}
if (__ess_R[0]) {
__ess_A18;
#ifdef TRACE_ACTION
fprintf(stderr, "__ess_A18\n");
#endif
}
E[5] = (__ess_R[3]&&!(__ess_R[0]))||__ess_R[0];
if (E[5]) {
__ess_A8;
#ifdef TRACE_ACTION
fprintf(stderr, "__ess_A8\n");
#endif
}
if (E[5]) {
__ess_A9;
#ifdef TRACE_ACTION
fprintf(stderr, "__ess_A9\n");
#endif
}
E[6] = __ess_R[1]&&!(__ess_R[0]);
if (E[6]) {
__ess_A11;
#ifdef TRACE_ACTION
fprintf(stderr, "__ess_A11\n");
#endif
}
E[7] = E[5]||E[6]||E[3];
E[8] = __ess_R[1]||__ess_R[2]||__ess_R[3];
__ess_R[0] = !(_true);
__ess_R[1] = E[5];
__ess_R[2] = E[6];
__ess_R[3] = E[3];
__ess__reset_input();
return E[7];
}

/* AUTOMATON RESET */

int ess_reset () {
__ess_R[0] = _true;
__ess_R[1] = _false;
__ess_R[2] = _false;
__ess_R[3] = _false;
__ess__reset_input();
return 0;
}
