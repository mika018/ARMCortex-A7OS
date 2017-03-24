#ifndef __MEM_H
#define __MEM_H

// Include functionality relating to newlib (the standard C library).

// #include <stdlib.h>
// #include <stdbool.h>
// #include <stddef.h>
// #include <stdint.h>
// #include <string.h>
// #include <limits.h>

#include "libc.h"

#define NO_OF_MEMBLOCKS 512

typedef struct memblock_t {
	void* address;
	int   dir_id;
	int   file_id;
	int   occupied;
	void* next_block;
} memblock_t;

// typedef struct dir_t {
// 	int   dir_id;
// 	void* address;
// } dir_t;

// typedef struct file_t {
// 	int   dir_id;
// 	int   file_id;
// 	void* address;
// 	void* next_block;
// } file_t;

#endif