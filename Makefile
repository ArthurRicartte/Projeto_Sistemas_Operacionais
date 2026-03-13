# Ferramentas
CC = gcc
AS = nasm
LD = ld
GRUB_MKRESCUE = grub-mkrescue

# Flags de compilação e linkagem
CFLAGS = -m32 -nostdlib -fno-builtin -fno-stack-protector \
         -nostartfiles -nodefaultlibs -Wall -Wextra -Werror -c
ASFLAGS_ELF = -f elf32
ASFLAGS_BIN = -f bin
LDFLAGS = -T config/linker.ld -m elf_i386

# Arquivos objeto do kernel
LOADER_OBJ = loader.o
KERNEL_OBJS = kmain.o fb.o serial.o io.o gdt_c.o gdt_s.o idt.o pic.o interrupts.o
OBJECTS = $(LOADER_OBJ) $(KERNEL_OBJS)

# Programa do usuário (módulo)
PROGRAM_SRC = src/program.s
PROGRAM_BIN = program.bin

# Alvo padrão
all: os.iso

# Compila o loader (assembly)
loader.o: src/boot/loader.s
	$(AS) $(ASFLAGS_ELF) src/boot/loader.s -o loader.o

# Compila os arquivos C
kmain.o: src/kernel/kmain.c src/kernel/gdt.h src/kernel/multiboot.h
	$(CC) $(CFLAGS) src/kernel/kmain.c -o kmain.o

fb.o: src/kernel/fb.c src/kernel/fb.h src/kernel/io.h
	$(CC) $(CFLAGS) src/kernel/fb.c -o fb.o

serial.o: src/kernel/serial.c src/kernel/serial.h src/kernel/io.h
	$(CC) $(CFLAGS) src/kernel/serial.c -o serial.o

io.o: src/io.s
	$(AS) $(ASFLAGS_ELF) src/io.s -o io.o

gdt_s.o: src/gdt.s
	$(AS) $(ASFLAGS_ELF) src/gdt.s -o gdt_s.o

gdt_c.o: src/kernel/gdt.c src/kernel/gdt.h
	$(CC) $(CFLAGS) src/kernel/gdt.c -o gdt_c.o

interrupts.o: src/interrupts.s
	$(AS) $(ASFLAGS_ELF) src/interrupts.s -o interrupts.o

pic.o: src/kernel/pic.c src/kernel/pic.h src/kernel/io.h
	$(CC) $(CFLAGS) src/kernel/pic.c -o pic.o

idt.o: src/kernel/idt.c src/kernel/idt.h src/kernel/pic.h
	$(CC) $(CFLAGS) src/kernel/idt.c -o idt.o

# Linkagem do kernel ELF
kernel.elf: $(OBJECTS)
	$(LD) $(LDFLAGS) $(OBJECTS) -o kernel.elf -Map=kernel.map

# Compila o programa do usuário para binário puro
$(PROGRAM_BIN): $(PROGRAM_SRC)
	$(AS) $(ASFLAGS_BIN) $(PROGRAM_SRC) -o $(PROGRAM_BIN)

# Prepara a estrutura de diretórios para a ISO
iso/:
	mkdir -p iso/boot/grub
	mkdir -p iso/modules

# Copia o kernel e o módulo para a ISO
iso/boot/kernel.elf: kernel.elf | iso/
	cp kernel.elf iso/boot/

iso/modules/$(PROGRAM_BIN): $(PROGRAM_BIN) | iso/
	cp $(PROGRAM_BIN) iso/modules/

# Copia o arquivo de configuração do GRUB
iso/boot/grub/grub.cfg: src/boot/grub.cfg | iso/
	cp src/boot/grub.cfg iso/boot/grub/

# Gera a ISO usando grub-mkrescue
os.iso: iso/boot/kernel.elf iso/modules/$(PROGRAM_BIN) iso/boot/grub/grub.cfg
	$(GRUB_MKRESCUE) -o os.iso iso/

# Executa no QEMU
run: os.iso
	qemu-system-i386 -cdrom os.iso -serial file:com1.out -d in_asm -D qemu.log

# Depuração com GDB
debug: os.iso
	qemu-system-i386 -s -S -cdrom os.iso -serial file:com1.out &
	gdb -ex "target remote localhost:1234" -ex "symbol-file kernel.elf"

# Limpeza
clean:
	rm -rf *.o *.bin *.elf os.iso iso/ com1.out

