bits 32

section .text
    global _start

_start:
    ; Identificador único para o Processo 4
    mov eax, 0x44444444 
.loop:
    inc eax
    
    mov ecx, 0x4000000
.delay:
    dec ecx
    jnz .delay
    
    jmp .loop