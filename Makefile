# Makefile – Projeto Sistemas Operacionais (Unidade 4)
# Desenvolvido por: Arthur Ricartte e Joao Veloso

# Ferramentas
CC = gcc
AS = nasm
LD = ld

# Flags
CFLAGS = -m32 -nostdlib -nostdinc -fno-builtin -fno-stack-protector \
         -nostartfiles -nodefaultlibs -Wall -Wextra -Werror -c
ASFLAGS_BOOT = -f bin
ASFLAGS_LOADER = -f elf32
LDFLAGS = -T config/linker.ld -m elf_i386

# Arquivos objeto
LOADER_OBJ = loader.o
KERNEL_OBJS = kmain.o fb.o serial.o io.o
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

# Kernel C
kmain.o: src/kernel/kmain.c
	$(CC) $(CFLAGS) src/kernel/kmain.c -o kmain.o

fb.o: src/kernel/fb.c src/kernel/fb.h src/kernel/io.h
	$(CC) $(CFLAGS) src/kernel/fb.c -o fb.o

serial.o: src/kernel/serial.c src/kernel/serial.h src/kernel/io.h
	$(CC) $(CFLAGS) src/kernel/serial.c -o serial.o

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

# Depuração
debug: disk.img
	qemu-system-i386 -s -S -fda disk.img -serial file:com1.out &
	gdb -ex "target remote localhost:1234" -ex "symbol-file kernel.elf"

# Limpeza
clean:
	rm -rf *.o *.bin *.elf disk.img com1.out
