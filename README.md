# Projeto-Sistemas-Operacionais
Projeto que visa a implementação de um Sistema Operacional básico, segundo as instruções do repositório:
https://littleosbook.github.io/

👷Integrantes do projeto👷:
Arthur Ricartte Pereira de Souza (Implementação das Unidades 2, 3, 4, 8, 9 e 14),
João Veloso Da Cruz Gouveia Segundo (Implementação das Unidades 4, 8, 9 e 14),
Pedro Benicio Lacet Da Silva (Implementação das Unidades 6, 7, 10 e os algoritmos de escalonamento),
Marquison De Sousa e Silva Junior (Implementação das Unidades 2, 3, 5, 10 e os algoritmos de escalonamento).

🔨Ferramentas Utilizadas🔨:
Ubuntu Linux - Utilizado como sistema operacional das máquinas virtuais
QEMU - Emulador x86
GRUB-MKRESCUE - Geração de ISO bootável

⚙️Compilação⚙️:
GCC - Compilador C
NASM - Montador Assembly
LD - Linker 
MAKE - Makefile

📌Antes de compilar, sugerimos que instale as dependências📌:
sudo apt update
sudo apt install -y gcc nasm binutils make qemu-system-x86 grub-common xorriso gdb git

Para executar o sistema operacional:
1 - compile⚙️:
make all 

2 - execute💻:
make run

3 - limpe os arquivos executáveis🧹:
make clean
# save-SO
