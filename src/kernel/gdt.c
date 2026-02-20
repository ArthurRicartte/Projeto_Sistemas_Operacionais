// gdt.c - Implementação da Global Descriptor Table
// Desenvolvido para o Capítulo 5

#include "gdt.h"

// O nosso array com 3 segmentos (Nulo, Código, Dados)
gdt_entry_t gdt[3];

// O ponteiro que vai dizer ao processador onde a GDT está
gdt_ptr_t gdt_ptr;

// Declaramos a função Assembly que vamos criar a seguir
extern void gdt_flush(unsigned int);

// Função para preencher uma entrada da GDT de forma simplificada
void gdt_set_gate(int num, unsigned int base, unsigned int limit, unsigned char access, unsigned char gran) {
    // Configurar as 3 partes da Base
    gdt[num].base_low    = (base & 0xFFFF);
    gdt[num].base_middle = (base >> 16) & 0xFF;
    gdt[num].base_high   = (base >> 24) & 0xFF;

    // Configurar o Limite (parte baixa e parte alta/granularidade)
    gdt[num].limit_low   = (limit & 0xFFFF);
    gdt[num].granularity = ((limit >> 16) & 0x0F);

    // Juntar a granularidade com os bits de flag (gran)
    gdt[num].granularity |= (gran & 0xF0);
    
    // Configurar as permissões de acesso
    gdt[num].access      = access;
}

// Função principal que inicializa a nossa nova GDT
void init_gdt(void) {
    // Configurar o ponteiro (tamanho total da tabela e o endereço de memória)
    gdt_ptr.limit = (sizeof(gdt_entry_t) * 3) - 1;
    gdt_ptr.base  = (unsigned int)&gdt;

    // Entrada 0: Segmento Nulo (obrigatório pela Intel)
    gdt_set_gate(0, 0, 0, 0, 0);

    // Entrada 1: Segmento de Código (Ring 0 / Kernel)
    // Base: 0 | Limite: 4GB (0xFFFFFFFF)
    // Acesso: 0x9A (Presente, Privilégio 0, Executável, Leitura)
    // Granularidade: 0xCF (Tamanho de 4KB, 32-bits)
    gdt_set_gate(1, 0, 0xFFFFFFFF, 0x9A, 0xCF);

    // Entrada 2: Segmento de Dados (Ring 0 / Kernel)
    // Base: 0 | Limite: 4GB (0xFFFFFFFF)
    // Acesso: 0x92 (Presente, Privilégio 0, Dados, Leitura/Escrita)
    // Granularidade: 0xCF (Tamanho de 4KB, 32-bits)
    gdt_set_gate(2, 0, 0xFFFFFFFF, 0x92, 0xCF);

    // Carregar a nova GDT passando o endereço do nosso ponteiro para o Assembly
    gdt_flush((unsigned int)&gdt_ptr);
}