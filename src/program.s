bits 32

section .text
    global _start

_start:
    mov eax, 0xDEADBEEF
    ret   ; retorna para o kernel
.loop:
    jmp .loop
