bits 32

section .text
    global _start

_start:
    ; Escreve um valor fácil de identificar no registrador eax
    mov eax, 0xDEADBEEF
    
    ; Loop infinito para não retornar ao kernel
.loop:
    jmp .loop
