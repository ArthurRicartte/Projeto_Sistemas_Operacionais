# ------------------------------------------------------------
# Makefile simples para SO
ASM = nasm
ASM_FLAGS = -f bin

all:
	$(ASM) $(ASM_FLAGS) src/boot/boot.asm -o boot.bin

run:
	bochs -q

clean:
	rm -f boot.bin
# ------------------------------------------------------------
