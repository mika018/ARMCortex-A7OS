#include "ipc.h"

#define EMPTY 123456

pipe_t pipe[ NO_OF_PIPES ];

void ipc_initialise() {
    for(int i = 0; i < NO_OF_PIPES; i++) {
        memset( &pipe[ 0 ], 0, sizeof( pipe_t ) );
        pipe[ i ].pipe_id = i;
    }
}

int new_pipe_id() {
    for( int i = 0; i < NO_OF_PIPES; i++ ) {
        if( !pipe[ i ].active ) return pipe[ i ].pipe_id;
    }
    return -1;
}

void buffer_initialise( buffer_t *buffer, pid_t pid ) {
    buffer->pid    = pid;
    buffer->signal = EMPTY;
}

int new_pipe( pid_t pid_1, pid_t pid_2 ) {
    int id = new_pipe_id();
    if( id != -1 ) {
        buffer_initialise( &pipe[ id ].buff_1, pid_1 );
        buffer_initialise( &pipe[ id ].buff_2, pid_2 );
        pipe[ id ].active = 1;
    } else {
        print("DEBUG ipc_new_pipe: All pipes busy\n", 35);
    }
    return id;
}

// checks if there exist a currently active pipe if not it creates a new one.
// returns the pipe index
int ipc_pipe( pid_t pid_1, pid_t pid_2 ) {
    for(int i = 0; i < NO_OF_PIPES; i++) {
        if( pipe[ i ].active ) {
            if( pipe[ i ].buff_1.pid == pid_1 && pipe[ i ].buff_2.pid == pid_2
             || pipe[ i ].buff_1.pid == pid_2 && pipe[ i ].buff_2.pid == pid_1) {
                 return i;
             } 
        }
    }
    int new = new_pipe( pid_1, pid_2 );
    return new;
}

buffer_t* find_buffer( pipe_t* pipe, pid_t pid ) {
    if( pipe->buff_1.pid == pid ) {
        return &( pipe->buff_1 );
    } else if( pipe->buff_2.pid == pid ) {
        return &( pipe->buff_2 );
    } else {
        print("DEBUG ipc_find_buffer: Wrong pipe selected\n", 43);
    }
} 

// if inactive debug, if your buffer full debug, if receiver buffer full debug
void ipc_send_message( int pipe_id, pid_t pid_src, pid_t pid_des, int sig ) {
    buffer_t* buff_src = find_buffer( &pipe[ pipe_id ], pid_src);
    buffer_t* buff_des = find_buffer( &pipe[ pipe_id ], pid_des);
    if( !pipe[ pipe_id ].active ) {
        print("DEBUG ipc_send_message: Selected pipe inactive\n", 47);
        return;
    } 
    if( buff_src->signal != EMPTY ) {
        print("DEBUG ipc_send_message: Sender's buffer is full\n", 48);
        return;
    }
    if( buff_des->signal != EMPTY ) {
        print("DEBUG ipc_send_message: Receiver's buffer is full\n", 50);
        return;
    }
    buff_des->signal = sig;
}

int ipc_receive_message( int pipe_id, pid_t pid_des ) {
    buffer_t* buff_des = find_buffer( &pipe[ pipe_id ], pid_des);
    if( buff_des->signal == EMPTY ) {
        print("DEBUG ipc_receive_message: Nothing to receive. Buffer empty\n", 60);
    }
    int result = buff_des->signal;
    buff_des->signal = EMPTY;
    return result;
}
