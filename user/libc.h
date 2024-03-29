#ifndef __LIBC_H
#define __LIBC_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "PL011.h"

// Define a type that that captures a Process IDentifier (PID).

typedef int pid_t;

/* The definitions below capture symbolic constants within these classes:
 *
 * 1. system call identifiers (i.e., the constant used by a system call
 *    to specify which action the kernel should take),
 * 2. signal identifiers (as used by the kill system call), 
 * 3. status codes for exit,
 * 4. standard file descriptors (e.g., for read and write system calls),
 * 5. inter-process communication system calls (make_pipe, msend, mreceive),
 * 6. additional supportive system calls such as get_pid and get_parent_pid,
 * 7. platform-specific constants, which may need calibration.
 *
 * They don't *precisely* match the standard C library, but are intended
 * to act as a limited model of similar concepts.
 */

#define SYS_YIELD     ( 0x00 )
#define SYS_GETPID    ( 0x01 )
#define SYS_FORK      ( 0x02 )
#define SYS_EXEC      ( 0x03 )
#define SYS_EXIT      ( 0x04 )
#define SYS_PRINT     ( 0x05 )
#define SYS_KILL      ( 0x06 )
#define SYS_MSEND     ( 0x07 )
#define SYS_MRECEIVE  ( 0x08 )
#define SYS_PIPE      ( 0x09 )
#define SYS_GETPARENT ( 0x13 )
#define SYS_OPEN      ( 0x20 )
#define SYS_CLOSE     ( 0x21 )
#define SYS_READ      ( 0x22 )
#define SYS_WRITE     ( 0x23 )
#define SYS_CAT       ( 0x30 )

#define SIG_TERM      ( 0x00 )
#define SIG_QUIT      ( 0x01 )

#define EXIT_SUCCESS  ( 0 )
#define EXIT_FAILURE  ( 1 )

#define KILL_SUCCESS  ( 0 )
#define KILL_FAILURE  ( 1 )

#define FILE_SUCCESS  ( 0 )
#define FILE_FAILURE  ( 1 )

#define IPC_FAILURE   ( 0 )
#define IPC_SUCCESS   ( 1 )
#define IPC_EMPTY     (-1 )

#define  STDIN_FILENO ( 0 )
#define STDOUT_FILENO ( 1 )
#define STDERR_FILENO ( 2 )



// convert ASCII string x into integer r
extern int  atoi( char* x        );
// convert integer x into ASCII string r
extern void itoa( char* r, int x );

// fills memory pointed to by s with a constant byte c of size count.
// similar in design to MEMSET(3).
extern void* memset( void *s, int c, size_t count );

// cooperatively yield control of processor, i.e., invoke the scheduler
extern void yield();

// opens a file and loads it onto the available address in disk.bin
extern int  open( char* name );
// closes a file by removing it from disk.bin
extern int  close( int fd );
// write n bytes from x to   the file descriptor fd; return bytes written
extern int  write( int fd, const void* x, size_t n );
// read  n bytes into x from the file descriptor fd; return bytes read
extern int  read( int fd,       void* x, size_t n );

// initialises the pipe between two processes
extern int  make_pipe( int pid_1, int pid_2 );
// sends a signal x from process pid_from to process pid_to along pipe pipe_id
extern int  msend( int pipe_id, int pid_src, int pid_des, int x );
// returns a signal sent to process pid_des on pipe pipe_id
extern int  mreceive( int pipe_id, int pid_des );

// prints x of size n to the d
extern void print( PL011_t* d, char* x, size_t n );

// perform fork, returning 0 iff. child or > 0 iff. parent process
extern int  fork();
// perform exit, i.e., terminate process with status x
extern void exit( int   x );
// perform exec, i.e., start executing program at address x with priotity p
extern void exec( const void* x , int p );

// signal process identified by pid with signal x
extern int  kill( pid_t pid, int x );

// returns a pid of a currently running process
extern int  get_pid();
// returns a parent pid of a currently running process
extern int  get_parent_pid();

#endif
