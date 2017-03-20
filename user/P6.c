#include "P6.h"

extern void main_P7(); 

int   child_pipe[NO_OF_PHILOSOPHERS];
int     child_id[NO_OF_PHILOSOPHERS];

int rnd = ODD;

int next_round() {
	if( rnd == ODD ) {
		return EVEN;
	} else {
		return ODD;
	}
}

void main_P6() {
	pid_t pid_parent = get_pid();
	for( int i = 0; i < NO_OF_PHILOSOPHERS; i++ ){
		int res = fork();
		if( res == 0 ) {  
			exec( &main_P7, 2 );
		} else {
			child_id[ i ] = res;
		}
	}
	for( int i = 0; i < NO_OF_PHILOSOPHERS; i++ ){
		child_pipe[ i ] = make_pipe( pid_parent, child_id[ i ] );
		msend( child_pipe[ i ], pid_parent, child_id[ i ], INIT );
	}
    yield();
	while( 1 ) { // Parent process
        switch ( rnd ) {
            case ODD: {
                for( int i = 0; i < NO_OF_PHILOSOPHERS; i++ ){
                    if( i % 2 == 0 ) {
                        msend( child_pipe[ i ], pid_parent, child_id[ i ], EAT );
                    } else {
                        msend( child_pipe[ i ], pid_parent, child_id[ i ], THINK );
                    }
                }
				break;
            }
            case EVEN: {
                for( int i = 0; i < NO_OF_PHILOSOPHERS; i++ ){
                    if( i % 2 == 0 ) {
                        msend( child_pipe[ i ], pid_parent, child_id[ i ], THINK );
                    } else {
                        msend( child_pipe[ i ], pid_parent, child_id[ i ], EAT );
                    }
                }
				break;
            }
        } 
        print( "Waiter       : SWITCH BOYS\n", 27);
		rnd = next_round();
        yield();
	}
	exit( EXIT_SUCCESS );
}
