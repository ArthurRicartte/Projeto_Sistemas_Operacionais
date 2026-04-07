bits 32
global loader
extern kmain

; ---------- CABEÇALHO MULTIBOOT ----------
MAGIC_NUMBER  equ 0x1BADB002
ALIGN_MODULES equ 0x00000001
CHECKSUM      equ -(MAGIC_NUMBER + ALIGN_MODULES)

; Definições para o Higher Half
VM_BASE        equ 0xC0000000
PDE_INDEX      equ (VM_BASE >> 22) ; Índice no Page Directory (768)

section .data
align 4096
; Um diretório de páginas simples que mapeia os primeiros 4MB da RAM
; tanto no endereço 0x00000000 quanto no 0xC0000000 (Identity mapping + Higher Half)
global page_directory
page_directory:
    ; 0x00000083: Presente + Escrita + 4MB Page
    dd 0x00000083
    times (PDE_INDEX - 1) dd 0
    dd 0x00000083
    times (1024 - PDE_INDEX - 1) dd 0

section .text
align 4
    dd MAGIC_NUMBER
    dd ALIGN_MODULES
    dd CHECKSUM

loader:
    ; Salva o ponteiro multiboot (estava em ebx)
    push ebx

    ; --- CONFIGURAR PAGINAÇÃO BÁSICA ---
    ; Carregar endereço físico do page directory
    mov eax, page_directory
    sub eax, VM_BASE          ; converte para endereço físico
    mov cr3, eax

    ; Ativar PSE (Page Size Extensions) para páginas de 4MB
    mov eax, cr4
    or eax, 0x00000010
    mov cr4, eax

    ; Ativar paginação (PG)
    mov eax, cr0
    or eax, 0x80000000
    mov cr0, eax

    ; --- SALTO PARA O HIGHER HALF ---
    lea eax, [higher_half]
    jmp eax

higher_half:
    ; Agora estamos rodando acima de 0xC0000000
    ; Recupera o ponteiro multiboot
    pop ebx
    push ebx                ; argumento para kmain
    call kmain

.halt:
    hlt
    jmp .halt