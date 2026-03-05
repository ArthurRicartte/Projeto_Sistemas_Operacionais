// gdt.h - Estruturas para a Global Descriptor Table
#ifndef GDT_H
#define GDT_H

// Estrutura de uma entrada da GDT (8 bytes)
struct gdt_entry_struct {
    unsigned short limit_low;   // Os 16 bits mais baixos do limite
    unsigned short base_low;    // Os 16 bits mais baixos da base
    unsigned char  base_middle; // Os próximos 8 bits da base
    unsigned char  access;      // Byte de acesso (determina o anel de privilégio, se é código/dados)
    unsigned char  granularity; // 4 bits altos do limite + 4 bits de flags
    unsigned char  base_high;   // Os últimos 8 bits da base
} __attribute__((packed));

typedef struct gdt_entry_struct gdt_entry_t;

// Estrutura do ponteiro da GDT (usado pela instrução lgdt em Assembly)
struct gdt_ptr_struct {
    unsigned short limit; // Tamanho de todas as entradas da GDT somadas - 1
    unsigned int   base;  // Endereço de memória onde a primeira entrada da GDT está
} __attribute__((packed));

typedef struct gdt_ptr_struct gdt_ptr_t;

// Função que vamos chamar no kmain.c para inicializar tudo
void init_gdt(void);

#endif