#include "scheduler.h"

extern void     main_console();
extern uint32_t tos_user;

pcb_t pcb[ TABLE_SIZE ], *current = NULL;
void* current_tos_user = &tos_user;

// returns a pointer to the new(empty) stack space of given size 
void* set_user_stack( int32_t size ) {
    void* old_tos_user = current_tos_user;
    current_tos_user -= size;
    return old_tos_user;
}

// returns the lowest non-running process id
pid_t new_pid() {
    for(int i = 0; i < TABLE_SIZE; i++) {
        if(!pcb[ i ].running) {
            return i;
        }
    }
    return -1;
}

// returns the pid of currently running process
pid_t scheduler_get_pid() {
    return current->pid;
}

// returns the parent pid of currently running process
pid_t scheduler_get_pid_parent(){
    return current->pid_parent;
}

// initialises the pcb table by loading and running the console
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

// returns a pid of a running process with the highest priority
pid_t next_pid() {
    uint32_t highest_priority = 0;
    uint32_t         next_pid = 0;
    for ( int i = 0; i < TABLE_SIZE; i++ ) {
        if ( pcb[ i ].running && pcb[ i ].priority_curr > highest_priority) {
            highest_priority = pcb[ i ].priority_curr;
            next_pid = i;
        }
    }
    return next_pid;
}

// updates the priority of all running processes 
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

// performs a context switch between two running processes
void scheduler_run( ctx_t* ctx ) {
    update_priority();
    pid_t next = next_pid();

    memcpy( &( current->ctx ), ctx, sizeof( ctx_t ) );
    memcpy(  ctx, &pcb[ next ].ctx, sizeof( ctx_t ) );

    current = &pcb[ next ];
}

// creates a new child process with the same context,
// new empty stack space and priority high enough to
// ensure the child process is run next.
pid_t scheduler_fork( ctx_t* ctx ) {
    pid_t new = new_pid();
    if( new == -1 ) {
        print( UART0, "SCHEDULER: Maximum processes already running\n", 45 );
        return new;
    }
    memcpy( &pcb[ new ], current, sizeof( pcb_t ));
    memcpy( &pcb[ new ].ctx, ctx, sizeof( ctx_t ));
    pcb[ new ].pid           = new;
    pcb[ new ].pid_parent    = current->pid;
    pcb[ new ].ctx.gpr[ 0 ]  = 0;
    pcb[ new ].ctx.cpsr      = 0x50;
    pcb[ new ].ctx.sp        = ( uint32_t ) set_user_stack( 5000 );
    pcb[ new ].running       = 1;
    pcb[ new ].priority_curr = FORK_PRIORITY;

    return pcb[ new ].pid;
}

// replaces a current process image with a new one by loading
// the address of the new process in the pc value
void scheduler_exec( ctx_t* ctx ) {
    uint32_t pc            = ( uint32_t )( ctx->gpr[ 0 ] );
    uint32_t priority      = ( uint32_t )( ctx->gpr[ 1 ] );

    current->ctx.pc        = pc;
    current->priority_init = priority;
    current->priority_curr = priority;

    memcpy( ctx, &(current->ctx), sizeof( ctx_t ));
}

// causes a process to terminate, removing it from the pcb table
void scheduler_exit( ctx_t* ctx ) {
    int32_t exit_code = ( int32_t )( ctx->gpr[0] );
    if( exit_code == 0 ) {  // EXIT_SUCCESS
        current->running = 0;
        current = &pcb[ 0 ];
        memcpy( ctx, &( current->ctx ), sizeof( ctx_t ) );
    }
}

// forces a process to terminate, removing it from the pcb table
int scheduler_kill( pid_t pid, int sig ) {
    if( pid == -1 ) {   // kill all
        for( int i = 0; i < TABLE_SIZE; i++ ) {
            if( pcb[ i ].pid != 0 ) {
                pcb[ i ].running = 0;
            }
        }
        return KILL_SUCCESS;
    } else {
        for( int i = 0; i < TABLE_SIZE; i++ ) {
            if( pcb[ i ].pid == pid ) {
                pcb[ i ].running = 0;
                return KILL_SUCCESS;
            }
        }
        return KILL_FAILURE;
    }
}
