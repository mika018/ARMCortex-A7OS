#include "P6.h"


#define NO_OF_PHILOSOPHERS 2
#define READ 0
#define WRITE 1
#define INIT 2
#define EAT 3
#define THINK 4
#define ODD 5
#define EVEN 6

int child_buffer[NO_OF_PHILOSOPHERS];
int     child_id[NO_OF_PHILOSOPHERS];


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
    int round = ODD;
	pid_t pid_parent = get_pid();
	for( int i = 0; i < NO_OF_PHILOSOPHERS; i++ ){
		msend( pid_parent, child_id[ i ], INIT );
	}
    yield();
	while( 1 ) { // Parent process
        switch ( round ) {
            case ODD: {
                for( int i = 0; i < NO_OF_PHILOSOPHERS; i++ ){
                    if( i % 2 == 0 ) {
                        msend( pid_parent, child_id[ i ], EAT );
                    } else {
                        msend( pid_parent, child_id[ i ], THINK );
                    }
                }
                round = EVEN;
            }
            case EVEN: {
                for( int i = 0; i < NO_OF_PHILOSOPHERS; i++ ){
                    if( i % 2 == 0 ) {
                        msend( pid_parent, child_id[ i ], THINK );
                    } else {
                        msend( pid_parent, child_id[ i ], EAT );
                    }
                }
                round = ODD;
            }
        }      
        print( "Waiter      : SWITCH BOYS\n", 26);
        yield();
	}
	exit( EXIT_SUCCESS );
}
