#include "mem.h"

file_t        file[ MAX_FILES ];
data_t        data[ MAX_DATA_LINES ];
disk_header_t header;

// Initialises the header ( used only on a new disk )
void file_setup() {
    memset( &header, 0, sizeof( disk_header_t ) );
    header.new_data_block = ( void* ) MAX_FILES + 1;
    int res = disk_wr( 0, (uint8_t*) &header, BLOCK_SIZE );
    if( res == 0 ) {
        print( UART0, "FILE: Disk successfully flushed!\n", 33 );
    } else {
        print( UART0, "FILE: Disk failed flushing!\n", 28 );
    }
}

//make sure there is space for new file and handle it in file_new
int new_file_id() {
    for( int i = 0; i < MAX_FILES; i++ ) {
        if( !file[ i ].active ) {
            return i;
        }
    }
    return -1;
}

// returns an address for a new(empty) data block and 
// updates the change to the header
void* new_data_block_address() {
    void* old_data_block = header.new_data_block;
    header.new_data_block++;
    int res = disk_wr( 0, (uint8_t*) &header, BLOCK_SIZE );
    return old_data_block;
}

// copies 15 characters of the given name and terminates with a null
// character to ensure valid name if the given name is longer than
// 15 characters
void set_file_name( int file_id, char* name ) {
    strncpy( file[ file_id ].name, name, 15 );
    file[ file_id ].name[ 15 ] = '\0';
}

// returns a file descriptor of a file with a given name
// returns -1 if it is not found
int find_file_id( char* name ) {
    for( int i = 0; i < MAX_FILES; i++ ) {
        if( file[ i ].active && !strcmp( file[ i ].name, name ) ) {
            return i;
        }
    }
    return -1;
}

// opens a file and loads it onto the available address in disk.bin
int file_open( char* name ) {
    int find = find_file_id( name );
    if( find != -1 ) {
        return find;
    } else {
        int file_id = new_file_id();
        if( file_id == -1 ) {
            print( UART0, "FILE: No enough space for a new file.\n", 38 );
            return file_id;
        } else {
            memset( &file[ file_id ], 0, sizeof( file_t ) );
            file[ file_id ].descriptor   = FILE;
            file[ file_id ].no_of_blocks = 1;
            file[ file_id ].address      = new_data_block_address();
            file[ file_id ].active       = 1;
            set_file_name( file_id, name );
            int res = disk_wr( file_id + 1, (uint8_t*) &file[ file_id ], BLOCK_SIZE );
            if( res == 0 ) {
                return file_id;
            } else {
                print( UART0, "FILE: New file not loaded successfully!\n", 40 );
                return FILE_FAILURE;
            }
        }
    }
}

// closes a file by removing it from disk.bin
int file_close( int fd ) {
    if( fd <= MAX_FILES ) {

        memset( &file[ fd ], 0, sizeof( file_t ) );
        int res = disk_wr( fd + 1, (uint8_t*) &file[ fd ], BLOCK_SIZE );
        return res;
    } else {
        return FILE_FAILURE;
    }
}

int get_data_block_id( void* address ) {
    return ( (uint32_t) address ) - MAX_FILES - 1;
}

int get_data_block_address( int index ) {
    return MAX_FILES + index + 1;
}

void load_header() {
    memset( &header, 0, sizeof( disk_header_t ) );
    int res = disk_rd( 0, (uint8_t*) &header, BLOCK_SIZE );
    if( res == 0 ) {
        print( UART0, "DISK: Header successfully loaded!\n", 34 );
    } else {
        print( UART0, "DISK: Header loading failed!\n", 29 );
    }
}

void load_files() {
    for( int i = 0; i < MAX_FILES; i++ ) {
        memset( &file[ i ], 0, sizeof( file_t ) );
        int res = disk_rd( i + 1, (uint8_t*) &file[ i ], BLOCK_SIZE );
        // print( "File i sucessfully loaded\n", 26 );
    }
}

void load_data() {
    for( int i = 0; i < MAX_FILES; i++ ) {
        if( file[ i ].active ) {
            int data_current = get_data_block_id( file[ i ].address );
            void* data_next;
            for( int j = 0; j < file[ i ].no_of_blocks; j++ ) {
                memset( &data[ data_current ], 0, sizeof( file_t ) );
                int res = disk_rd( get_data_block_address( data_current ), 
                                   (uint8_t*) &data[ data_current ], BLOCK_SIZE);
                data_next = data[ data_current ].next_data_block;
                data_current = get_data_block_id( data_next );
            }
        }
    }
}

void file_disk_load() {
    load_header();
    load_files();
    load_data();
}

int file_write( int fd, void* x, size_t n ) {
    int data_current = get_data_block_id( file[ fd ].address );
    void* data_next;
    for( int i = 0; i < file[ fd ].no_of_blocks; i++ ) {
        if( data[ data_current ].full ) {
            data_next = data[ data_current ].next_data_block;
            data_current = get_data_block_id( data_next );
        }
    }
    // data_current is the one to write to.
    char* input = ( char* ) x;
    while( n != 0 ) {
        // char aux_current[MAX_CONTENT] = data[data_current].content;
        int size_in_use = strlen( data[ data_current ].content );
        int size_left   = MAX_CONTENT - size_in_use;
        char aux_append[ size_left + 1 ];
        memset( aux_append, 0, sizeof(aux_append) );
        if( size_left > n ) {
            strncpy( aux_append, input, n );
            n = 0;
        } else {
            strncpy( aux_append, input, size_left );
            n     -= size_left;
            input += size_left;
        }
        strcat( data[ data_current ].content, aux_append );
        if( strlen( data[ data_current ].content ) == MAX_CONTENT ) {
            data[ data_current ].full = 1;
            data[ data_current ].next_data_block = new_data_block_address();
            int res = disk_wr( get_data_block_address( data_current ), (uint8_t*) &data[ data_current ], BLOCK_SIZE );
            data_next = data[ data_current ].next_data_block;
            data_current = get_data_block_id( data_next );
            ++file[ fd ].no_of_blocks;
            res = disk_wr( fd + 1, (uint8_t*) &file[ fd ], BLOCK_SIZE );
        } else {
            int res = disk_wr( get_data_block_address( data_current ), (uint8_t*) &data[ data_current ], BLOCK_SIZE );
        }
    }
    return FILE_SUCCESS;
}

int file_read( int fd, char* x, size_t n ) {
    int data_current = get_data_block_id( file[ fd ].address );
    // memset( x, 0, sizeof( x ) );
    // strncpy( ( char* ) x, " ", 1 );
    char output[ n + 1 ];
    output[0] = '\0';
    // memset( output, 0, sizeof( output ) );
    size_t n_left = n;
    void* data_next;
    while( n_left > 0 ) {
        if ( n_left > MAX_CONTENT && data[ data_current ].full ) {
            strncat( output, data[ data_current ].content, MAX_CONTENT );
            n_left -= strlen( data[ data_current ].content );
            data_next = data[ data_current ].next_data_block;
            data_current = get_data_block_id( data_next );
        } else if( n_left > strlen( data[ data_current ].content ) ) {
            strncat( output, data[ data_current ].content, strlen( data[ data_current ].content ) );
            n_left = 0;
        } else {
            strncat( output, data[ data_current ].content, n_left );
            n_left = 0;
        }
    }
    strcpy( x, output );
    return n - n_left;

}