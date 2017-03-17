#include "ipc.h"
// #include "PL011.h"

message messages[ MESS_TABLE_SIZE ];

void ipc_initialise() {
    for (int i = 0; i < MESS_TABLE_SIZE; i++) {
        memset( &messages[ 0 ], 0, sizeof( message ) );
        messages[ i ].pid_src = 0;
        messages[ i ].pid_des = 0;
        messages[ i ].signal  = 0;
        messages[ i ].active  = 0;
    }
}

int new_message_id() {
    for ( int i = 0; i < MESS_TABLE_SIZE; i++ ) {
        if ( !messages[ i ].active ) return i;
    }
    return -1;
}

int find_message_for( int destination ) {
    for ( int i = 0; i < MESS_TABLE_SIZE; i++ ) {
        if ( messages[ i ].pid_des == destination && messages[ i ].active) return i;
    }
    return -1;
}

//change to check if dest is still waiting.
void ipc_send_message( int source, int destination, int signal ) {
    int id = new_message_id();
    if ( id !=  -1 ) { 
        messages[ id ].pid_src = source;
        messages[ id ].pid_des = destination;
        messages[ id ].signal  = signal;
        messages[ id ].active  = 1;
    }
}

int ipc_receive_message( int destination ) {
    int id = find_message_for( destination );
    if ( id != -1 ) {
        messages[ id ].active = 0;
        return messages[ id ].signal;
    }
    return id;
}
