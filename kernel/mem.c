#include "mem.h"

void* new_file;
void* new_data_block;

file_t        file[ NO_OF_FILES ];
disk_header_t header;

uint32_t get_block_address( int32_t block_row ) {
    return block_row * BLOCK_SIZE;
}

void file_setup() {
    memset( &header, 0, sizeof( disk_header_t ) );
    header.new_file       = get_block_address( 1 );
    header.new_data_block = get_block_address( NO_OF_FILES + 1 );
    int res = disk_wr( get_block_address( 0 ), (uint8_t*) &header, BLOCK_SIZE );
    if( res == 0 ) {
        print( "Disk ready!\n", 12 );
    } else {
        print( "Disk failed!\n", 13 );
    }
}
