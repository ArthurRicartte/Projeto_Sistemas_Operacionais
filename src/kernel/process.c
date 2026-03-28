#include "process.h"
#include "kheap.h"
#include "pmm.h"
#include "string.h"
#include "pit.h"
#include "gdt.h"

process_t *current_process = (void *)0;
process_t *ready_queue = (void *)0;
static uint32_t next_pid = 1;
extern uint32_t timer_ticks;
extern tss_entry_t tss;

void create_process(uint32_t entry_point, char *name)
{
    process_t *proc = (process_t *)kmalloc(sizeof(process_t));
    uint32_t stack_phys = (uint32_t)pmm_alloc_page();
    uint32_t *stack = (uint32_t *)(stack_phys + 0xC0000000 + 4096);

    // [Contexto de Hardware para Ring 3]
    *(--stack) = 0x23; // SS (User Data + RPL 3)

    // Ajuste para evitar erro de sequence-point do GCC
    stack = stack - 1;
    *stack = (uint32_t)(stack + 1); // ESP do usuário

    *(--stack) = 0x202;       // EFLAGS (IF=1)
    *(--stack) = 0x1B;        // CS (User Code + RPL 3)
    *(--stack) = entry_point; // EIP

    // [Interrupt Stub + Registers]
    *(--stack) = 0; // err
    *(--stack) = 0; // int_no
    for (int i = 0; i < 8; i++)
        *(--stack) = 0; // eax...edi

    // [Segments para User Mode]
    for (int i = 0; i < 4; i++)
        *(--stack) = 0x23; // ds, es, fs, gs

    proc->esp = (uint32_t)stack;
    proc->pid = next_pid++;
    proc->state = PROCESS_READY;
    proc->arrival_time = timer_ticks;
    proc->burst_time = 0;

    int i;
    for (i = 0; i < 31 && name[i] != '\0'; i++)
    {
        proc->name[i] = name[i];
    }
    proc->name[i] = '\0';

    // Lista Circular (Round Robin)
    if (ready_queue == (void *)0)
    {
        ready_queue = proc;
        proc->next = proc;
    }
    else
    {
        process_t *temp = ready_queue;
        while (temp->next != ready_queue)
            temp = temp->next;
        temp->next = proc;
        proc->next = ready_queue;
    }
}

void schedule_preemptive(void)
{
    if (current_process != (void *)0)
    {
        current_process->state = PROCESS_READY;
        current_process = current_process->next;
        current_process->state = PROCESS_RUNNING;

        // Atualiza a TSS para que o próximo processo tenha sua própria pilha de Kernel
        tss.esp0 = (uint32_t)current_process->esp & 0xFFFFF000;
        tss.esp0 += 4096;
    }
}