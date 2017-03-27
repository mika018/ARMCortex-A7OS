#include "mem.h"

file_t        file[ MAX_FILES ];
disk_header_t header;

uint32_t get_block_address( int32_t block_row ) {
    return block_row * 32; //still not good
}

void file_setup() {
    memset( &header, 0, sizeof( disk_header_t ) );
    header.new_file       = ( void* ) get_block_address( 1 );
    header.new_data_block = ( void* ) get_block_address( MAX_FILES + 1 );
    int res = disk_wr( get_block_address( 0 ), (uint8_t*) &header, BLOCK_SIZE );
    if( res == 0 ) {
        // print( "Disk ready!\n", 12 );
    } else {
        // print( "Disk failed!\n", 13 );
    }
}

void read_header() {
    memset( &header, 0, sizeof( disk_header_t ) );
    int res = disk_rd( get_block_address( 0 ), (uint8_t*) &header, BLOCK_SIZE );
    if( res == 0 ) {
        // print( "Header successfully loaded!\n", 28 );
    } else {
        // print( "Header loading failed!\n", 23 );
    }
}

void read_files() {
    for( int i = 0; i < MAX_FILES; i++ ) {
        memset( &file[ i ], 0, sizeof( file_t ) );
        int res = disk_rd( get_block_address( i + 1 ), (uint8_t*) &file[ i ], BLOCK_SIZE );
        // print( "File i sucessfully loaded\n", 26 );
    }
}

void file_disk_load() {
    read_header();
    read_files();
}

//make sure there is space for new file and handle it in file_new
int new_file_id() {
    int new_id = ( ( uint32_t ) header.new_file / 256 ) - 1;
    if( new_id < MAX_FILES ) {
        return new_id;
    } else {
        return -1;
    }
}

void* get_new_data_block_address() {
    void* old_data_block = header.new_data_block;
    header.new_data_block += BLOCK_SIZE;
    int res = disk_wr( get_block_address( 0 ), (uint8_t*) &header, BLOCK_SIZE );
    return old_data_block;
}

// copies 19 characters of the given name and terminates with a null
// character to ensure valid name if the given name is longer than
// 20 characters
void set_file_name( int file_id, char* name ) {
    strncpy( file[ file_id ].name, name, 19 );
    file[ file_id ].name[ 19 ] = '\0';
}

int file_new( char* name ) {
    int file_id = new_file_id();
    memset( &file[ file_id ], 0, sizeof( file_t ) );
    file[ file_id ].descriptor   = FILE;
    file[ file_id ].no_of_blocks = 0;
    file[ file_id ].address      = get_new_data_block_address();
    set_file_name( file_id, name );
    int res = disk_wr( get_block_address( file_id + 1 ), (uint8_t*) &file[ file_id ], BLOCK_SIZE );
    if( res == 0 ) {
        // print( "New file created!\n", 12 );
        header.new_file += BLOCK_SIZE;
        int res = disk_wr( get_block_address( 0 ), (uint8_t*) &header, BLOCK_SIZE );
        return FILE_SUCCESS;
    } else {
        // print( "New file failed!\n", 13 );
        return FILE_FAILURE;
    }
}
