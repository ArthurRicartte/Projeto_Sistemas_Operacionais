global loader                   ; o símbolo de entrada para o ELF
extern kmain                    ; a função definida no C

MAGIC_NUMBER equ 0x1BADB002     ; constante magica do multiboot
FLAGS        equ 0x0            ; flags
CHECKSUM     equ -MAGIC_NUMBER  ; soma de verificação
KERNEL_STACK_SIZE equ 4096      ; tamanho da pilha (4KB)

section .text
align 4
    dd MAGIC_NUMBER
    dd FLAGS
    dd CHECKSUM

loader:
    mov esp, kernel_stack + KERNEL_STACK_SIZE   ; aponta ESP para o topo da pilha
    call kmain                                  ; chama a função C
.loop:
    jmp .loop                                   ; loop infinito se kmain retornar

section .bss
align 4
kernel_stack:
    resb KERNEL_STACK_SIZE                      ; reserva memória para a pilha