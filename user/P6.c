#include "P6.h"

int child_buffer[NO_OF_PHILOSOPHERS];
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
	for( int i = 0; i < NO_OF_PHILOSOPHERS; i++ ){
		int res = fork();
		if( res == 0 ) {  
			pid_t pid_child = get_pid();
            char *id;
            // itoa( id, pid_child );
			while( 1 ) { // Child process
                itoa( id, i + 1 );
				child_buffer[ i ] = mreceive( pid_child );
				switch( child_buffer[ i ] ) {
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
		} else {
			child_id[ i ] = res;
		}
	}
	pid_t pid_parent = get_pid();
	for( int i = 0; i < NO_OF_PHILOSOPHERS; i++ ){
		msend( pid_parent, child_id[ i ], INIT );
	}
    yield();
	while( 1 ) { // Parent process
        switch ( rnd ) {
            case ODD: {
                for( int i = 0; i < NO_OF_PHILOSOPHERS; i++ ){
                    if( i % 2 == 0 ) {
                        msend( pid_parent, child_id[ i ], EAT );
                    } else {
                        msend( pid_parent, child_id[ i ], THINK );
                    }
                }
				break;
            }
            case EVEN: {
                for( int i = 0; i < NO_OF_PHILOSOPHERS; i++ ){
                    if( i % 2 == 0 ) {
                        msend( pid_parent, child_id[ i ], THINK );
                    } else {
                        msend( pid_parent, child_id[ i ], EAT );
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
