#ifndef __SCHEDULER_H
#define __SCHEDULER_H

// Include functionality relating to newlib (the standard C library).

#include <stdlib.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

/* The kernel source code is made simpler via three type definitions:
 *
 * - a type that captures a Process IDentifier (PID), which is really
 *   just an integer,
 * - a type that captures each component of an execution context (i.e.,
 *   processor state) in a compatible order wrt. the low-level handler
 *   preservation and restoration prologue and epilogue, and
 * - a type that captures a process PCB.
 */
#define TABLE_SIZE 15

typedef int pid_t;

typedef struct {
  uint32_t cpsr, pc, gpr[ 13 ], sp, lr;
} ctx_t;

typedef struct pcb_t {
  pid_t pid;
  ctx_t ctx;
  bool running;
  uint32_t init_priority;
  uint32_t curr_priority;
} pcb_t;

void scheduler_initialise( ctx_t* ctx );
void scheduler_run( ctx_t* ctx );
pid_t scheduler_fork( ctx_t* ctx );
void scheduler_exec( ctx_t* ctx );
void scheduler_exit( ctx_t* ctx );
pid_t get_pid();

/*
typedef struct {
  pcb_t* head;
} pcb_table;
*/
#endif