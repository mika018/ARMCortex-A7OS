#ifndef __MEM_H
#define __MEM_H

// Include functionality relating to newlib (the standard C library).

// #include <stdlib.h>
// #include <stdbool.h>
// #include <stddef.h>
// #include <stdint.h>
#include <string.h>
// #include <limits.h>

#include "libc.h"
#include "disk.h"

#define NO_OF_BLOCKS 4096
#define BLOCK_SIZE   32

#define MAX_FILES 95
#define MAX_DATA_LINES 32673 // 32768 - MAX_FILES

#define MAX_CONTENT 24

#define DIRECTORY 0
#define FILE      1

typedef struct file_t {
	int32_t descriptor;
	char    name[ 16 ];
	int32_t	no_of_blocks;
	int32_t active;
	void*   address;
} file_t;

typedef struct disk_header_t {
	void* new_file;
	void* new_data_block;
	char  holder[ 24 ];
} disk_header_t;

typedef struct data_t {
	char    content[ MAX_CONTENT ];
	int32_t full;
	void*   next_data_block;
} data_t;

void file_setup();
void file_disk_load();
int  file_open( char* name );
int  file_close( int fd );
int  file_write( int fd, void* x, size_t n );

#endif