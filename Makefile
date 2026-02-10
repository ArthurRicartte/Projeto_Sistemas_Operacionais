#MakeFile serve para definir a compilacao, linkagem e empacotar o SO
#Desenvolvido por: Arthur Ricartte e Joao Veloso (Ultima atualizacao: 10-02-2026)
# --- ferramentas ---
CC = gcc
AS = nasm
LD = ld

# --- flags ---
CFLAGS = -m32 -nostdlib -nostdinc -fno-builtin -fno-stack-protector \
         -nostartfiles -nodefaultlibs -Wall -Wextra -Werror -c
ASFLAGS_BOOT = -f bin
ASFLAGS_LOADER = -f elf32
LDFLAGS = -T linker.ld -m elf_i386

# --- arquivos ---
OBJECTS = loader.o kmain.o

# --- ALVO PRINCIPAL ---
all: disk.img

# Bootloader (binário plano, setor de boot):
boot.bin: src/boot/boot.asm
	$(AS) $(ASFLAGS_BOOT) src/boot/boot.asm -o boot.bin

# Loader (modo protegido):
loader.o: src/boot/loader.s
	$(AS) $(ASFLAGS_LOADER) src/boot/loader.s -o loader.o

# Kernel em C:
kmain.o: src/kernel/kmain.c
	$(CC) $(CFLAGS) src/kernel/kmain.c -o kmain.o

# Linkar tudo em ELF:
kernel.elf: $(OBJECTS)
	$(LD) $(LDFLAGS) $(OBJECTS) -o kernel.elf

# Converter ELF para binário bruto:
kernel.bin: kernel.elf
	objcopy -O binary kernel.elf kernel.bin

# Criar imagem de disco:
disk.img: boot.bin kernel.bin
	dd if=/dev/zero of=disk.img bs=512 count=2880  # Disquete 1.44MB
	dd if=boot.bin of=disk.img conv=notrunc        # Setor de boot
	dd if=kernel.bin of=disk.img bs=512 seek=1 conv=notrunc  # Setor 2+

# Executar com QEMU:
run: disk.img
	qemu-system-i386 -fda disk.img -boot a

# Limpar:
clean:
	rm -rf *.o *.bin *.elf disk.img

# Depurar com GDB:
debug: disk.img
	qemu-system-i386 -s -S -fda disk.img &
	gdb -ex "target remote localhost:1234" -ex "symbol-file kernel.elf"