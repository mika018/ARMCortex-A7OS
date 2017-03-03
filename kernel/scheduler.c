#include "scheduler.h"

pid_t next_pid = 1;

pcb_t pcb[ table_size ], *current = NULL;

extern void     main_console();
extern uint32_t tos_console;
extern void     main_P3();
extern uint32_t tos_P3;
extern void     main_P4();
extern uint32_t tos_P4;
extern void     main_P5();
extern uint32_t tos_P5;

pid_t new_pid() {
    for(int i = 0; i < table_size; i++) {
        if(!pcb[ i ].running) {
            return i;
        }
    }
    return -1;
}

void scheduler_initialise(ctx_t* ctx) {
    memset( &pcb[ 0 ], 0, sizeof( pcb_t ) );
    pcb[ 0 ].pid       = new_pid();
    pcb[ 0 ].ctx->cpsr = 0x50;
    pcb[ 0 ].ctx->pc   = ( uint32_t )( &main_console );
    pcb[ 0 ].ctx->sp   = ( uint32_t )( &tos_console  );
    pcb[ 0 ].running   = 1;

    for (int i = 1; i < table_size - 1; i++) {
        memset( &pcb[ i ], 0, sizeof( pcb_t ) );
        pcb[ i ].pid       = 100;
        pcb[ i ].ctx->cpsr = 0xFF;
        pcb[ i ].ctx->pc   = 0xFF;
        pcb[ i ].ctx->sp   = 0xFF;
        pcb[ i ].running   = 0;
    }

    current = &pcb[ 0 ]; memcpy( ctx, &current->ctx, sizeof( ctx_t ) );
}

pcb_t next_process() {
    pid_t current_pid = current->pid;
    for (int i = current_pid; i < 99 + current_pid; i++) {
        int pid_next = i % 99;
        if (pcb[ pid_next ].pid != 100) return pcb[ pid_next ];
    }
    return *current;
}

void scheduler_run(ctx_t* ctx) {
    pcb_t next = next_process();

    memcpy( current->ctx, ctx, sizeof( ctx_t ) );
    memcpy( ctx, next.ctx, sizeof( ctx_t ) );

    current = &next;
}

/*
pcb_t* find_pcb(pid_t pid, pcb_table* table) {
    pcb_t* pcb = table->head;
    for(int i = 0; i < next_pid; i++) {
        if (pcb->pid == pid) return pcb;
        pcb = pcb->next_pcb;
    }
    return NULL;
}

void insert_pcb(ctx_t* ctx, pcb_table* table) {
    pcb_t* pcb = new_pcb(ctx);
    pcb->next_pcb = table->head->next_pcb;
    table->head->next_pcb = pcb;
}

 */