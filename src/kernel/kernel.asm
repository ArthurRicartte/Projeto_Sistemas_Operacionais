bits 16
org 0x1000  ; Onde o bootloader carrega

start:
    ; Limpar tela
    mov ax, 0x0003
    int 0x10
    
    ; Posicionar cursor
    mov ah, 0x02
    mov bh, 0
    mov dh, 5  ; Linha 5
    mov dl, 20 ; Coluna 20
    int 0x10
    
    ; Mensagem
    mov si, msg
    call print
    
    ; Parar
    jmp $

print:
    lodsb
    test al, al
    jz .done
    mov ah, 0x0E
    int 0x10
    jmp print
.done:
    ret

msg db "KERNEL EM MODO REAL FUNCIONANDO!", 0

; Preencher para 1024 bytes (2 setores)
times 1024-($-$$) db 0
