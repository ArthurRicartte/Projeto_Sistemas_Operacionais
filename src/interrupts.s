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
    pusha                       ; salva todos os registradores (8 * 4 = 32 bytes)

    ; Chama interrupt_handler(cpu_state*, stack_state*, interrupt)
    push dword [esp+32]         ; número da interrupção (terceiro argumento)
    lea eax, [esp+36]           ; endereço do error_code (stack_state)
    push eax                    ; ponteiro para stack_state (segundo argumento)
    push esp                    ; ponteiro para cpu_state (primeiro argumento) - aponta para edi
    call interrupt_handler
    add esp, 12                 ; remove os 3 argumentos

    popa                        ; restaura registradores
    add esp, 8                  ; remove número da interrupção e código de erro
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