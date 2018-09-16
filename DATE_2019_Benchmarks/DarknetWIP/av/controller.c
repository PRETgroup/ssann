/* sscc : C CODE OF SORTED EQUATIONS controller - INLINE MODE */

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
#define __controller_GENERIC_TEST(TEST) return TEST;
typedef void (*__controller_APF)();
static __controller_APF *__controller_PActionArray;

#include "controller.h"

/* EXTERN DECLARATIONS */

#ifndef _NO_EXTERN_DEFINITIONS
#ifndef _NO_PROCEDURE_DEFINITIONS
#ifndef _run_front_DEFINED
#ifndef run_front
extern void run_front();
#endif
#endif
#ifndef _run_sides_DEFINED
#ifndef run_sides
extern void run_sides();
#endif
#endif
#ifndef _run_back_DEFINED
#ifndef run_back
extern void run_back();
#endif
#endif
#ifndef _run_fin_DEFINED
#ifndef run_fin
extern void run_fin();
#endif
#endif
#endif
#endif

/* INITIALIZED CONSTANTS */

/* MEMORY ALLOCATION */

static boolean __controller_V0;


/* INPUT FUNCTIONS */

void controller_I_start () {
__controller_V0 = _true;
}

/* ACTIONS */

/* PREDEFINED ACTIONS */

/* PRESENT SIGNAL TESTS */

#define __controller_A1 \
__controller_V0

/* OUTPUT ACTIONS */

#define __controller_A2 \
controller_O_done()

/* ASSIGNMENTS */

#define __controller_A3 \
__controller_V0 = _false

/* PROCEDURE CALLS */

#define __controller_A4 \
run_fin()

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

int controller_number_of_execs () {
return (0);
}


/* AUTOMATON (STATE ACTION-TREES) */



static void __controller__reset_input () {
__controller_V0 = _false;
}

/* REDEFINABLE BIT TYPE */

#ifndef __SSC_BIT_TYPE_DEFINED
typedef char __SSC_BIT_TYPE;
#endif

/* REGISTER VARIABLES */

static __SSC_BIT_TYPE __controller_R[2] = {_true,
 _false};

/* AUTOMATON ENGINE */

int controller () {
/* AUXILIARY VARIABLES */

static __SSC_BIT_TYPE E[3];
if (!(_true)) {
__controller_A2;
#ifdef TRACE_ACTION
fprintf(stderr, "__controller_A2\n");
#endif
}
E[0] = !(_true);
E[1] = __controller_R[1]&&!(__controller_R[0]);
E[2] = E[1]&&(
#ifdef TRACE_ACTION
fprintf(stderr, "test 1\n"),
#endif
__controller_A1);
if (E[2]) {
__controller_A4;
#ifdef TRACE_ACTION
fprintf(stderr, "__controller_A4\n");
#endif
}
E[1] = E[1]&&!((
#ifdef TRACE_ACTION
fprintf(stderr, "test 1\n"),
#endif
__controller_A1));
__controller_R[1] = __controller_R[0]||E[2]||(__controller_R[1]&&E[1]);
E[1] = __controller_R[1];
__controller_R[0] = !(_true);
__controller__reset_input();
return E[1];
}

/* AUTOMATON RESET */

int controller_reset () {
__controller_R[0] = _true;
__controller_R[1] = _false;
__controller__reset_input();
return 0;
}
