#include "idt.h"
#include "process.h"
#include "pic.h"

// Estruturas internas
idt_entry_t idt[256];
idt_ptr_t idt_ptr;

// Protótipos externos do Assembly (interrupts.s)
extern void load_idt(uint32_t);
extern uint32_t interrupt_handlers[256];

void idt_set_gate(uint8_t num, uint32_t base, uint16_t selector, uint8_t flags)
{
    idt[num].base_low = (base & 0xFFFF);
    idt[num].base_high = (base >> 16) & 0xFFFF;
    idt[num].selector = selector;
    idt[num].always0 = 0;
    idt[num].flags = flags;
}

void init_idt(void)
{
    idt_ptr.limit = (sizeof(idt_entry_t) * 256) - 1;
    idt_ptr.base = (uint32_t)&idt;

    // Preenche a IDT usando os endereços gerados no interrupts.s
    for (int i = 0; i < 256; i++)
    {
        idt_set_gate(i, interrupt_handlers[i], 0x08, 0x8E);
    }

    load_idt((uint32_t)&idt_ptr);
}

uint32_t interrupt_handler(cpu_state_t *cpu)
{
    // 1. Salva contexto atual
    if (current_process != (void *)0)
    {
        current_process->esp = (uint32_t)cpu;
    }

    // 2. Trata Timer ou Yield
    if (cpu->int_no == 32)
    {
        pic_acknowledge(32);
        schedule_preemptive();
    }
    else if (cpu->int_no >= 32 && cpu->int_no <= 47)
    {
        pic_acknowledge(cpu->int_no);
    }

    // 3. Retorna nova pilha para o Assembly fazer o mov esp, eax
    if (current_process != (void *)0)
    {
        return current_process->esp;
    }
    return (uint32_t)cpu;
}