#include "pit.h"
#include "io.h"
#include "fb.h"
#include "process.h"
#include "pic.h"

#include "scheduler.h"

#define PIT_COMMAND_PORT 0x43
#define PIT_CHANNEL0_DATA 0x40

volatile uint32_t timer_ticks = 0;

// Handler chamado a cada interrupção do timer (IRQ0)
void timer_handler(void)
{
    timer_ticks++;

    // Envia o EOI (End of Interrupt) para o PIC
    // 32 é o número da interrupção mapeada para o IRQ0
    pic_acknowledge(32);

    // Chama o escalonador avançado para trocar de processo
    advanced_schedule();
}

// Função que configura o PIT e ativa o escalonamento
void init_preemptive_scheduler(uint32_t frequency, int algorithm)
{
    current_algorithm = algorithm;
    // O divisor para o PIT (1.193182 MHz / frequência desejada)
    uint32_t divisor = 1193180 / frequency;

    // Envia o comando para o PIT: Channel 0, lobyte/hibyte, Square Wave, Binary
    outb(PIT_COMMAND_PORT, 0x36);

    // Envia o divisor em duas etapas
    outb(PIT_CHANNEL0_DATA, (uint8_t)(divisor & 0xFF));
    outb(PIT_CHANNEL0_DATA, (uint8_t)((divisor >> 8) & 0xFF));

    // Desativa a máscara no PIC para permitir interrupções do Timer (IRQ0)
    pic_clear_mask(0);
}