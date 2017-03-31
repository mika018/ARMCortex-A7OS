#include "P6.h"

int buffer;

void main_P7() { 
	pid_t pid_child  = get_pid();
	pid_t pid_parent = get_parent_pid();
	int   pipe 		 = make_pipe( pid_parent, pid_child );
	char  *id;
	while( 1 ) { // Child process
		itoa( id, pid_child );
		buffer = mreceive( pipe, pid_child );
		print( "Philosopher ", 12 );
		print( id, strlen( id ) );
		switch( buffer ) {
			case INIT: {
				print( ": READY\n", 8 );
				break;
			}
			case EAT: {
				print( ": EATING\n", 9 );
				break;
			}
			case THINK: {
				print( ": THINKING\n", 11 );
				break;
			}
			case IPC_EMPTY: {
				print( ": WAITING FOR COMMAND\n", 22 );
				break;
			}
			default: {
				print( ": UNSUPPORTED SIGNAL\n", 21 );
				break;
			}
		}
		yield();
	}
		
	exit( EXIT_SUCCESS );
}