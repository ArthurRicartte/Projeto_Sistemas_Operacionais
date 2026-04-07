#ifndef IDT_H
#define IDT_H

#include "process.h" // Garante que cpu_state_t seja conhecido

// Estrutura de uma entrada da IDT (8 bytes)
struct idt_entry_struct
{
    unsigned short base_low;
    unsigned short selector;
    unsigned char always0;
    unsigned char flags;
    unsigned short base_high;
} __attribute__((packed));
typedef struct idt_entry_struct idt_entry_t;

struct idt_ptr_struct
{
    unsigned short limit;
    unsigned int base;
} __attribute__((packed));
typedef struct idt_ptr_struct idt_ptr_t;

// Funções públicas
void init_idt(void);
void idt_set_gate(uint8_t num, uint32_t base, uint16_t selector, uint8_t flags);

uint32_t interrupt_handler(cpu_state_t *cpu);

#endif