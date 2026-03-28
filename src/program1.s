bits 32

section .text
    global _start

_start:
    ; Identificador único para o Processo 1 no registrador EAX
    mov eax, 0x11111111 
.loop:
    inc eax
    
    ; Delay longo para simular processamento e permitir escalonamento
    mov ecx, 0x1000000
.delay:
    dec ecx
    jnz .delay
    
    ; Loop infinito (Ring 3 isolado)
    jmp .loop