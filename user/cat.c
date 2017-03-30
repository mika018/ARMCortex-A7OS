#include "cat.h"

void _cat( char* filename ) {
	int fd = open( filename );
	char* result = "";
	int rd = read( fd, result, UINT32_MAX );
	print( result, strlen( result ) );
	exit( EXIT_SUCCESS );
}
