#include "libc.h"

// convert ASCII string x into integer r
int  atoi( char* x        ) {
  char* p = x; bool s = false; int r = 0;

  if     ( *p == '-' ) {
    s =  true; p++;
  }
  else if( *p == '+' ) {
    s = false; p++;
  }

  for( int i = 0; *p != '\x00'; i++, p++ ) {
    r = s ? ( r * 10 ) - ( *p - '0' ) :
            ( r * 10 ) + ( *p - '0' ) ;
  }

  return r;
}

// convert integer x into ASCII string r
void itoa( char* r, int x ) {
  char* p = r; int t, n;

  if( x < 0 ) {
    p++; t = -x; n = 1;
  }
  else {
         t = +x; n = 1;
  }

  while( t >= n ) {
    p++; n *= 10;
  }

  *p-- = '\x00';

  do {
    *p-- = '0' + ( t % 10 ); t /= 10;
  } while( t );

  if( x < 0 ) {
    *p-- = '-';
  }

  return;
}

// fills memory pointed to by s with a constant byte c of size count.
// similar in design to MEMSET(3).
void* memset( void *s, int c, size_t count ) {
  char *xs = s;
  
  while( count-- ) {
    *xs++ = c;
  }
  
  return s;
}

// cooperatively yield control of processor, i.e., invoke the scheduler
void yield() {
  asm volatile( "svc %0     \n" // make system call SYS_YIELD
              :
              : "I" (SYS_YIELD)
              : );

  return;
}

// prints x of size n to d
void print( PL011_t* d, char* x, size_t n ) {
  for( int i = 0; i < n; i++ ) {
    PL011_putc( d, x[ i ], true );
  }
}

// initialises the pipe between two processes
int make_pipe( int pid_1, int pid_2 ) {
  int r;

  asm volatile( "mov r0, %2 \n" // assign r0 = pid_1
                "mov r1, %3 \n" // assign r1 = pid_2
                "svc %1     \n" // make system call SYS_MRECEIVE
                "mov %0, r0 \n" // assign r  = r0
              : "=r" (r) 
              : "I" (SYS_PIPE), "r" (pid_1), "r" (pid_2)
              : "r0", "r1" );

  return r;
}

// sends a signal x from process pid_from to process pid_to along pipe pipe_id
int msend( int pipe_id, int pid_src, int pid_des, int x ) {
  int r;

  asm volatile( "mov r0, %2 \n" // assign r0 = pipe_id
                "mov r1, %3 \n" // assign r1 = pid_src
                "mov r2, %4 \n" // assign r2 = pid_des
                "mov r3, %5 \n" // assign r3 =       x
                "svc %1     \n" // make system call SYS_MSEND
                "mov %0, r0 \n" // assign r  =      r0
              : "=r" (r)
              : "I" (SYS_MSEND), "r" (pipe_id), "r" (pid_src), "r" (pid_des), "r" (x) 
              : "r0", "r1", "r2", "r3" );
  
  return r;
}

// returns a signal sent to process pid_des on pipe pipe_id
int mreceive( int pipe_id, int pid_des ) {
  int r;

  asm volatile( "mov r0, %2 \n" // assign r0 = pipe_id
                "mov r1, %3 \n" // assign r1 = pid_des
                "svc %1     \n" // make system call SYS_MRECEIVE
                "mov %0, r0 \n" // assign r  = r0
              : "=r" (r) 
              : "I" (SYS_MRECEIVE), "r" (pipe_id), "r" (pid_des)
              : "r0", "r1" );

  return r;
}

// returns a pid of a currently running process
int get_pid() {
  int r;

  asm volatile( "svc %1     \n" // make system call SYS_GETPID
                "mov %0, r0 \n" // assign r  = r0
              : "=r" (r) 
              : "I" (SYS_GETPID)
              : );

  return r;
}

// returns a parent pid of a currently running process
int get_parent_pid() {
  int r;

  asm volatile( "svc %1     \n" // make system call SYS_GETPARENT
                "mov %0, r0 \n" // assign r  = r0
              : "=r" (r) 
              : "I" (SYS_GETPARENT)
              : );

  return r;
}

// opens a file and loads it onto the available address in disk.bin
int open( char* name ) {
  int r;

  asm volatile( "mov r0, %2 \n" // assign r0 = name
                "svc %1     \n" // make system call SYS_OPEN
                "mov %0, r0 \n" // assign r  = r0
              : "=r" (r) 
              : "I" (SYS_OPEN), "r" (name)
              : "r0" );

  return r;

}

// closes a file by removing it from disk.bin
int close( int fd ) {
  int r;

  asm volatile( "mov r0, %2 \n" // assign r0 = fd
                "svc %1     \n" // make system call SYS_CLOSE
                "mov %0, r0 \n" // assign r  = r0
              : "=r" (r) 
              : "I" (SYS_CLOSE), "r" (fd)
              : "r0" );

  return r;

}

// write n bytes from x to   the file descriptor fd; return bytes written
int write( int fd, const void* x, size_t n ) {
  int r;

  asm volatile( "mov r0, %2 \n" // assign r0 = fd
                "mov r1, %3 \n" // assign r1 =  x
                "mov r2, %4 \n" // assign r2 =  n
                "svc %1     \n" // make system call SYS_WRITE
                "mov %0, r0 \n" // assign r  = r0
              : "=r" (r) 
              : "I" (SYS_WRITE), "r" (fd), "r" (x), "r" (n)
              : "r0", "r1", "r2" );

  return r;
}

// read  n bytes into x from the file descriptor fd; return bytes read
int  read( int fd,       void* x, size_t n ) {
  int r;

  asm volatile( "mov r0, %2 \n" // assign r0 = fd
                "mov r1, %3 \n" // assign r1 =  x
                "mov r2, %4 \n" // assign r2 =  n
                "svc %1     \n" // make system call SYS_READ
                "mov %0, r0 \n" // assign r  = r0
              : "=r" (r) 
              : "I" (SYS_READ),  "r" (fd), "r" (x), "r" (n) 
              : "r0", "r1", "r2" );

  return r;
}

// perform fork, returning 0 iff. child or > 0 iff. parent process
int fork() {
  int r;

  asm volatile( "svc %1     \n" // make system call SYS_FORK
                "mov %0, r0 \n" // assign r  = r0 
              : "=r" (r) 
              : "I" (SYS_FORK)
              : "r0" );

  return r;
}

// perform exit, i.e., terminate process with status x
void exit( int x ) {
  asm volatile( "mov r0, %1 \n" // assign r0 =  x
                "svc %0     \n" // make system call SYS_EXIT
              :
              : "I" (SYS_EXIT), "r" (x)
              : "r0" );

  return;
}

// perform exec, i.e., start executing program at address x with priotity p
void exec( const void* x, int p ) {
  asm volatile( "mov r0, %1 \n" // assign r0 = x
                "mov r1, %2 \n" // assign r1 = p
                "svc %0     \n" // make system call SYS_EXEC
              :
              : "I" (SYS_EXEC), "r" (x), "r" (p)
              : "r0", "r1" );

  return;
}

// signal process identified by pid with signal x
int kill( int pid, int x ) {
  int r;

  asm volatile( "mov r0, %2 \n" // assign r0 =  pid
                "mov r1, %3 \n" // assign r1 =    x
                "svc %1     \n" // make system call SYS_KILL
                "mov %0, r0 \n" // assign r0 =    r
              : "=r" (r) 
              : "I" (SYS_KILL), "r" (pid), "r" (x)
              : "r0", "r1" );

  return r;
}

