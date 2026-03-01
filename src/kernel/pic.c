#include "io.h"
#include "pic.h"

#define PIC1_COMMAND 0x20
#define PIC1_DATA 0x21
#define PIC2_COMMAND 0xA0
#define PIC2_DATA 0xA1

#define PIC_EOI 0x20

#define ICW1_ICW4 0x01
#define ICW1_INIT 0x10
#define ICW4_8086 0x01

void init_pic(void)
{
    // Inicializa ambos os PICs
    outb(PIC1_COMMAND, ICW1_INIT | ICW1_ICW4);
    outb(PIC2_COMMAND, ICW1_INIT | ICW1_ICW4);

    // Mapeia IRQs para interrupções 0x20-0x2F
    outb(PIC1_DATA, 0x20); // IRQ0-7 -> 0x20-0x27
    outb(PIC2_DATA, 0x28); // IRQ8-15 -> 0x28-0x2F

    // Configura cascateamento (PIC2 ligado à IRQ2 do PIC1)
    outb(PIC1_DATA, 4);
    outb(PIC2_DATA, 2);

    // Modo 8086
    outb(PIC1_DATA, ICW4_8086);
    outb(PIC2_DATA, ICW4_8086);

    // Habilita apenas IRQ0 (timer) e IRQ1 (teclado) no master, desabilita todas no slave
    outb(PIC1_DATA, 0xFC); // 11111100 -> IRQ0 e IRQ1 desmascarados
    outb(PIC2_DATA, 0xFF); // Todos os slaves mascarados
}

void pic_acknowledge(unsigned int interrupt)
{
    if (interrupt < 0x20 || interrupt > 0x2F)
        return;

    if (interrupt < 0x28)
    {
        outb(PIC1_COMMAND, PIC_EOI);
    }
    else
    {
        outb(PIC2_COMMAND, PIC_EOI);
        outb(PIC1_COMMAND, PIC_EOI); // reconhece também no master por causa do cascade
    }
}