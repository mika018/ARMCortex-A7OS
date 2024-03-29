#include "P6.h"

extern void main_P7(); 

int child_pipe[NO_OF_PHILOSOPHERS];
int  chopstick[NO_OF_PHILOSOPHERS];
int   child_id[NO_OF_PHILOSOPHERS];
pid_t pid_parent;

int first_index = 0;

int update_first_index() {
	first_index++;
	return first_index % NO_OF_PHILOSOPHERS;
}

void initialise() {
	pid_parent = get_pid();
	for( int i = 0; i < NO_OF_PHILOSOPHERS; i++ ){
		chopstick[ i ] = 1;
		int res = fork();
		if( res == 0 ) {  
			exec( &main_P7, 2 );
		} else if( res == -1 ) { // maximum processes running
			continue;
		} else {
			child_id[ i ] = res;
			child_pipe[ i ] = make_pipe( pid_parent, child_id[ i ] );
			int r = msend( child_pipe[ i ], pid_parent, child_id[ i ], INIT );
		}
	}
	yield();
}

int chopsticks_available( int i ) {
	if( chopstick[ i ] && chopstick[ ( i + 1 ) % NO_OF_PHILOSOPHERS ] ){
		return 1;
	} else {
		return 0;
	}
}

void chopsticks_remove( int i ) {
	chopstick[ i ]     							= 0;
	chopstick[ ( i + 1 ) % NO_OF_PHILOSOPHERS ] = 0;
}

void chopsticks_return() {
	for( int i = 0; i < NO_OF_PHILOSOPHERS; i++ ) {
		chopstick[ i ] = 1;
	}
}

void main_P6() {
	initialise();
	while( 1 ) { 
		for( int i = first_index; i < NO_OF_PHILOSOPHERS + first_index; i++ ) {
			int id = i % NO_OF_PHILOSOPHERS;
			if( chopsticks_available( id ) ) {
				int r = msend( child_pipe[ id ], pid_parent, child_id[ id ], EAT );
				chopsticks_remove( id );
			} else {
                int r = msend( child_pipe[ id ], pid_parent, child_id[ id ], THINK );
			}
		}
		first_index = update_first_index();
		chopsticks_return();
        print( UART0, "Waiter       : SWITCH BOYS\n", 27);
        yield();
	}
	exit( EXIT_SUCCESS );
}
