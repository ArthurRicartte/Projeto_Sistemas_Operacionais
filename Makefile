# ==========================================
# SISTEMA OPERACIONAL - CONFIGURAÇÕES
# ==========================================

# Diretórios
SRC_DIR    = src
BUILD_DIR  = build
BOOT_DIR   = $(SRC_DIR)/boot
KERNEL_DIR = $(SRC_DIR)/kernel

# Arquivos fonte
BOOT_SRC   = $(BOOT_DIR)/boot.asm
KERNEL_SRC = $(KERNEL_DIR)/kernel.asm
KERNEL_C   = $(KERNEL_DIR)/kernel.c

# Arquivos binários
BOOT_BIN   = $(BUILD_DIR)/boot.bin
KERNEL_BIN = $(BUILD_DIR)/kernel.bin
OS_IMG     = $(BUILD_DIR)/os.img

# Compiladores
ASM        = nasm
ASM_FLAGS  = -f bin

# Emulador
QEMU       = qemu-system-i386
QEMU_FLAGS = -drive format=raw,file=$(OS_IMG) -monitor stdio

# ==========================================
# REGRAS DE COMPILAÇÃO
# ==========================================

.PHONY: all clean run debug

# Alvo principal
all: $(OS_IMG)

# Compilar bootloader
$(BOOT_BIN): $(BOOT_SRC)
	@echo "🔧 Compilando bootloader..."
	@mkdir -p $(BUILD_DIR)
	$(ASM) $(ASM_FLAGS) $< -o $@
	@echo "✅ Bootloader: $(shell stat -c%s $@) bytes"

# Compilar kernel (assembly)
$(KERNEL_BIN): $(KERNEL_SRC)
	@echo "🔧 Compilando kernel..."
	$(ASM) $(ASM_FLAGS) $< -o $@
	@echo "✅ Kernel: $(shell stat -c%s $@) bytes"

# Criar imagem de disco
$(OS_IMG): $(BOOT_BIN) $(KERNEL_BIN)
	@echo "📀 Criando imagem de disco..."
	@dd if=/dev/zero of=$@ bs=512 count=2880 2>/dev/null
	@dd if=$(BOOT_BIN) of=$@ conv=notrunc 2>/dev/null
	@dd if=$(KERNEL_BIN) of=$@ bs=512 seek=1 conv=notrunc 2>/dev/null
	@echo "✅ Imagem criada: $@"
	@echo "🔍 Verificando setores..."
	@echo "   Setor 0: bootloader"
	@dd if=$@ bs=512 count=1 2>/dev/null | tail -c 2 | hexdump -C | grep -q "55 aa" && echo "   ✅ Assinatura boot OK" || echo "   ❌ Assinatura boot FALHOU"
	@echo "   Setor 1-2: kernel"
	@SIZE=$$(dd if=$@ bs=512 skip=1 count=2 2>/dev/null | wc -c); \
	if [ $$SIZE -ge 1024 ]; then \
		echo "   ✅ Kernel carregado ($$SIZE bytes)"; \
	else \
		echo "   ❌ Kernel incompleto ($$SIZE bytes)"; \
	fi

# Executar no QEMU
run: $(OS_IMG)
	@echo "🚀 Iniciando QEMU..."
	$(QEMU) $(QEMU_FLAGS)

# Debug com QEMU (conecta no GDB)
debug: $(OS_IMG)
	@echo "🐛 Iniciando QEMU em modo debug (aguardando GDB)..."
	@echo "   Execute em outro terminal: gdb -ex 'target remote localhost:1234'"
	$(QEMU) $(QEMU_FLAGS) -s -S

# Limpar tudo
clean:
	@echo "🧹 Limpando arquivos compilados..."
	rm -rf $(BUILD_DIR)/*
	@echo "✅ Limpeza concluída"

# Mostrar estrutura do projeto
tree:
	@echo "📁 Estrutura do projeto:"
	@find . -type f -name "*.asm" -o -name "*.c" -o -name "*.h" -o -name "Makefile" -o -name "*.ld" | sort | sed 's|^\./||'

# Verificar integridade
check:
	@echo "🔍 Verificando integridade do projeto..."
	@echo "1. Arquivos fonte essenciais:"
	@for file in $(BOOT_SRC) $(KERNEL_SRC) Makefile; do \
		if [ -f "$$file" ]; then \
			echo "   ✅ $$file"; \
		else \
			echo "   ❌ $$file (FALTANDO)"; \
		fi; \
	done
	@echo "2. Tamanhos esperados após compilação:"
	@echo "   Bootloader: 512 bytes"
	@echo "   Kernel: 1024 bytes (2 setores)"
	@echo "   Imagem: 1.44 MB (2880 setores)"

# Ajuda
help:
	@echo "📖 COMANDOS DISPONÍVEIS:"
	@echo "  make all     - Compila tudo e cria imagem"
	@echo "  make run     - Compila e executa no QEMU"
	@echo "  make debug   - Executa em modo debug (conecta GDB)"
	@echo "  make clean   - Remove arquivos compilados"
	@echo "  make tree    - Mostra estrutura do projeto"
	@echo "  make check   - Verifica integridade"
	@echo "  make help    - Mostra esta ajuda"
