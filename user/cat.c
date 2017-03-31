#include "cat.h"

void cat( char* filename ) {
	int fd = open( filename );
  	char* result = "";
	int rd = read( fd, result, CAT_MAX );
	print( UART1, result, strlen( result ) );
	print( UART1, "\n", 1 );
}
