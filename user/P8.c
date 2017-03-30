#include "P8.h"

void main_P8() {
	int fd = open( "README" );
	char* input = "Well this seems like an interesting coursework but it is full of debugging!";
	int wr = write( fd, input, strlen( input ) );
	char* res = "";
	int rd = read( fd, res, 11 );
	int rdi = read( fd, res, 99999 );
	int wri = write( fd, "BRAVOOOOOOO0000000000000", 24 );
	exit( EXIT_SUCCESS );
}
