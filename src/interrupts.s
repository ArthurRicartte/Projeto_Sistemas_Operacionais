global load_idt
global common_interrupt_handler
global interrupt_handlers

extern interrupt_handler

; Macro para interrupções SEM código de erro
%macro no_error_code_interrupt_handler 1
global interrupt_handler_%1
interrupt_handler_%1:
    push dword 0                ; empurra código de erro dummy
    push dword %1               ; empurra número da interrupção
    jmp common_interrupt_handler
%endmacro

; Macro para interrupções COM código de erro
%macro error_code_interrupt_handler 1
global interrupt_handler_%1
interrupt_handler_%1:
    push dword %1               ; empurra número da interrupção
    jmp common_interrupt_handler
%endmacro

; Gera handlers para todas as 256 interrupções
%assign i 0
%rep 256
%if i == 8 || i == 10 || i == 11 || i == 12 || i == 13 || i == 14 || i == 17
    error_code_interrupt_handler i
%else
    no_error_code_interrupt_handler i
%endif
%assign i i+1
%endrep

; Handler comum
common_interrupt_handler:               
    pusha
    push ds
    push es
    push fs
    push gs

    push esp                    ; Envia ponteiro da pilha (cpu_state_t)
    call interrupt_handler
    mov esp, eax                ; <--- TROCA DE CONTEXTO AQUI

    pop gs
    pop fs
    pop es
    pop ds
    popa
    add esp, 8
    iret
; load_idt - Carrega a IDT
; C prototype: void load_idt(unsigned int idt_ptr_addr);
load_idt:
    mov eax, [esp+4]            ; pega o endereço do ponteiro
    lidt [eax]                  ; carrega a IDT
    ret

; Tabela com os endereços de todos os stubs (para uso em C)
interrupt_handlers:
%assign i 0
%rep 256
    dd interrupt_handler_ %+ i
%assign i i+1
%endrep