#include "libc.h"

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

void* memset( void *s, int c, size_t count ) {
  char *xs = s;
  
  while( count-- ) {
    *xs++ = c;
  }
  
  return s;
}

void yield() {
  asm volatile( "svc %0     \n" // make system call SYS_YIELD
              :
              : "I" (SYS_YIELD)
              : );

  return;
}

void print( const void* x, size_t n ) {
  asm volatile( "mov r0, %1 \n" // assign r0 =  x
                "mov r1, %2 \n" // assign r1 =  n
                "svc %0     \n" // make system call SYS_PRINT
              : 
              : "I" (SYS_PRINT), "r" (x), "r" (n)
              : "r0", "r1" );

  return;
}

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

void msend( int pipe_id, int pid_src, int pid_des, int x ) {
  asm volatile( "mov r0, %1 \n" // assign r0 = pipe_id
                "mov r1, %2 \n" // assign r1 = pid_src
                "mov r2, %3 \n" // assign r2 = pid_des
                "mov r3, %4 \n" // assign r3 =       x
                "svc %0     \n" // make system call SYS_MSEND
              :  
              : "I" (SYS_MSEND), "r" (pipe_id), "r" (pid_src), "r" (pid_des), "r" (x) 
              : "r0", "r1", "r2", "r3" );
}

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

int get_pid() {
  int r;

  asm volatile( "svc %1     \n" // make system call SYS_GETPID
                "mov %0, r0 \n" // assign r  = r0
              : "=r" (r) 
              : "I" (SYS_GETPID)
              : );

  return r;
}

int get_parent_pid() {
  int r;

  asm volatile( "svc %1     \n" // make system call SYS_GETPARENT
                "mov %0, r0 \n" // assign r  = r0
              : "=r" (r) 
              : "I" (SYS_GETPARENT)
              : );

  return r;
}

int open( char* name ) {
  int r;

  asm volatile( "mov r0, %2 \n" // assign r0 = name
                "svc %1     \n" // make system call SYS_WRITE
                "mov %0, r0 \n" // assign r  = r0
              : "=r" (r) 
              : "I" (SYS_OPEN), "r" (name)
              : "r0" );

  return r;

}

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

int fork() {
  int r;

  asm volatile( "svc %1     \n" // make system call SYS_FORK
                "mov %0, r0 \n" // assign r  = r0 
              : "=r" (r) 
              : "I" (SYS_FORK)
              : "r0" );

  return r;
}

void exit( int x ) {
  asm volatile( "mov r0, %1 \n" // assign r0 =  x
                "svc %0     \n" // make system call SYS_EXIT
              :
              : "I" (SYS_EXIT), "r" (x)
              : "r0" );

  return;
}

void exec( const void* x, int p ) {
  asm volatile( "mov r0, %1 \n" // assign r0 = x
                "mov r1, %2 \n" // assign r1 = p
                "svc %0     \n" // make system call SYS_EXEC
              :
              : "I" (SYS_EXEC), "r" (x), "r" (p)
              : "r0", "r1" );

  return;
}

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
