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

# Arquivos objeto do kernel (restore_context.o REMOVIDO)
LOADER_OBJ = loader.o
KERNEL_OBJS = kmain.o fb.o serial.o io.o gdt_c.o gdt_s.o idt.o pic.o interrupts.o pmm.o kheap.o process.o scheduler.o pit.o
OBJECTS = $(LOADER_OBJ) $(KERNEL_OBJS)

# Alvo padrão
all: programs os.iso

# Compila o loader (assembly)
loader.o: src/boot/loader.s
	$(AS) $(ASFLAGS_ELF) src/boot/loader.s -o loader.o

# Compila os arquivos C
kmain.o: src/kernel/kmain.c src/kernel/gdt.h src/kernel/multiboot.h src/kernel/process.h
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

idt.o: src/kernel/idt.c src/kernel/idt.h src/kernel/pic.h src/kernel/pit.h
	$(CC) $(CFLAGS) src/kernel/idt.c -o idt.o

pmm.o: src/kernel/pmm.c src/kernel/pmm.h src/kernel/multiboot.h
	$(CC) $(CFLAGS) src/kernel/pmm.c -o pmm.o

kheap.o: src/kernel/kheap.c src/kernel/kheap.h src/kernel/pmm.h
	$(CC) $(CFLAGS) src/kernel/kheap.c -o kheap.o

process.o: src/kernel/process.c src/kernel/process.h src/kernel/string.h src/kernel/pmm.h src/kernel/kheap.h
	$(CC) $(CFLAGS) src/kernel/process.c -o process.o

scheduler.o: src/kernel/scheduler.c src/kernel/scheduler.h src/kernel/process.h src/kernel/fb.h src/kernel/string.h
	$(CC) $(CFLAGS) src/kernel/scheduler.c -o scheduler.o

pit.o: src/kernel/pit.c src/kernel/pit.h src/kernel/io.h src/kernel/process.h
	$(CC) $(CFLAGS) src/kernel/pit.c -o pit.o

# Linkagem do kernel ELF
kernel.elf: $(OBJECTS)
	$(LD) $(LDFLAGS) $(OBJECTS) -o kernel.elf -Map=kernel.map

# Programas de teste (assembly)
PROGRAMS = program1 program2 program3 program4

programs: $(foreach prog,$(PROGRAMS),$(prog).bin)

%.bin: src/%.s
	$(AS) $(ASFLAGS_BIN) $< -o $@

# Prepara a estrutura de diretórios para a ISO
iso/:
	mkdir -p iso/boot/grub
	mkdir -p iso/modules

# Copia o kernel e módulos para a ISO
iso/boot/kernel.elf: kernel.elf | iso/
	cp kernel.elf iso/boot/

# Regra genérica para copiar os binários dos programas
iso/modules/%.bin: %.bin | iso/
	cp $< $@

# Copia o arquivo de configuração do GRUB
iso/boot/grub/grub.cfg: src/boot/grub.cfg | iso/
	cp src/boot/grub.cfg iso/boot/grub/

# Gera a ISO (Dependências atualizadas)
os.iso: iso/boot/kernel.elf iso/boot/grub/grub.cfg $(foreach prog,$(PROGRAMS),iso/modules/$(prog).bin)
	$(GRUB_MKRESCUE) -o os.iso iso/

# Executa no QEMU
run: os.iso
	qemu-system-i386 -cdrom os.iso -serial file:com1.out

# Depuração com GDB
debug: os.iso
	qemu-system-i386 -s -S -cdrom os.iso -serial file:com1.out &
	gdb -ex "target remote localhost:1234" -ex "symbol-file kernel.elf"

# Limpeza
clean:
	rm -rf *.o *.bin *.elf os.iso iso/ com1.out kernel.map