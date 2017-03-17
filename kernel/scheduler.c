#include "scheduler.h"
#include "PL011.h"

// pid_t next_pid = 1;

extern void     main_console();
extern uint32_t tos_user;
// extern uint32_t tos_console;
// extern uint32_t tos_P3;

pcb_t pcb[ TABLE_SIZE ], *current = NULL;
void* current_tos_user = &tos_user;

void* set_user_stack( int32_t size ) {
    void* old_tos_user = current_tos_user;
    current_tos_user -= size;
    return old_tos_user;
}

pid_t new_pid() {
    for(int i = 0; i < TABLE_SIZE; i++) {
        if(!pcb[ i ].running) {
            return i;
        }
    }
    return -1;
}

pid_t scheduler_get_pid() {
    return current->pid;
}

void scheduler_initialise( ctx_t* ctx ) {
    memset( &pcb[ 0 ], 0, sizeof( pcb_t ) );
    pcb[ 0 ].pid            = new_pid();
    pcb[ 0 ].ctx.cpsr       = 0x50;
    pcb[ 0 ].ctx.pc         = ( uint32_t ) ( &main_console );
    pcb[ 0 ].ctx.sp         = ( uint32_t ) set_user_stack( 5000 );
    pcb[ 0 ].running        = 1;
    pcb[ 0 ].init_priority  = 2;
    pcb[ 0 ].curr_priority  = 2;

    for (int i = 1; i < TABLE_SIZE; i++) {
        memset( &pcb[ i ], 0, sizeof( pcb_t ) );
        pcb[ i ].pid            = 100;
        pcb[ i ].ctx.cpsr       = 0xFF;
        pcb[ i ].ctx.pc         = 0xFF;
        pcb[ i ].ctx.sp         = 0xFF;
        pcb[ i ].running        = 0;
        pcb[ i ].init_priority  = 0;
        pcb[ i ].curr_priority  = 0;
    }

    current = &pcb[ 0 ]; memcpy( ctx, &current->ctx, sizeof( ctx_t ) );
}

pid_t next_pid() {
    // pid_t current_pid = current->pid;
    uint32_t highest_priority = 0;
    uint32_t next_pid = 0;
    for ( int i = 0; i < TABLE_SIZE; i++ ) {
        if ( pcb[ i ].running && pcb[ i ].curr_priority > highest_priority) {
            highest_priority = pcb[ i ].curr_priority;
            next_pid = i;
        }
    }
    return next_pid;
}

void update_priority() {
    pid_t current_pid = current->pid;
    for ( int i = current_pid + 1; i < TABLE_SIZE + current_pid; i++ ) {
        int pid_next = i % TABLE_SIZE;
        if ( pcb[ pid_next ].running ) {
            pcb[ pid_next ].curr_priority *= 2;
        }
    }
    current->curr_priority = current->init_priority;
}

void scheduler_run( ctx_t* ctx ) {
    update_priority();
    pid_t next = next_pid();

    memcpy( &(current->ctx), ctx, sizeof( ctx_t ) );
    memcpy( ctx, &pcb[ next ].ctx, sizeof( ctx_t ) );

    current = &pcb[ next];
}

pid_t scheduler_fork( ctx_t* ctx ) {
    pid_t new = new_pid();
    // if new_pid == -1 then no enough space
    memcpy( &pcb[ new ], current, sizeof( pcb_t ));
    memcpy( &pcb[ new ].ctx, ctx, sizeof( ctx_t ));
    pcb[ new ].pid           = new;
    pcb[ new ].ctx.gpr[ 0 ]  = 0;
    pcb[ new ].ctx.cpsr      = 0x50;
    pcb[ new ].ctx.sp        = ( uint32_t ) set_user_stack( 5000 );
    pcb[ new ].running       = 1;
    pcb[ new ].curr_priority = UINT32_MAX;

    return pcb[ new ].pid;
}

void scheduler_exec( ctx_t* ctx ) {
    uint32_t pc            = ( uint32_t )( ctx->gpr[ 0 ] );
    uint32_t priority      = ( uint32_t )( ctx->gpr[ 1 ] );
    current->ctx.pc        = pc;
    current->init_priority = priority;
    current->curr_priority = priority;
    memcpy( ctx, &(current->ctx), sizeof( ctx_t ));
}

void scheduler_exit( ctx_t* ctx ) {
    int32_t exit_code = ( int32_t )( ctx->gpr[0] );
    if ( exit_code == 0) {  // EXIT_SUCCESS
        current->running = 0;
        // scheduler_run( ctx );
        current = &pcb[ 0 ];
        memcpy( ctx, &(current->ctx), sizeof( ctx_t ) );
    }
}
