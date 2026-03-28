#include "process.h"
#include "idt.h"
#include "fb.h"
#include "serial.h"
#include "gdt.h"
#include "pic.h"
#include "multiboot.h"
#include "pmm.h"
#include "kheap.h"
#include "string.h"
#include "pit.h"

#define KERNEL_VIRTUAL_BASE 0xC0000000

/* Labels exportados pelo linker script */
extern uint32_t kernel_physical_start;
extern uint32_t kernel_physical_end;
extern tss_entry_t tss;

// Função de delay simples (busy wait)
static void sleep(unsigned int timer)
{
    for (volatile unsigned int i = 0; i < timer; i++)
        ;
}

// Função auxiliar para converter números para hexadecimal
static void write_hex(unsigned int num)
{
    char hex_chars[] = "0123456789ABCDEF";
    char buffer[11];
    buffer[0] = '0';
    buffer[1] = 'x';
    buffer[10] = '\0';
    for (int i = 0; i < 8; i++)
    {
        unsigned char nibble = (num >> (28 - i * 4)) & 0xF;
        buffer[2 + i] = hex_chars[nibble];
    }
    fb_write(buffer, 10);
}

void kmain(unsigned int ebx)
{
    // 1. Inicialização Base de Hardware
    init_gdt();
    fb_clear();
    serial_configure(SERIAL_COM1);

    fb_write("GDT Inicializada!\n", 18);
    fb_write("Kernel em Modo Protegido (Ring 0) ativo.\n", 41);
    serial_write(SERIAL_COM1, "Kernel Booting...\n", 18);

    // 2. Inicialização de Interrupções
    init_idt();
    init_pic();
    fb_write("IDT e PIC configurados.\n", 24);

    // 3. Memória e Higher Half
    // O ponteiro ebx vem como endereço físico, precisamos do virtual
    multiboot_info_t *mbinfo_virt = (multiboot_info_t *)(ebx + KERNEL_VIRTUAL_BASE);

    fb_write("\n[Unidade 9/10] Verificando Memoria...\n", 38);
    fb_write("\nEndereco de kmain (Virtual): ", 29);
    write_hex((unsigned int)kmain);
    fb_write("\n", 1);

    pmm_init(mbinfo_virt, (uint32_t)&kernel_physical_start, (uint32_t)&kernel_physical_end);
    fb_write("PMM e Heap inicializados.\n", 26);

    // 4. Preparação de Processos via Multiboot (Unidade 14)
    fb_write("\n[Unidade 14] Carregando modulos como processos...\n", 51);

    if (mbinfo_virt->flags & MULTIBOOT_INFO_MODS)
    {
        if (mbinfo_virt->mods_count > 0)
        {
            multiboot_module_t *mods = (multiboot_module_t *)(mbinfo_virt->mods_addr + KERNEL_VIRTUAL_BASE);

            for (uint32_t i = 0; i < mbinfo_virt->mods_count; i++)
            {
                uint32_t prog_start = mods[i].mod_start + KERNEL_VIRTUAL_BASE;

                fb_write("Modulo ", 7);
                write_hex(i);
                fb_write(" em: ", 5);
                write_hex(prog_start);
                fb_write("\n", 1);

                create_process(prog_start, "external_prog");
            }
        }
        else
        {
            fb_write("ERRO: mods_count = 0\n", 21);
        }
    }
    else
    {
        fb_write("ERRO: Nenhum modulo Multiboot detectado!\n", 41);
    }

    // 5. Configuração do Escalonador Preemptivo
    // Frequência de 20Hz (Troca a cada 50ms aprox.)
    init_preemptive_scheduler(20);
    fb_write("Escalonador Round Robin (PIT) ativo.\n", 37);

    // 6. Salto para Ring 3 (Modo Usuário)
    current_process = ready_queue;
    if (current_process != (void *)0)
    {
        current_process->state = PROCESS_RUNNING;

        // Atualiza a TSS antes do primeiro salto
        tss.esp0 = (uint32_t)current_process->esp & 0xFFFFF000;
        tss.esp0 += 4096;

        fb_write("\n*** Saltando para Ring 3 (User Mode) ***\n", 41);
        sleep(5000000); // Breve pausa para o usuário ler as mensagens

        // 7. DISPARO DO CONTEXTO INICIAL
        __asm__ volatile(
            "mov %0, %%esp\n"
            "pop %%gs\n"
            "pop %%fs\n"
            "pop %%es\n"
            "pop %%ds\n"
            "popa\n"
            "add $8, %%esp\n"
            "iret\n"
            : : "r"(current_process->esp));
    }

    // Caso não existam processos, o Kernel entra em repouso
    fb_write("Aviso: Fila de prontos vazia. Kernel em HLT.\n", 45);
    while (1)
    {
        __asm__ volatile("hlt");
    }
}