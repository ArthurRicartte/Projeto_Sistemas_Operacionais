// Nucleo do Sistema Operacional

#include "fb.h"
#include "serial.h"
#include "gdt.h"       // Adicionado para o Capítulo 5
#include "idt.h"       //Adicionado para o Capítulo 6
#include "pic.h"       //Adicionado para o Capítulo 6
#include "multiboot.h" // NOVO: Para o Capítulo 7
#include "pmm.h"       // Capítulo 10 - Page Frame Allocator
#include "kheap.h"     // Capítulo 10 - Kernel Heap

/* Labels exportados pelo linker script */
extern uint32_t kernel_physical_start;
extern uint32_t kernel_physical_end;
extern uint32_t kernel_virtual_start;
extern uint32_t kernel_virtual_end;

// Função de delay simples (busy wait)
// Quanto maior o valor, maior a pausa.
static void sleep(unsigned int timer)
{
    for (volatile unsigned int i = 0; i < timer; i++)
    {
        // O loop vazio, mas "volatile" impede que o compilador otimize
    }
}

// NOVO: Função auxiliar para converter números para hexadecimal
static void write_hex(unsigned int num)
{
    char hex_chars[] = "0123456789ABCDEF";
    char buffer[11]; // "0xFFFFFFFF" + '\0'

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

// FUNÇÃO PRINCIPAL MODIFICADA - AGORA RECEBE EBX
void kmain(unsigned int ebx) // <--- MUDANÇA: parâmetro adicionado
{
    // Inicializa a GDT antes de qualquer outra coisa
    init_gdt();

    // Inicializa o framebuffer
    fb_clear();

    // Inicializa a porta serial COM1
    serial_configure(SERIAL_COM1);

    // Escreve mensagens no framebuffer
    fb_write("GDT Inicializada!\n", 18);
    fb_write("Kernel em Modo Protegido Funcionando!\n", 37);
    sleep(10000000);

    fb_write("\nDriver de framebuffer ativo.\n", 30);
    sleep(10000000);

    // Escreve na porta serial (sera capturada no arquivo com1.out)
    serial_write(SERIAL_COM1, "Hello from serial port!\n", 24);
    serial_write(SERIAL_COM1, "Isso vai para o arquivo com1.out\n", 34);

    // Inicializa a IDT e o PIC
    fb_write("Inicializando IDT...\n", 21);
    init_idt(); // inicializa tabela de interrupcoes

    fb_write("Configurando PIC...\n", 20);
    init_pic(); //  configura o controlador de interrupcoes

    fb_write("Habilitando interrupcoes...\n", 28);
    __asm__ volatile("sti"); // habilita interrupcoes na CPU

    fb_write("Interrupcoes habilitadas. Digite algo no teclado...\n", 52);

    // NOVO: Codigo do Capítulo 7 - Carregar e executar módulo
    fb_write("\n[Unidade 7] Procurando modulos do GRUB...\n", 41);

    // Converter ebx para a estrutura multiboot
    multiboot_info_t *mbinfo = (multiboot_info_t *)ebx;

    // Verificar se módulos foram carregados (bit 3 das flags)
    if (mbinfo->flags & MULTIBOOT_INFO_MODS)
    {
        fb_write("\nModulos detectados! Flags ok.\n", 30);

        if (mbinfo->mods_count > 0)
        {
            fb_write("\nNumero de modulos: ", 19);

            // Mostrar contagem (convertendo para caractere)
            char count_str[2];
            count_str[0] = '0' + mbinfo->mods_count;
            count_str[1] = '\n';
            fb_write(count_str, 2);

            // Pegar endereço do primeiro módulo
            multiboot_module_t *mod = (multiboot_module_t *)mbinfo->mods_addr;
            unsigned int module_address = mod->mod_start;

            fb_write("Endereco do modulo: ", 20);
            write_hex(module_address);
            fb_write("\n", 1);

            fb_write("Executando modulo...\n", 21);

            // Executar o módulo
            void (*program)(void) = (void (*)(void))module_address;
            program();

            // Se chegar aqui, o módulo retornou (não deveria)
            fb_write("ERRO: Modulo retornou!\n", 22);
        }
        else
        {
            fb_write("ERRO: mods_count = 0\n", 21);
        }
    }
    else
    {
        fb_write("\nERRO: Nenhum modulo carregado!\n", 30);
        fb_write("Verifique menu.lst e pasta modules/\n", 36);
    }

    // ---------- DEMONSTRAÇÃO DA UNIDADE 9 (HIGHER HALF) ----------
    fb_write("\n\n[Unidade 9] Verificando higher half...\n", 37);

    // Mostrar o endereço virtual da função kmain (deve ser > 0xC0000000)
    fb_write("\nEndereco de kmain: ", 19);
    write_hex((unsigned int)kmain);
    fb_write("\n", 1);

    // Ler e mostrar o valor do registrador CR3 (endereço físico do page directory)
    uint32_t cr3_value;
    __asm__ volatile("mov %%cr3, %0" : "=r"(cr3_value));
    fb_write("CR3 (page directory fisico): ", 29);
    write_hex(cr3_value);
    fb_write("\n", 1);
    // -------------------------------------------------------------

    // ---------- UNIDADE 10: PAGE FRAME ALLOCATION ----------
    fb_write("\n[Unidade 10] Inicializando PMM...\n", 35);
    serial_write(SERIAL_COM1, "[Unidade 10] Inicializando PMM...\n", 34);

    /* O ponteiro mbinfo contem enderecos FISICOS do GRUB.
     * Precisamos converter para virtual somando 0xC0000000 */
    multiboot_info_t *mbinfo_virt = (multiboot_info_t *)((uint32_t)mbinfo);
    pmm_init(mbinfo_virt,
             (uint32_t)&kernel_physical_start,
             (uint32_t)&kernel_physical_end);

    fb_write("PMM inicializado com sucesso!\n", 30);

    // Teste: alocar e liberar um page frame
    void* test_page = pmm_alloc_page();
    if (test_page) {
        fb_write("Pagina alocada (fisico): ", 25);
        write_hex((uint32_t)test_page);
        fb_write("\n", 1);
        serial_write(SERIAL_COM1, "Pagina alocada com sucesso!\n", 28);

        kfree(test_page);
        fb_write("Pagina liberada com sucesso!\n", 29);
    } else {
        fb_write("ERRO: Falha ao alocar pagina!\n", 30);
        serial_write(SERIAL_COM1, "ERRO: Falha ao alocar pagina!\n", 30);
    }
    // ---------------------------------------------------------

    // Loop infinito original (preservado)
    while (1)
    {
        __asm__ volatile("hlt"); // Halt instruction to save power
    }
}
