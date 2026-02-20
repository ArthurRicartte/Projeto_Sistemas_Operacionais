// Nucleo do Sistema Operacional – Unidade 4
// Desenvolvido por: Arthur Ricartte - ultima atualizacao (20/02/2026)
#include "fb.h"
#include "serial.h"
#include "gdt.h" // Adicionado para o Capítulo 5

// Função de delay simples (busy wait)
// Quanto maior o valor, maior a pausa.
static void sleep(unsigned int timer){
    for (volatile unsigned int i = 0; i < timer; i++){
        // O loop vazio, mas "volatile" impede que o compilador otimize
    }
}

void kmain(void){
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

    fb_write("Driver de framebuffer ativo.\n", 30);
    sleep(10000000);

    fb_write("Testando rolagem...\n", 20);
    sleep(10000000);

    // Força rolagem (escreve 30 linhas)
    for (int i = 0; i < 30; i++){
        fb_write("Linha de teste para rolagem.\n", 29);
        sleep(5000000); // pausa menor entre linhas para ver a rolagem
    }

    // Escreve na porta serial (será capturada no arquivo com1.out)
    serial_write(SERIAL_COM1, "Hello from serial port!\n", 24);
    serial_write(SERIAL_COM1, "Isso vai para o arquivo com1.out\n", 34);

    while (1){
        // Loop infinito
    }
}