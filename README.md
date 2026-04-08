# Projeto-Sistemas-Operacionais

Tem como objetivo implementar um Sistema Operacional básico, seguindo as instruções do repositório:
https://littleosbook.github.io/

---

## 👷 Integrantes do projeto 👷

- **Arthur Ricartte Pereira de Souza** - Implementação das Unidades 2, 3, 4, 8, 9 e 14
- **João Veloso Da Cruz Gouveia Segundo** - Implementação das Unidades 4, 8, 9 e 14
- **Pedro Benicio Lacet Da Silva** - Implementação das Unidades 6, 7, 10 e os algoritmos de escalonamento
- **Marquison De Sousa e Silva Junior** - Implementação das Unidades 2, 3, 5, 10 e os algoritmos de escalonamento

---

## 🔨 Ferramentas Utilizadas 🔨

- **Ubuntu Linux** - Sistema operacional das máquinas virtuais
- **QEMU** - Emulador x86
- **GRUB-MKRESCUE** - Geração de ISO bootável

---

## ⚙️ Compiladores ⚙️

- **GCC** - Compilador C
- **NASM** - Montador Assembly
- **LD** - Linker
- **MAKE** - Makefile

---

## 📌 Dependências 📌

Antes de compilar, sugerimos que instale as dependências:

```bash
sudo apt update
sudo apt install -y gcc nasm binutils make qemu-system-x86 grub-common xorriso gdb git
```
---

## 🧑‍💻 Como compilar ? 🧑‍💻
- **make all** - compila todos os arquivos, gerando executáveis
- **make run** - executa os arquivos,
- **make clean** - remove todos os arquivos executáveis
