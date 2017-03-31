#include "cat.h"

void _cat( char* filename, char* result ) {
	int fd = open( filename );
	memset( result, 0, sizeof( result ) );
	int rd = read( fd, result, sizeof( result ) );
	//exit( EXIT_SUCCESS );
}
