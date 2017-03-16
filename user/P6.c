#include "P6.h"


#define NO_OF_PHILOSOPHERS 2
#define READ 0
#define WRITE 1
#define INIT 2
#define EAT 3
#define THINK 4


// int         chan[NO_OF_PHILOSOPHERS][2];
// int child_buffer[NO_OF_PHILOSOPHERS];
// int     child_id[NO_OF_PHILOSOPHERS];


void main_P6() {
// 	for( int i = 0; i < NO_OF_PHILOSOPHERS; i++ ){
// 		int res = fork();
// 		if( res == 0 ) {  
// 			pid_t pid_child = get_pid();
// 			while( 1 ) { // Child process
// 				child_buffer[ i ] = pipe_receive_message( pid_child );
// 				switch( child_buffer[ i ] ) {
// 					case INIT: {
// 						putss( "Philosopher ", 12 );
// 						PL011_putc( UART0, i, true );
// 						putss( ": READY\n", 8 );
// 						break;
// 					}
// 					case EAT: {
// 						putss( "Philosopher ", 12 );
// 						PL011_putc( UART0, i, true );
// 						putss( ": EATING\n", 9 );
// 						break;
// 					}
// 					case THINK: {
// 						putss( "Philosopher ", 12 );
// 						PL011_putc( UART0, i, true );
// 						putss( ": THINKING\n", 11 );
// 						break;
// 					}
// 				}
// 			}
// 		} else {
// 			child_id[ i ] = res;
// 		}
// 	}
// 	pid_t pid_parent = get_pid();
// 	for( int i = 0; i < NO_OF_PHILOSOPHERS; i++ ){
// 		int res = pipe_send_message( get_pid, child_id[ i ], 2 );
// 	}
// 	while( 1 ) { // Parent process

// 	}
// 	exit( EXIT_SUCCESS );
}
