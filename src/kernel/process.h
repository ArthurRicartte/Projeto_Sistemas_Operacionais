#ifndef PROCESS_H
#define PROCESS_H

#include "multiboot.h"
#define PROCESS_READY 0
#define PROCESS_RUNNING 1
#define PROCESS_TERMINATED 2

#define SCHED_FCFS 0
#define SCHED_SJF 1
#define SCHED_RR 2

typedef struct cpu_state
{
    uint32_t gs, fs, es, ds;
    uint32_t edi, esi, ebp, esp_dummy, ebx, edx, ecx, eax;
    uint32_t int_no, err_code;
    uint32_t eip, cs, eflags;
} __attribute__((packed)) cpu_state_t;

typedef struct process
{
    uint32_t esp; // Stack pointer salvo
    uint32_t pid;
    uint32_t state;

    //(Estatísticas e Algoritmos)
    uint32_t arrival_time;    // Momento da criação
    uint32_t burst_time;      // Tempo estimado de execução (SJF)
    uint32_t total_exec_time; // Quanto tempo já rodou

    // Estatisticas finais
    uint32_t waiting_time;
    uint32_t turnaround_time;

    struct process *next;
    char name[32];
} process_t;

extern process_t *current_process;
extern process_t *ready_queue;
extern int current_algorithm;

void create_process(uint32_t entry_point, char *name, uint32_t burst_time);
void init_preemptive_scheduler(uint32_t quantum_ms, int algorithm);
void schedule_preemptive(void);
void print_statistics(void);

#endif