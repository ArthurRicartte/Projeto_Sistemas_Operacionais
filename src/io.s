global outb
global inb

; outb - envia um byte para uma porta de E/S
; stack: [esp+8] = dados, [esp+4] = porta, [esp] = retorno
outb:
    mov al, [esp+8]
    mov dx, [esp+4]
    out dx, al
    ret

; inb - lê um byte de uma porta de E/S
; stack: [esp+4] = porta, [esp] = retorno
inb:
    mov dx, [esp+4]
    in  al, dx
    ret
