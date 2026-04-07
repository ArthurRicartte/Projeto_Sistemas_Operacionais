#include "gdt.h"
#include "multiboot.h" // Para uint32_t, uint16_t, etc.

// array agora com 6 segmentos:
// 0:Nulo, 1:KCode, 2:KData, 3:UCode, 4:UData, 5:TSS
gdt_entry_t gdt[6];

// O ponteiro que vai dizer ao processador onde a GDT está
gdt_ptr_t gdt_ptr;

// Estrutura global da TSS (Task State Segment)
tss_entry_t tss;

// Declaramos a função Assembly que recarrega os registradores de segmento
extern void gdt_flush(unsigned int);

// Função para preencher uma entrada da GDT de forma simplificada
void gdt_set_gate(int num, unsigned int base, unsigned int limit, unsigned char access, unsigned char gran)
{
    // Configurar as 3 partes da Base
    gdt[num].base_low = (base & 0xFFFF);
    gdt[num].base_middle = (base >> 16) & 0xFF;
    gdt[num].base_high = (base >> 24) & 0xFF;

    // Configurar o Limite (parte baixa e parte alta/granularidade)
    gdt[num].limit_low = (limit & 0xFFFF);
    gdt[num].granularity = ((limit >> 16) & 0x0F);

    // Juntar a granularidade com os bits de flag (gran)
    gdt[num].granularity |= (gran & 0xF0);

    // Configurar as permissões de acesso (determina Ring 0 ou Ring 3)
    gdt[num].access = access;
}

// Função para configurar a TSS na GDT
void write_tss(int num, uint16_t ss0, uint32_t esp0)
{
    uint32_t base = (uint32_t)&tss;
    uint32_t limit = sizeof(tss);

    // Adiciona a entrada na GDT (Acesso 0xE9 = Presente, Ring 3, TSS Disponível)
    gdt_set_gate(num, base, limit, 0xE9, 0x00);

    // Limpa a estrutura para evitar lixo de memória
    for (int i = 0; i < (int)sizeof(tss); i++)
    {
        ((char *)&tss)[i] = 0;
    }

    tss.ss0 = ss0;                // Segmento de pilha do kernel (geralmente 0x10)
    tss.esp0 = esp0;              // Endereço da pilha que o CPU usará ao sair do Ring 3
    tss.iomap_base = sizeof(tss); // Bloqueia acesso direto a portas I/O no modo usuário
}

// Função principal que inicializa a nossa GDT completa
void init_gdt(void)
{
    // Configurar o ponteiro (6 entradas * 8 bytes cada - 1)
    gdt_ptr.limit = (sizeof(gdt_entry_t) * 6) - 1;
    gdt_ptr.base = (unsigned int)&gdt;

    // Entrada 0: Segmento Nulo (obrigatório pela Intel)
    gdt_set_gate(0, 0, 0, 0, 0);

    // Entrada 1: Segmento de Código Kernel (Ring 0)
    // Acesso: 0x9A (Presente, Privilégio 0, Executável, Leitura)
    gdt_set_gate(1, 0, 0xFFFFFFFF, 0x9A, 0xCF);

    // Entrada 2: Segmento de Dados Kernel (Ring 0)
    // Acesso: 0x92 (Presente, Privilégio 0, Dados, Leitura/Escrita)
    gdt_set_gate(2, 0, 0xFFFFFFFF, 0x92, 0xCF);

    // Entrada 3: Segmento de Código Usuário (Ring 3)
    // Acesso: 0xFA (Presente, Privilégio 3, Executável, Leitura)
    gdt_set_gate(3, 0, 0xFFFFFFFF, 0xFA, 0xCF);

    // Entrada 4: Segmento de Dados Usuário (Ring 3)
    // Acesso: 0xF2 (Presente, Privilégio 3, Dados, Leitura/Escrita)
    gdt_set_gate(4, 0, 0xFFFFFFFF, 0xF2, 0xCF);

    // Entrada 5: Task State Segment (TSS)
    // Necessária para transição Ring 3 -> Ring 0 nas interrupções
    write_tss(5, 0x10, 0);

    // Carregar a GDT no processador via Assembly
    gdt_flush((unsigned int)&gdt_ptr);

    // Carrega o registrador de tarefa (TR) com o seletor da TSS (índice 5 * 8 = 0x28)
    __asm__ volatile("ltr %%ax" : : "a"(0x28));
}