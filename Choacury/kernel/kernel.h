#ifndef __KERNEL_H
#define __KERNEL_H

#include "reqheaders/stdint.h"
#include "reqheaders/stddef.h"

#include   "GIC.h"
#include "PL011.h"
#include "SP804.h"

#include "interrupt.h"

#include <string.h>


#include "P0.h"
#include "P1.h"
#include "P2.h"

int numPrograms;
int ipcFlag;
extern uint32_t tos_Programs;
/* The kernel source code is made simpler by three type definitions:
 *
 * - a type that captures each component of an execution context (i.e.,
 *   processor state) in a compatible order wrt. the low-level handler
     preservation and restoration prologue and epilogue,
 * - a type that captures a process identifier, which is basically an
 *   integer, and
 * - a type that captures a process PCB.
 */

typedef struct {
  uint32_t cpsr, pc, gpr[ 13 ], sp, lr;
} ctx_t;

typedef int pid_t;

typedef struct {
  pid_t pid;
  ctx_t ctx;
  int priority;
} pcb_t;

typedef struct {
	int max;
	int forks[16];
	int noWithForks;
} dp_data;

#endif
