;Desenvolvido por: Arthur Ricartte e Joao Veloso (Ultima Atualizacao: 10-02-2026)
bits 32
global loader
extern kmain

; ---------- CABEÇALHO MULTIBOOT (OBRIGATÓRIO PARA GRUB) ----------
MAGIC_NUMBER  equ 0x1BADB002
ALIGN_MODULES equ 0x00000001          ; Flag para alinhar módulos
CHECKSUM      equ -(MAGIC_NUMBER + ALIGN_MODULES)

section .text
align 4
    dd MAGIC_NUMBER
    dd ALIGN_MODULES
    dd CHECKSUM
; ------------------------------------------------------------------

loader:
    ; O GRUB coloca um ponteiro importante em ebx. Vamos salvá-lo.
    push ebx                          ; Salva o ponteiro para passar ao kmain
    
    ;A pilha já foi configurada pelo bootloader (esp = 0x9000)
    
    ;Chamar o kernel principal em C
    call kmain
    
    ;Se kmain retornar (não deveria), loop infinito
.halt:
    hlt
    jmp .halt

section .bss
;Pilha opcional, já que o bootloader configurou
