/* sscc : C CODE OF SORTED EQUATIONS playgame - INLINE MODE */

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
#define __playgame_GENERIC_TEST(TEST) return TEST;
typedef void (*__playgame_APF)();
static __playgame_APF *__playgame_PActionArray;

#include "playgame.h"

/* EXTERN DECLARATIONS */

#ifndef _NO_EXTERN_DEFINITIONS
#ifndef _NO_PROCEDURE_DEFINITIONS
#ifndef _rabbit_decide_DEFINED
#ifndef rabbit_decide
extern void rabbit_decide(integer* ,integer*);
#endif
#endif
#ifndef _wolf1_decide_DEFINED
#ifndef wolf1_decide
extern void wolf1_decide(integer* ,integer*);
#endif
#endif
#ifndef _wolf2_decide_DEFINED
#ifndef wolf2_decide
extern void wolf2_decide(integer* ,integer*);
#endif
#endif
#endif
#endif

/* INITIALIZED CONSTANTS */

/* MEMORY ALLOCATION */

static boolean __playgame_V0;
static integer __playgame_V1;
static integer __playgame_V2;
static integer __playgame_V3;
static integer __playgame_V4;
static integer __playgame_V5;
static integer __playgame_V6;
static integer __playgame_V7;
static integer __playgame_V8;
static integer __playgame_V9;
static integer __playgame_V10;
static integer __playgame_V11;
static integer __playgame_V12;


/* INPUT FUNCTIONS */

void playgame_I_decide () {
__playgame_V0 = _true;
}

/* ACTIONS */

/* PREDEFINED ACTIONS */

/* PRESENT SIGNAL TESTS */

#define __playgame_A1 \
__playgame_V0

/* OUTPUT ACTIONS */

#define __playgame_A2 \
playgame_O_rx(__playgame_V1)
#define __playgame_A3 \
playgame_O_ry(__playgame_V2)
#define __playgame_A4 \
playgame_O_w1x(__playgame_V3)
#define __playgame_A5 \
playgame_O_w1y(__playgame_V4)
#define __playgame_A6 \
playgame_O_w2x(__playgame_V5)
#define __playgame_A7 \
playgame_O_w2y(__playgame_V6)

/* ASSIGNMENTS */

#define __playgame_A8 \
__playgame_V0 = _false
#define __playgame_A9 \
__playgame_V1 = __playgame_V7
#define __playgame_A10 \
__playgame_V2 = __playgame_V8
#define __playgame_A11 \
__playgame_V3 = __playgame_V9
#define __playgame_A12 \
__playgame_V4 = __playgame_V10
#define __playgame_A13 \
__playgame_V5 = __playgame_V11
#define __playgame_A14 \
__playgame_V6 = __playgame_V12

/* PROCEDURE CALLS */

#define __playgame_A15 \
rabbit_decide(&__playgame_V7,&__playgame_V8)
#define __playgame_A16 \
wolf1_decide(&__playgame_V9,&__playgame_V10)
#define __playgame_A17 \
wolf2_decide(&__playgame_V11,&__playgame_V12)

/* CONDITIONS */

/* DECREMENTS */

/* START ACTIONS */

/* KILL ACTIONS */

/* SUSPEND ACTIONS */

/* ACTIVATE ACTIONS */

/* WRITE ARGS ACTIONS */

/* RESET ACTIONS */

#define __playgame_A18 \

#define __playgame_A19 \

#define __playgame_A20 \

#define __playgame_A21 \

#define __playgame_A22 \

#define __playgame_A23 \

#define __playgame_A24 \

#define __playgame_A25 \

#define __playgame_A26 \

#define __playgame_A27 \

#define __playgame_A28 \

#define __playgame_A29 \


/* ACTION SEQUENCES */

/* FUNCTIONS RETURNING NUMBER OF EXEC */

int playgame_number_of_execs () {
return (0);
}


/* AUTOMATON (STATE ACTION-TREES) */



static void __playgame__reset_input () {
__playgame_V0 = _false;
}

/* REDEFINABLE BIT TYPE */

#ifndef __SSC_BIT_TYPE_DEFINED
typedef char __SSC_BIT_TYPE;
#endif

/* REGISTER VARIABLES */

static __SSC_BIT_TYPE __playgame_R[2] = {_true,
 _false};

/* AUTOMATON ENGINE */

int playgame () {
/* AUXILIARY VARIABLES */

static __SSC_BIT_TYPE E[4];
if (__playgame_R[0]) {
__playgame_A18;
#ifdef TRACE_ACTION
fprintf(stderr, "__playgame_A18\n");
#endif
}
E[0] = __playgame_R[1]&&!(__playgame_R[0]);
E[1] = E[0]&&(
#ifdef TRACE_ACTION
fprintf(stderr, "test 1\n"),
#endif
__playgame_A1);
if (E[1]) {
__playgame_A15;
#ifdef TRACE_ACTION
fprintf(stderr, "__playgame_A15\n");
#endif
}
if (E[1]) {
__playgame_A16;
#ifdef TRACE_ACTION
fprintf(stderr, "__playgame_A16\n");
#endif
}
if (E[1]) {
__playgame_A17;
#ifdef TRACE_ACTION
fprintf(stderr, "__playgame_A17\n");
#endif
}
if (E[1]) {
__playgame_A9;
#ifdef TRACE_ACTION
fprintf(stderr, "__playgame_A9\n");
#endif
}
if (E[1]) {
__playgame_A2;
#ifdef TRACE_ACTION
fprintf(stderr, "__playgame_A2\n");
#endif
}
if (__playgame_R[0]) {
__playgame_A19;
#ifdef TRACE_ACTION
fprintf(stderr, "__playgame_A19\n");
#endif
}
if (E[1]) {
__playgame_A10;
#ifdef TRACE_ACTION
fprintf(stderr, "__playgame_A10\n");
#endif
}
if (E[1]) {
__playgame_A3;
#ifdef TRACE_ACTION
fprintf(stderr, "__playgame_A3\n");
#endif
}
if (__playgame_R[0]) {
__playgame_A20;
#ifdef TRACE_ACTION
fprintf(stderr, "__playgame_A20\n");
#endif
}
if (E[1]) {
__playgame_A11;
#ifdef TRACE_ACTION
fprintf(stderr, "__playgame_A11\n");
#endif
}
if (E[1]) {
__playgame_A4;
#ifdef TRACE_ACTION
fprintf(stderr, "__playgame_A4\n");
#endif
}
if (__playgame_R[0]) {
__playgame_A21;
#ifdef TRACE_ACTION
fprintf(stderr, "__playgame_A21\n");
#endif
}
if (E[1]) {
__playgame_A12;
#ifdef TRACE_ACTION
fprintf(stderr, "__playgame_A12\n");
#endif
}
if (E[1]) {
__playgame_A5;
#ifdef TRACE_ACTION
fprintf(stderr, "__playgame_A5\n");
#endif
}
if (__playgame_R[0]) {
__playgame_A22;
#ifdef TRACE_ACTION
fprintf(stderr, "__playgame_A22\n");
#endif
}
if (E[1]) {
__playgame_A13;
#ifdef TRACE_ACTION
fprintf(stderr, "__playgame_A13\n");
#endif
}
if (E[1]) {
__playgame_A6;
#ifdef TRACE_ACTION
fprintf(stderr, "__playgame_A6\n");
#endif
}
if (__playgame_R[0]) {
__playgame_A23;
#ifdef TRACE_ACTION
fprintf(stderr, "__playgame_A23\n");
#endif
}
if (E[1]) {
__playgame_A14;
#ifdef TRACE_ACTION
fprintf(stderr, "__playgame_A14\n");
#endif
}
if (E[1]) {
__playgame_A7;
#ifdef TRACE_ACTION
fprintf(stderr, "__playgame_A7\n");
#endif
}
E[2] = !(_true);
E[3] = __playgame_R[0]||E[1];
if (E[3]) {
__playgame_A24;
#ifdef TRACE_ACTION
fprintf(stderr, "__playgame_A24\n");
#endif
}
if (E[3]) {
__playgame_A25;
#ifdef TRACE_ACTION
fprintf(stderr, "__playgame_A25\n");
#endif
}
if (E[3]) {
__playgame_A26;
#ifdef TRACE_ACTION
fprintf(stderr, "__playgame_A26\n");
#endif
}
if (E[3]) {
__playgame_A27;
#ifdef TRACE_ACTION
fprintf(stderr, "__playgame_A27\n");
#endif
}
if (E[3]) {
__playgame_A28;
#ifdef TRACE_ACTION
fprintf(stderr, "__playgame_A28\n");
#endif
}
if (E[3]) {
__playgame_A29;
#ifdef TRACE_ACTION
fprintf(stderr, "__playgame_A29\n");
#endif
}
E[0] = E[0]&&!((
#ifdef TRACE_ACTION
fprintf(stderr, "test 1\n"),
#endif
__playgame_A1));
__playgame_R[1] = E[3]||(__playgame_R[1]&&E[0]);
E[0] = __playgame_R[1];
__playgame_R[0] = !(_true);
__playgame__reset_input();
return E[0];
}

/* AUTOMATON RESET */

int playgame_reset () {
__playgame_R[0] = _true;
__playgame_R[1] = _false;
__playgame__reset_input();
return 0;
}
