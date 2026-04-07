#include "idt.h"
#include "process.h"
#include "pic.h"
#include "io.h"
#include "fb.h"
#include "serial.h"
#include "pit.h"

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
        timer_handler();
    }
    else if (cpu->int_no >= 32 && cpu->int_no <= 47)
    {
        if (cpu->int_no == 33)
        {
            unsigned char scancode = inb(0x60);
            if (scancode == 0x0E) // backspace pressionado
            {
                fb_delete_char();
                serial_write(SERIAL_COM1, "\b", 1);
            }
            else if (!(scancode & 0x80)) // apenas tecla pressionada
            {
                if (scancode == 0x48) // Seta para Cima
                {
                    fb_scroll_up();
                }
                else if (scancode == 0x50) // Seta para Baixo
                {
                    fb_scroll_down();
                }
                else
                {
                    char ascii = scan_code_to_ascii(scancode);
                    if (ascii)
                    {
                        char str[2] = {ascii, '\0'};
                        fb_write(str, 1);
                        serial_write(SERIAL_COM1, str, 1);
                    }
                }
            }
        }
        pic_acknowledge(cpu->int_no);
    }

    // 3. Retorna nova pilha para o Assembly fazer o mov esp, eax
    if (current_process != (void *)0)
    {
        return current_process->esp;
    }
    return (uint32_t)cpu;
}