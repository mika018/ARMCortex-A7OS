#include "P6.h"

#define NO_OF_PHILOSOPHERS 2
#define READ 0
#define WRITE 1


int          chan[NO_OF_PHILOSOPHERS][2];
char child_buffer[NO_OF_PHILOSOPHERS][5];

void main_P6() {
	for( int i = 0; i < NO_OF_PHILOSOPHERS; i++ ){
		if( fork() == 0 ) {  
			while( 1 ) { // Child process

			}
		}
	}
	while( 1 ) { // Parent process

	}
	exit( EXIT_SUCCESS );
}
