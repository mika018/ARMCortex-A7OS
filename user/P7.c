#include "P7.h"

int buffer;

void main_P7() { 
	pid_t pid_child  = get_pid();
	pid_t pid_parent = get_parent_pid();
	int   pipe 		 = make_pipe( pid_parent, pid_child );
	char  *id;
	while( 1 ) { // Child process
		itoa( id, pid_child );
		buffer = mreceive( pipe, pid_child );
		switch( buffer ) {
			case INIT: {
				print( "Philosopher ", 12 );
				print( id, 1 );
				print( ": READY\n", 8 );
				break;
			}
			case EAT: {
				print( "Philosopher ", 12 );
				print( id, 1 );
				print( ": EATING\n", 9 );
				break;
			}
			case THINK: {
				print( "Philosopher ", 12 );
				print( id, 1 );
				print( ": THINKING\n", 11 );
				break;
			}
			default   : {
				break;
			}
		}
		yield();
	}
		
	exit( EXIT_SUCCESS );
}
