#include "console.h"

// reads the string from the UART
void gets( char* x, int n ) {
  for( int i = 0; i < n; i++ ) {
    x[ i ] = PL011_getc( UART1, true );
    
    if( x[ i ] == '\x0A' ) {
      x[ i ] = '\x00'; break;
    }
  }
}

/* Since we lack a *real* loader (as a result of lacking a storage 
 * medium to store program images), the following approximates one:
 * given a program name, from the set of programs statically linked
 * into the kernel image, it returns a pointer to the entry point.
 */

extern void main_P3(); 
extern void main_P4(); 
extern void main_P5(); 
extern void main_P6(); 
extern void main_P8();


void* load( char* x ) {
  if     ( 0 == strcmp( x, "P3" ) ) {
    return &main_P3;
  }
  else if( 0 == strcmp( x, "P4" ) ) {
    return &main_P4;
  }
  else if( 0 == strcmp( x, "P5" ) ) {
    return &main_P5;
  }
  else if( 0 == strcmp( x, "P6" ) ) {
    return &main_P6;
  }
  else if( 0 == strcmp( x, "P8" ) ) {
    return &main_P8;
  }

  return NULL;
}

/* The behaviour of the console process can be summarised as an
 * (infinite) loop over three main steps, namely
 *
 * 1. write a command prompt then read a command,
 * 2. split the command into tokens using strtok,
 * 3. execute whatever steps the command dictates.
 */

void main_console() {
  char* p, x[ 1024 ];

  while( 1 ) {
    print( UART1, "shell$ ", 7 ); gets( x, 1024 ); p = strtok( x, " " );

    if     ( 0 == strcmp( p, "fork" ) ) {
      pid_t pid = fork();
      if( 0 == pid ) {
        void* addr = load( strtok( NULL, " " ) );
        char* priority_char = strtok( NULL, " ");
        int priority;
        // If priority not entered set it to the default value (2)
        if( priority_char == 0x0 ) {
          priority = 2;
        } else {
          priority = atoi( priority_char );
        }
        exec( addr, priority );
      }
    } 
    else if( 0 == strcmp( p, "kill" ) ) {
      char* pid_char = strtok( NULL, " " );
      int   s   = atoi( strtok( NULL, " " ) );
      
      if( 0 == strcmp( pid_char, "all" ) ) {
        kill( -1, s );
      } else {
        pid_t pid = atoi( pid_char );
        kill( pid, s );
      }
    } 
    else if( 0 == strcmp( p, "open" ) ) {
      char* name = strtok( NULL, "\0" );
      int res = open( name );

      char result[2];
      result[0] = '\0';
      itoa( result, res );

      print( UART1, result, strlen( result ) );
      print( UART1, "\n", 1 );
    }
    else if( 0 == strcmp( p, "close" ) ) {
      int fd = atoi( strtok( NULL, "\0" ) );
      
      close( fd );
    } 
    else if( 0 == strcmp( p, "write" ) ) {
      int   fd = atoi( strtok( NULL, " " ) );
      char* x  = strtok( NULL, "\0" );
      int   n  = strlen( x );
      
      write( fd, x, n );
    }
    else if( 0 == strcmp( p, "cat" ) ) {
      char* filename  = strtok( NULL, " " );
      
      cat( filename);
    } 
    else {
      print( UART1, "unknown command\n", 16 );
    }
  }

  exit( EXIT_SUCCESS );
}
