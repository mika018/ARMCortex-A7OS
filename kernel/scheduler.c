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

pid_t scheduler_get_pid_parent(){
    return current->pid_parent;
}

void scheduler_initialise( ctx_t* ctx ) {
    memset( &pcb[ 0 ], 0, sizeof( pcb_t ) );
    pcb[ 0 ].pid            = 0;
    pcb[ 0 ].pid_parent     = 0;
    pcb[ 0 ].ctx.cpsr       = 0x50;
    pcb[ 0 ].ctx.pc         = ( uint32_t ) ( &main_console );
    pcb[ 0 ].ctx.sp         = ( uint32_t ) set_user_stack( 5000 );
    pcb[ 0 ].running        = 1;
    pcb[ 0 ].priority_init  = 2;
    pcb[ 0 ].priority_curr  = 2;

    for (int i = 1; i < TABLE_SIZE; i++) {
        memset( &pcb[ i ], 0, sizeof( pcb_t ) );
    }

    current = &pcb[ 0 ]; memcpy( ctx, &current->ctx, sizeof( ctx_t ) );
}

pid_t next_pid() {
    // pid_t current_pid = current->pid;
    uint32_t highest_priority = 0;
    uint32_t next_pid = 0;
    for ( int i = 0; i < TABLE_SIZE; i++ ) {
        if ( pcb[ i ].running && pcb[ i ].priority_curr > highest_priority) {
            highest_priority = pcb[ i ].priority_curr;
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
            pcb[ pid_next ].priority_curr += pcb[ pid_next ].priority_init;
        }
    }
    current->priority_curr = current->priority_init;
}

void scheduler_run( ctx_t* ctx ) {
    update_priority();
    pid_t next = next_pid();

    memcpy( &(current->ctx), ctx, sizeof( ctx_t ) );
    memcpy( ctx, &pcb[ next ].ctx, sizeof( ctx_t ) );

    current = &pcb[ next ];
}

pid_t scheduler_fork( ctx_t* ctx ) {
    pid_t new = new_pid();
    // if new_pid == -1 then no enough space
    memcpy( &pcb[ new ], current, sizeof( pcb_t ));
    memcpy( &pcb[ new ].ctx, ctx, sizeof( ctx_t ));
    pcb[ new ].pid           = new;
    pcb[ new ].pid_parent    = current->pid;
    pcb[ new ].ctx.gpr[ 0 ]  = 0;
    pcb[ new ].ctx.cpsr      = 0x50;
    pcb[ new ].ctx.sp        = ( uint32_t ) set_user_stack( 5000 );
    pcb[ new ].running       = 1;
    pcb[ new ].priority_curr = UINT32_MAX;

    return pcb[ new ].pid;
}

void scheduler_exec( ctx_t* ctx ) {
    uint32_t pc            = ( uint32_t )( ctx->gpr[ 0 ] );
    uint32_t priority      = ( uint32_t )( ctx->gpr[ 1 ] );
    current->ctx.pc        = pc;
    current->priority_init = priority;
    current->priority_curr = priority;
    memcpy( ctx, &(current->ctx), sizeof( ctx_t ));
}

void scheduler_exit( ctx_t* ctx ) {
    int32_t exit_code = ( int32_t )( ctx->gpr[0] );
    if( exit_code == 0) {  // EXIT_SUCCESS
        current->running = 0;
        // scheduler_run( ctx );
        current = &pcb[ 0 ];
        memcpy( ctx, &(current->ctx), sizeof( ctx_t ) );
    }
}

int scheduler_kill( pid_t pid, int sig ) {
    for( int i = 0; i < TABLE_SIZE; i++ ) {
        if( pcb[ i ].pid == pid ) {
            pcb[ i ].running = 0;
            return 0;
        }
    }
    return -1;
}
