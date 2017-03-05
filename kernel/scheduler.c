#include "scheduler.h"

// pid_t next_pid = 1;

extern void     main_console();

extern uint32_t tos_user;

pcb_t pcb[ table_size ], *current = NULL;
void* current_tos_user = &tos_user;

void* set_user_stack( int32_t size ) {
    void* old_tos_user = current_tos_user;
    current_tos_user -= size;
    return old_tos_user;
}

pid_t new_pid() {
    for(int i = 0; i < table_size; i++) {
        if(!pcb[ i ].running) {
            return i;
        }
    }
    return -1;
}

void scheduler_initialise( ctx_t* ctx ) {
    memset( &pcb[ 0 ], 0, sizeof( pcb_t ) );
    pcb[ 0 ].pid       = new_pid();
    pcb[ 0 ].ctx.cpsr  = 0x50;
    pcb[ 0 ].ctx.pc    = ( uint32_t ) ( &main_console );
    pcb[ 0 ].ctx.sp    = ( uint32_t ) set_user_stack( 1000 );
    pcb[ 0 ].running   = 1;

    for (int i = 1; i < table_size - 1; i++) {
        memset( &pcb[ i ], 0, sizeof( pcb_t ) );
        pcb[ i ].pid       = 100;
        pcb[ i ].ctx.cpsr  = 0xFF;
        pcb[ i ].ctx.pc    = 0xFF;
        pcb[ i ].ctx.sp    = 0xFF;
        pcb[ i ].running   = 0;
    }

    current = &pcb[ 0 ]; memcpy( ctx, &current->ctx, sizeof( ctx_t ) );
}

pcb_t next_process() {
    pid_t current_pid = current->pid;
    for ( int i = current_pid + 1; i < table_size + current_pid; i++ ) {
        int pid_next = i % table_size;
        if ( pcb[ pid_next ].running ) return pcb[ pid_next ];
    }
    return *current;
}

void scheduler_run( ctx_t* ctx ) {
    pcb_t next = next_process();

    memcpy( &current->ctx, ctx, sizeof( ctx_t ) );
    memcpy( ctx, &next.ctx, sizeof( ctx_t ) );

    current = &next;
}

pid_t scheduler_fork( ctx_t* ctx ) {
    pid_t new = new_pid();
    // if new_pid == -1 then no enough space
    memcpy( &pcb[ new ], current, sizeof( pcb_t ));
    // memcpy( &pcb[ new ].ctx, ctx, sizeof( ctx_t ));

    pcb[ new ].pid          = new;
    pcb[ new ].ctx.gpr[ 0 ] = 0;
    pcb[ new ].ctx.sp       = ( uint32_t ) set_user_stack( 1000 );
    pcb[ new ].running      = 0;

    return pcb[ new ].pid;
}

void scheduler_exec( uint32_t pc ) {
    current->ctx.pc   = pc;
    current->running  = 1;
} 
