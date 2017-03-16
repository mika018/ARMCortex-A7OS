#ifndef __IPC_H
#define __IPC_H

// Include functionality relating to newlib (the standard C library).

#include <stdlib.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#define MESS_TABLE_SIZE 105

typedef int pid_t;

typedef struct message {
  pid_t pid_src;
  pid_t pid_des;
  int   signal;
  bool  active;
} message;

void ipc_initialise();
int pipe_send_message( int source, int destination, int signal );
int pipe_receive_message( int destination );

#endif