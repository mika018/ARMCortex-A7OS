#include "P6.h"

int buffer;

void main_P7() { 
	pid_t pid_child  = get_pid();
	pid_t pid_parent = get_parent_pid();
	int   pipe 		 = make_pipe( pid_parent, pid_child );
	char  id[3];
	memset( id, 0, sizeof( id ) );
	while( 1 ) { // Child process
		itoa( id, pid_child );
		buffer = mreceive( pipe, pid_child );
		print( UART0, "Philosopher ", 12 );
		print( UART0, id, strlen( id ) );
		switch( buffer ) {
			case INIT: {
				print( UART0, ": READY\n", 8 );
				break;
			}
			case EAT: {
				print( UART0, ": EATING\n", 9 );
				break;
			}
			case THINK: {
				print( UART0, ": THINKING\n", 11 );
				break;
			}
			case IPC_EMPTY: {
				print( UART0, ": WAITING FOR COMMAND\n", 22 );
				break;
			}
			default: {
				print( UART0, ": UNSUPPORTED SIGNAL\n", 21 );
				break;
			}
		}
		yield();
	}
		
	exit( EXIT_SUCCESS );
}