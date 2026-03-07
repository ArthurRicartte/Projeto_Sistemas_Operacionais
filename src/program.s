; Programa simples para testar carregamento de módulos
; Desenvolvido por: Arthur Ricartte e Joao Veloso
; Data: 06/03/2026

bits 32

section .text
    global _start

_start:
    ; Escreve um valor fácil de identificar no registrador eax
    mov eax, 0xDEADBEEF
    
    ; Loop infinito para não retornar ao kernel
.loop:
    jmp .loop
