#ifndef __IPC_H
#define __IPC_H

// Include functionality relating to newlib (the standard C library).

// #include <stdlib.h>
// #include <stdbool.h>
// #include <stddef.h>
// #include <stdint.h>
// #include <string.h>
// #include <limits.h>

#include "libc.h"

#define NO_OF_PIPES 105

typedef int pid_t;

typedef struct buffer_t {
  pid_t pid;
  int   signal;
} buffer_t;

typedef struct pipe_t {
  buffer_t buff_1;
  buffer_t buff_2;
  bool     active;
} pipe_t;

void ipc_initialise();
int  ipc_pipe( pid_t pid_1, pid_t pid_2 );
void ipc_send_message( int pipe_id, pid_t pid_src, pid_t pid_des, int sig );
int  ipc_receive_message( int pipe_id, pid_t pid_des );

#endif