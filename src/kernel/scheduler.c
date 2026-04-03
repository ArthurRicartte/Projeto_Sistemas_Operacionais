#include "scheduler.h"
#include "gdt.h"
#include "fb.h"
#include "string.h"
#include "serial.h"

int current_algorithm = SCHED_RR;
int total_processes = 0;
int finished_processes = 0;

extern uint32_t timer_ticks;
extern tss_entry_t tss;

static void itoa(uint32_t n, char *buf)
{
    int i = 0;
    if (n == 0) {
        buf[i++] = '0';
        buf[i] = '\0';
        return;
    }
    char temp[32];
    int j = 0;
    while (n > 0) {
        temp[j++] = '0' + (n % 10);
        n /= 10;
    }
    while (j > 0) {
        buf[i++] = temp[--j];
    }
    buf[i] = '\0';
}

void reset_pipeline_for_next_algorithm(int new_algo)
{
    current_algorithm = new_algo;
    finished_processes = 0;
    
    if (new_algo == SCHED_SJF) {
        fb_write("\n\n[Unidade 14] Trocando para Escalonador (SJF)...\n", 50);
        serial_write(SERIAL_COM1, "\n\n[Unidade 14] Trocando para Escalonador (SJF)...\n", 50);
    } else if (new_algo == SCHED_RR) {
        fb_write("\n\n[Unidade 14] Trocando para Escalonador (RR)...\n", 49);
        serial_write(SERIAL_COM1, "\n\n[Unidade 14] Trocando para Escalonador (RR)...\n", 49);
    }

    process_t *temp = ready_queue;
    if (temp != (void *)0) {
        do {
            temp->state = PROCESS_READY;
            temp->total_exec_time = 0;
            temp->waiting_time = 0;
            temp->turnaround_time = 0;
            // Reinicia o tempo de chegada virtual para agora
            temp->arrival_time = timer_ticks; 
            temp = temp->next;
        } while (temp != ready_queue);
    }

    current_process = ready_queue;
    current_process->state = PROCESS_RUNNING;
    
    tss.esp0 = (uint32_t)current_process->esp & 0xFFFFF000;
    tss.esp0 += 4096;

    fb_write(" -> Executando PID 1\n", 21);
    serial_write(SERIAL_COM1, " -> Executando PID 1\n", 21);
}

void advanced_schedule(void)
{
    if (current_process == (void *)0) return;

    if (current_process->state == PROCESS_RUNNING) {
        current_process->total_exec_time++;

        if (current_process->total_exec_time >= current_process->burst_time) {
            current_process->state = PROCESS_TERMINATED;
            current_process->turnaround_time = timer_ticks - current_process->arrival_time;
            current_process->waiting_time = current_process->turnaround_time - current_process->burst_time;
            finished_processes++;

            if (finished_processes >= total_processes) {
                print_statistics();
                return;
            }
        }
    }

    int need_switch = 0;

    if (current_process->state == PROCESS_TERMINATED) {
        need_switch = 1;
    } else if (current_algorithm == SCHED_RR) {
        need_switch = 1; 
    } else if (current_algorithm == SCHED_FCFS || current_algorithm == SCHED_SJF) {
        need_switch = 0; 
    }

    if (need_switch) {
        if (current_process->state == PROCESS_RUNNING) {
            current_process->state = PROCESS_READY;
        }

        process_t *next_proc = current_process->next;

        if (current_algorithm == SCHED_SJF) {
            process_t *temp = ready_queue;
            process_t *best = (void *)0;
            uint32_t min_burst = 0xFFFFFFFF;
            do {
                if (temp->state == PROCESS_READY && temp->burst_time < min_burst) {
                    min_burst = temp->burst_time;
                    best = temp;
                }
                temp = temp->next;
            } while (temp != ready_queue);

            if (best != (void *)0) {
                next_proc = best;
            }
        } else {
            while (next_proc->state == PROCESS_TERMINATED) {
                next_proc = next_proc->next;
            }
        }

        current_process = next_proc;
        current_process->state = PROCESS_RUNNING;

        char buf[8];
        fb_write(" -> Executando PID ", 19);
        serial_write(SERIAL_COM1, " -> Executando PID ", 19);
        itoa(current_process->pid, buf);
        fb_write(buf, strlen(buf));
        serial_write(SERIAL_COM1, buf, strlen(buf));
        fb_write("\n", 1);
        serial_write(SERIAL_COM1, "\n", 1);

        tss.esp0 = (uint32_t)current_process->esp & 0xFFFFF000;
        tss.esp0 += 4096;
    }
}

void print_statistics(void)
{
    if (current_algorithm == SCHED_FCFS) {
        fb_write("\n--- ESTATISTICAS DE ESCALONAMENTO (FCFS) ---\n", 46);
        serial_write(SERIAL_COM1, "\n--- ESTATISTICAS DE ESCALONAMENTO (FCFS) ---\n", 46);
    } else if (current_algorithm == SCHED_SJF) {
        fb_write("\n--- ESTATISTICAS DE ESCALONAMENTO (SJF) ---\n", 45);
        serial_write(SERIAL_COM1, "\n--- ESTATISTICAS DE ESCALONAMENTO (SJF) ---\n", 45);
    } else {
        fb_write("\n--- ESTATISTICAS DE ESCALONAMENTO (RR) ---\n", 44);
        serial_write(SERIAL_COM1, "\n--- ESTATISTICAS DE ESCALONAMENTO (RR) ---\n", 44);
    }
    
    process_t *temp = ready_queue;
    uint32_t total_wait = 0, total_turnaround = 0;

    char buf[16];
    do {
        fb_write("PROCESSO ", 9);
        serial_write(SERIAL_COM1, "PROCESSO ", 9);
        itoa(temp->pid, buf);
        fb_write(buf, strlen(buf));
        serial_write(SERIAL_COM1, buf, strlen(buf));
        fb_write(" - Wait: ", 9);
        serial_write(SERIAL_COM1, " - Wait: ", 9);
        itoa(temp->waiting_time, buf);
        fb_write(buf, strlen(buf));
        serial_write(SERIAL_COM1, buf, strlen(buf));

        fb_write(" - Burst: ", 10);
        serial_write(SERIAL_COM1, " - Burst: ", 10);
        itoa(temp->burst_time, buf);
        fb_write(buf, strlen(buf));
        serial_write(SERIAL_COM1, buf, strlen(buf));

        fb_write(" - Turnaround: ", 15);
        serial_write(SERIAL_COM1, " - Turnaround: ", 15);
        itoa(temp->turnaround_time, buf);
        fb_write(buf, strlen(buf));
        serial_write(SERIAL_COM1, buf, strlen(buf));
        fb_write("\n", 1);
        serial_write(SERIAL_COM1, "\n", 1);

        total_wait += temp->waiting_time;
        total_turnaround += temp->turnaround_time;
        temp = temp->next;
    } while (temp != ready_queue && temp != (void *)0);

    if (total_processes > 0) {
        fb_write("MEDIA WAIT: ", 12);
        serial_write(SERIAL_COM1, "MEDIA WAIT: ", 12);
        itoa(total_wait / total_processes, buf);
        fb_write(buf, strlen(buf));
        serial_write(SERIAL_COM1, buf, strlen(buf));
        fb_write("\nMEDIA TURNAROUND: ", 19);
        serial_write(SERIAL_COM1, "\nMEDIA TURNAROUND: ", 19);
        itoa(total_turnaround / total_processes, buf);
        fb_write(buf, strlen(buf));
        serial_write(SERIAL_COM1, buf, strlen(buf));
    }
    fb_write("\n-------------------------------------\n", 39);
    serial_write(SERIAL_COM1, "\n-------------------------------------\n", 39);
    
    if (current_algorithm == SCHED_FCFS) {
        reset_pipeline_for_next_algorithm(SCHED_SJF);
    } else if (current_algorithm == SCHED_SJF) {
        reset_pipeline_for_next_algorithm(SCHED_RR);
    } else {
        fb_write("Testes Finalizados. Sistema em repouso (HLT).\n", 46);
        serial_write(SERIAL_COM1, "Testes Finalizados. Sistema em repouso (HLT).\n", 46);
        current_process = (void *)0;
    }
}
