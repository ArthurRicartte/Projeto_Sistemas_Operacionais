# Desenvolvido por: Arthur Ricartte e Joao Veloso - Ultima atualizacao: 20/02/2026

# Ferramentas
CC = gcc
AS = nasm
LD = ld

# Flags de compilação e linkagem
CFLAGS = -m32 -nostdlib -nostdinc -fno-builtin -fno-stack-protector \
         -nostartfiles -nodefaultlibs -Wall -Wextra -Werror -c
ASFLAGS_BOOT = -f bin
ASFLAGS_LOADER = -f elf32
LDFLAGS = -T config/linker.ld -m elf_i386

# Arquivos objeto (Adicionados gdt_c.o e gdt_s.o para o Cap 5)
LOADER_OBJ = loader.o
# novos objetos: idt.o , pic.o , interrupts.o
KERNEL_OBJS =  kmain.o fb.o serial.o io.o gdt_c.o gdt_s.o idt.o pic.o interrupts.o 

OBJECTS = $(LOADER_OBJ) $(KERNEL_OBJS)

# Alvo padrão
all: disk.img

# Bootloader (binário)
boot.bin: src/boot/boot.asm
	$(AS) $(ASFLAGS_BOOT) src/boot/boot.asm -o boot.bin

# Loader (modo protegido)
loader.o: src/boot/loader.s
	$(AS) $(ASFLAGS_LOADER) src/boot/loader.s -o loader.o

# io.s (assembly de E/S)
io.o: src/io.s
	$(AS) $(ASFLAGS_LOADER) src/io.s -o io.o

# --- Regras para a GDT (Capítulo 5) ---
gdt_s.o: src/gdt.s
	$(AS) $(ASFLAGS_LOADER) src/gdt.s -o gdt_s.o

gdt_c.o: src/kernel/gdt.c src/kernel/gdt.h
	$(CC) $(CFLAGS) src/kernel/gdt.c -o gdt_c.o
# --------------------------------------

# Kernel C
kmain.o: src/kernel/kmain.c src/kernel/gdt.h
	$(CC) $(CFLAGS) src/kernel/kmain.c -o kmain.o

fb.o: src/kernel/fb.c src/kernel/fb.h src/kernel/io.h
	$(CC) $(CFLAGS) src/kernel/fb.c -o fb.o

serial.o: src/kernel/serial.c src/kernel/serial.h src/kernel/io.h
	$(CC) $(CFLAGS) src/kernel/serial.c -o serial.o

# --- NOVAS REGRAS PARA INTERRUPCOES ---
interrupts.o: src/interrupts.s
	$(AS) $(ASFLAGS_LOADER) src/interrupts.s -o interrupts.o

pic.o: src/kernel/pic.c src/kernel/pic.h src/kernel/io.h
	$(CC) $(CFLAGS) src/kernel/pic.c -o pic.o

idt.o: src/kernel/idt.c src/kernel/idt.h src/kernel/pic.h
	$(CC) $(CFLAGS) src/kernel/idt.c -o idt.o
	
# Linkagem
kernel.elf: $(OBJECTS)
	$(LD) $(LDFLAGS) $(OBJECTS) -o kernel.elf

# Converter ELF para binário
kernel.bin: kernel.elf
	objcopy -O binary kernel.elf kernel.bin

# Imagem de disco
disk.img: boot.bin kernel.bin
	dd if=/dev/zero of=disk.img bs=512 count=2880
	dd if=boot.bin of=disk.img conv=notrunc
	dd if=kernel.bin of=disk.img bs=512 seek=1 conv=notrunc

# Executar no QEMU com redirecionamento serial
run: disk.img
	qemu-system-i386 -fda disk.img -boot a -serial file:com1.out

# Depuração com GDB
debug: disk.img
	qemu-system-i386 -s -S -fda disk.img -serial file:com1.out &
	gdb -ex "target remote localhost:1234" -ex "symbol-file kernel.elf"

# Limpeza
clean:
	rm -rf *.o *.bin *.elf disk.img com1.out
