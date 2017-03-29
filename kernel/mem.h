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

#define DIRECTORY 0
#define FILE      1

typedef struct file_t {
	int32_t descriptor;
	char    name[ 20 ];
	int32_t	no_of_blocks;
	void*   address;
} file_t;

typedef struct disk_header_t {
	void* new_file;
	void* new_data_block;
	char  holder[ 24 ];
} disk_header_t;

void file_setup();
void file_disk_load();
int  file_new( char* name );

#endif