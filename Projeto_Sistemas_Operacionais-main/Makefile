# --- ferramentas ---
CC = gcc
AS = nasm
LD = ld

# --- flags ---
CFLAGS = -m32 -nostdlib -nostdinc -fno-builtin -fno-stack-protector \
         -nostartfiles -nodefaultlibs -Wall -Wextra -Werror -c
ASFLAGS = -f elf
LDFLAGS = -T linker.ld -m elf_i386

# --- arquivos ---
OBJECTS = loader.o kmain.o

# --- ALVO PRINCIPAL ---
all: os.iso

# compila o Assembly (Loader)
loader.o: src/boot/loader.s
	$(AS) $(ASFLAGS) src/boot/loader.s -o loader.o

# compila o C (Kernel)
kmain.o: src/kernel/kmain.c
	$(CC) $(CFLAGS) src/kernel/kmain.c -o kmain.o

# linkagem (Gera o executável ELF)
kernel.elf: $(OBJECTS)
	$(LD) $(LDFLAGS) $(OBJECTS) -o kernel.elf

# criação da ISO
os.iso: kernel.elf
	@echo "--- criando estrutura da ISO ---"
	mkdir -p iso/boot/grub
	cp kernel.elf iso/boot/kernel.elf
	
	@echo "--- criando menu do GRUB ---"
	echo 'default=0' > iso/boot/grub/menu.lst
	echo 'timeout=0' >> iso/boot/grub/menu.lst
	echo 'title OS' >> iso/boot/grub/menu.lst
	echo 'kernel /boot/kernel.elf' >> iso/boot/grub/menu.lst
	
	@echo "--- verificando stage2_eltorito ---"
	@if [ ! -f iso/boot/grub/stage2_eltorito ]; then \
		echo "baixando stage2_eltorito..."; \
		wget -q https://github.com/littleosbook/littleosbook/raw/master/files/stage2_eltorito -O iso/boot/grub/stage2_eltorito; \
	fi
	
	@echo "--- gerando a imagem ISO ---"
	genisoimage -R                              \
                -b boot/grub/stage2_eltorito    \
                -no-emul-boot                   \
                -boot-load-size 4               \
                -A os                           \
                -input-charset utf8             \
                -quiet                          \
                -boot-info-table                \
                -o os.iso                       \
                iso

# --- INTERFACE DE EXECUÇÃO ---
run: all
	@echo "--- iniciando Bochs com CD-ROM ---"
	sudo bochs -q 'romimage: file=/usr/share/seabios/bios.bin' 'vgaromimage: file=/usr/share/seabios/vgabios.bin' 'boot: cdrom' 'ata0-master: type=cdrom, path="os.iso", status=inserted'

# --- LIMPEZA ---
clean:
	rm -rf *.o *.elf *.iso iso