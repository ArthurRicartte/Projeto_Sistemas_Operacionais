;Desenvolvido por: Arthur Ricartte e Joao Veloso (Ultima Atualizacao: 10-02-2026)
bits 32
global loader
extern kmain

section .text
loader:
    ;A pilha já foi configurada pelo bootloader (esp = 0x9000)
    
    ;Chamar o kernel principal em C
    call kmain
    
    ;Se kmain retornar (não deveria), loop infinito
.halt:
    hlt
    jmp .halt

section .bss
;Pilha opcional, já que o bootloader configurou