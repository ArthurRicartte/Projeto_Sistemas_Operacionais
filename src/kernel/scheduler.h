#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "process.h"

#define SCHED_FCFS 0
#define SCHED_SJF 1
#define SCHED_RR 2

extern int current_algorithm;
extern int total_processes;
extern int finished_processes;

void advanced_schedule(void);
void print_statistics(void);

#endif
