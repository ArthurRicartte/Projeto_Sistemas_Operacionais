bits 32

section .text
    global _start

_start:
    ; Identificador único para o Processo 3
    mov eax, 0x33333333 
.loop:
    inc eax
    
    mov ecx, 0x3000000
.delay:
    dec ecx
    jnz .delay
    
    jmp .loop