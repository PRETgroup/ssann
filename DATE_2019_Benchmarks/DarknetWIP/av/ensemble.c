/* sscc : C CODE OF SORTED EQUATIONS ensemble - INLINE MODE */

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
#define __ensemble_GENERIC_TEST(TEST) return TEST;
typedef void (*__ensemble_APF)();
static __ensemble_APF *__ensemble_PActionArray;

#include "ensemble.h"

/* EXTERN DECLARATIONS */

#ifndef _NO_EXTERN_DEFINITIONS
#ifndef _NO_PROCEDURE_DEFINITIONS
#ifndef _run_cnn_DEFINED
#ifndef run_cnn
extern void run_cnn(integer ,integer);
#endif
#endif
#ifndef _run_ensemble_DEFINED
#ifndef run_ensemble
extern void run_ensemble(integer);
#endif
#endif
#endif
#endif

/* INITIALIZED CONSTANTS */

/* MEMORY ALLOCATION */

static boolean __ensemble_V0;


/* INPUT FUNCTIONS */

void ensemble_I_start () {
__ensemble_V0 = _true;
}

/* ACTIONS */

/* PREDEFINED ACTIONS */

/* PRESENT SIGNAL TESTS */

#define __ensemble_A1 \
__ensemble_V0

/* OUTPUT ACTIONS */

/* ASSIGNMENTS */

#define __ensemble_A2 \
__ensemble_V0 = _false

/* PROCEDURE CALLS */

#define __ensemble_A3 \
run_cnn(0,0)
#define __ensemble_A4 \
run_cnn(0,1)
#define __ensemble_A5 \
run_cnn(0,2)
#define __ensemble_A6 \
run_ensemble(0)
#define __ensemble_A7 \
run_cnn(1,0)
#define __ensemble_A8 \
run_cnn(1,1)
#define __ensemble_A9 \
run_cnn(1,2)
#define __ensemble_A10 \
run_ensemble(1)
#define __ensemble_A11 \
run_cnn(2,0)
#define __ensemble_A12 \
run_cnn(2,1)
#define __ensemble_A13 \
run_cnn(2,2)
#define __ensemble_A14 \
run_ensemble(2)
#define __ensemble_A15 \
run_cnn(3,0)
#define __ensemble_A16 \
run_cnn(3,1)
#define __ensemble_A17 \
run_cnn(3,2)
#define __ensemble_A18 \
run_ensemble(3)
#define __ensemble_A19 \
run_cnn(4,0)
#define __ensemble_A20 \
run_cnn(4,1)
#define __ensemble_A21 \
run_cnn(4,2)
#define __ensemble_A22 \
run_ensemble(4)

/* CONDITIONS */

/* DECREMENTS */

/* START ACTIONS */

/* KILL ACTIONS */

/* SUSPEND ACTIONS */

/* ACTIVATE ACTIONS */

/* WRITE ARGS ACTIONS */

/* RESET ACTIONS */

/* ACTION SEQUENCES */

/* FUNCTIONS RETURNING NUMBER OF EXEC */

int ensemble_number_of_execs () {
return (0);
}


/* AUTOMATON (STATE ACTION-TREES) */



static void __ensemble__reset_input () {
__ensemble_V0 = _false;
}

/* REDEFINABLE BIT TYPE */

#ifndef __SSC_BIT_TYPE_DEFINED
typedef char __SSC_BIT_TYPE;
#endif

/* REGISTER VARIABLES */

static __SSC_BIT_TYPE __ensemble_R[2] = {_true,
 _false};

/* AUTOMATON ENGINE */

int ensemble () {
/* AUXILIARY VARIABLES */

static __SSC_BIT_TYPE E[3];
E[0] = !(_true);
E[1] = __ensemble_R[1]&&!(__ensemble_R[0]);
E[2] = E[1]&&(
#ifdef TRACE_ACTION
fprintf(stderr, "test 1\n"),
#endif
__ensemble_A1);
if (E[2]) {
__ensemble_A3;
#ifdef TRACE_ACTION
fprintf(stderr, "__ensemble_A3\n");
#endif
}
if (E[2]) {
__ensemble_A4;
#ifdef TRACE_ACTION
fprintf(stderr, "__ensemble_A4\n");
#endif
}
if (E[2]) {
__ensemble_A5;
#ifdef TRACE_ACTION
fprintf(stderr, "__ensemble_A5\n");
#endif
}
if (E[2]) {
__ensemble_A6;
#ifdef TRACE_ACTION
fprintf(stderr, "__ensemble_A6\n");
#endif
}
if (E[2]) {
__ensemble_A7;
#ifdef TRACE_ACTION
fprintf(stderr, "__ensemble_A7\n");
#endif
}
if (E[2]) {
__ensemble_A8;
#ifdef TRACE_ACTION
fprintf(stderr, "__ensemble_A8\n");
#endif
}
if (E[2]) {
__ensemble_A9;
#ifdef TRACE_ACTION
fprintf(stderr, "__ensemble_A9\n");
#endif
}
if (E[2]) {
__ensemble_A10;
#ifdef TRACE_ACTION
fprintf(stderr, "__ensemble_A10\n");
#endif
}
if (E[2]) {
__ensemble_A11;
#ifdef TRACE_ACTION
fprintf(stderr, "__ensemble_A11\n");
#endif
}
if (E[2]) {
__ensemble_A12;
#ifdef TRACE_ACTION
fprintf(stderr, "__ensemble_A12\n");
#endif
}
if (E[2]) {
__ensemble_A13;
#ifdef TRACE_ACTION
fprintf(stderr, "__ensemble_A13\n");
#endif
}
if (E[2]) {
__ensemble_A14;
#ifdef TRACE_ACTION
fprintf(stderr, "__ensemble_A14\n");
#endif
}
if (E[2]) {
__ensemble_A15;
#ifdef TRACE_ACTION
fprintf(stderr, "__ensemble_A15\n");
#endif
}
if (E[2]) {
__ensemble_A16;
#ifdef TRACE_ACTION
fprintf(stderr, "__ensemble_A16\n");
#endif
}
if (E[2]) {
__ensemble_A17;
#ifdef TRACE_ACTION
fprintf(stderr, "__ensemble_A17\n");
#endif
}
if (E[2]) {
__ensemble_A18;
#ifdef TRACE_ACTION
fprintf(stderr, "__ensemble_A18\n");
#endif
}
if (E[2]) {
__ensemble_A19;
#ifdef TRACE_ACTION
fprintf(stderr, "__ensemble_A19\n");
#endif
}
if (E[2]) {
__ensemble_A20;
#ifdef TRACE_ACTION
fprintf(stderr, "__ensemble_A20\n");
#endif
}
if (E[2]) {
__ensemble_A21;
#ifdef TRACE_ACTION
fprintf(stderr, "__ensemble_A21\n");
#endif
}
if (E[2]) {
__ensemble_A22;
#ifdef TRACE_ACTION
fprintf(stderr, "__ensemble_A22\n");
#endif
}
E[1] = E[1]&&!((
#ifdef TRACE_ACTION
fprintf(stderr, "test 1\n"),
#endif
__ensemble_A1));
__ensemble_R[1] = __ensemble_R[0]||E[2]||(__ensemble_R[1]&&E[1]);
E[1] = __ensemble_R[1];
__ensemble_R[0] = !(_true);
__ensemble__reset_input();
return E[1];
}

/* AUTOMATON RESET */

int ensemble_reset () {
__ensemble_R[0] = _true;
__ensemble_R[1] = _false;
__ensemble__reset_input();
return 0;
}
