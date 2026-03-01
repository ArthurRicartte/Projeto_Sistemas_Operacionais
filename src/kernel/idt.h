#ifndef IDT_H
#define IDT_H

// Estrutura de uma entrada da IDT (8 bytes)
struct idt_entry_struct
{
    unsigned short base_low;  // Endereço do handler (bits 0-15)
    unsigned short selector;  // Seletor do segmento de código (0x08)
    unsigned char always0;    // Sempre zero
    unsigned char flags;      // Flags: 0x8E = presente, ring0, interrupt gate
    unsigned short base_high; // Endereço do handler (bits 16-31)
} __attribute__((packed));
typedef struct idt_entry_struct idt_entry_t;

// Estrutura do ponteiro da IDT (usada pela instrução LIDT)
struct idt_ptr_struct
{
    unsigned short limit; // Tamanho total da IDT - 1
    unsigned int base;    // Endereço base da IDT
} __attribute__((packed));
typedef struct idt_ptr_struct idt_ptr_t;

// Estado dos registradores salvos pelo PUSHA (na ordem da pilha)
struct cpu_state
{
    unsigned int edi;
    unsigned int esi;
    unsigned int ebp;
    unsigned int esp; // ESP original antes do PUSHA
    unsigned int ebx;
    unsigned int edx;
    unsigned int ecx;
    unsigned int eax;
} __attribute__((packed));

// Estado da pilha (código de erro e registradores empurrados pela CPU)
struct stack_state
{
    unsigned int error_code;
    unsigned int eip;
    unsigned int cs;
    unsigned int eflags;
} __attribute__((packed));

// Funções públicas
void init_idt(void);
void idt_set_gate(int num, unsigned int base, unsigned short selector, unsigned char flags);
void interrupt_handler(struct cpu_state *cpu, struct stack_state *stack, unsigned int interrupt);

#endif