#ifndef PIT_H
#define PIT_H

#include "multiboot.h"

/* Inicializa o PIT com intervalo em milissegundos */
void pit_init(uint32_t ms);

/* Handler do timer (chamado pela interrupção) */
void timer_handler(void);

void init_preemptive_scheduler(uint32_t frequency, int algorithm);

#endif