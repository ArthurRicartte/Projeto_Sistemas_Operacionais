#include "idt.h"
#include "io.h"
#include "fb.h"
#include "serial.h"
#include "pic.h"

extern void load_idt(unsigned int idt_ptr_addr);

// A IDT propriamente dita
static idt_entry_t idt[256];
static idt_ptr_t idt_ptr;

// Tabela de endereços dos stubs (definida em interrupts.s)
extern unsigned int interrupt_handlers[256];

// Função para preencher uma entrada da IDT
void idt_set_gate(int num, unsigned int base, unsigned short selector, unsigned char flags)
{
    idt[num].base_low = base & 0xFFFF;
    idt[num].base_high = (base >> 16) & 0xFFFF;
    idt[num].selector = selector;
    idt[num].always0 = 0;
    idt[num].flags = flags;
}

// Inicializa a IDT
void init_idt(void)
{
    // Configura o ponteiro da IDT
    idt_ptr.limit = sizeof(idt_entry_t) * 256 - 1;
    idt_ptr.base = (unsigned int)&idt;

    // Preenche todas as 256 entradas com os endereços dos stubs
    for (int i = 0; i < 256; i++)
    {
        idt_set_gate(i, interrupt_handlers[i], 0x08, 0x8E); // 0x8E = interrupt gate, ring 0
    }

    // Carrega a IDT (função assembly)
    load_idt((unsigned int)&idt_ptr);
}

// Mapeamento simples de scan code para ASCII (apenas teclas pressionadas, US)
static char scan_code_to_ascii(unsigned char scancode)
{
    if (scancode & 0x80)
        return 0; // liberação de tecla ignorada
    static char map[] = {
        0, 0, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', 0, 0,
        'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n', 0,
        'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`', 0, '\\',
        'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0, '*', 0, ' '};
    if (scancode < sizeof(map))
        return map[scancode];
    return 0;
}

// Handler C chamado para todas as interrupções
void interrupt_handler(struct cpu_state *cpu, struct stack_state *stack, unsigned int interrupt)
{
    (void)cpu;
    (void)stack;

    // Exceções da CPU (0-31)
    if (interrupt < 32)
    {
        fb_write("Exceção! ", 9);
        // Exibe o número da exceção e trava
        char buf[3];
        buf[0] = '0' + (interrupt / 10);
        buf[1] = '0' + (interrupt % 10);
        buf[2] = '\n';
        fb_write(buf, 3);
        while (1)
            ; // trava
    }
    // Interrupções de hardware do PIC (32-47)
    else if (interrupt >= 32 && interrupt <= 47)
    {
        if (interrupt == 33)
        { // IRQ1 (teclado)
            unsigned char scancode = inb(0x60);

            if (scancode == 0x0E) // backspace pressionado
            {
                fb_delete_char();
                serial_write(SERIAL_COM1, "\b", 1); // opcional: envia backspace para a serial
            }
            else if (!(scancode & 0x80)) // apenas tecla pressionada (não liberada)
            {
                char ascii = scan_code_to_ascii(scancode);
                if (ascii)
                {
                    char str[2] = {ascii, '\0'};
                    fb_write(str, 1);
                    serial_write(SERIAL_COM1, str, 1);
                }
            }

            pic_acknowledge(interrupt);
        }
        else if (interrupt == 32)
        { // IRQ0 (timer)
            // Por enquanto, apenas reconhece
            pic_acknowledge(interrupt);
        }
        else
        {
            // Demais IRQs: apenas reconhece
            pic_acknowledge(interrupt);
        }
    }
    // Outras interrupções (por exemplo, geradas por software) ignoradas
}