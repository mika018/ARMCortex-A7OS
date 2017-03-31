#ifndef __SCHEDULER_H
#define __SCHEDULER_H

#include "libc.h"

#define TABLE_SIZE    30
#define FORK_PRIORITY 4194304

typedef int pid_t;

typedef struct {
  uint32_t cpsr, pc, gpr[ 13 ], sp, lr;
} ctx_t;

typedef struct pcb_t {
  pid_t pid;
  pid_t pid_parent;
  ctx_t ctx;
  bool running;
  uint32_t priority_init;
  uint32_t priority_curr;
} pcb_t;

// initialises the pcb table by loading the console.
void  scheduler_initialise( ctx_t* ctx );
// performs a context switch between two running processes
void  scheduler_run( ctx_t* ctx );

// creates a new child process identical to the parent process
pid_t scheduler_fork( ctx_t* ctx );
// replaces a current process image with a new one
void  scheduler_exec( ctx_t* ctx );
// causes a process to terminate, removing it from the pcb table
void  scheduler_exit( ctx_t* ctx );
// forces a process to terminate, removing it from the pcb table
int   scheduler_kill( pid_t pid, int sig );

// returns the pid of currently tunning process
pid_t scheduler_get_pid();
// returns the parent pid of currently tunning process
pid_t scheduler_get_pid_parent();

#endif