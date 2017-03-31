#ifndef __IPC_H
#define __IPC_H

#include "libc.h"

#define NO_OF_PIPES 105

typedef int pid_t;

typedef struct buffer_t {
  pid_t pid;
  int   signal;
} buffer_t;

typedef struct pipe_t {
  int      pipe_id;
  buffer_t buff_1;
  buffer_t buff_2;
  bool     active;
} pipe_t;

// intialises all pipes by padding them with zeros
void ipc_initialise();
// initialises the pipe between two processes, pid_1 and pid_2
int  ipc_pipe( pid_t pid_1, pid_t pid_2 );
// sends a signal x from process pid_from to process pid_to along pipe pipe_id
int  ipc_send_message( int pipe_id, pid_t pid_src, pid_t pid_des, int sig );
// returns a signal sent to process pid_des on pipe pipe_id
int  ipc_receive_message( int pipe_id, pid_t pid_des );

#endif