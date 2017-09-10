./brainfuck hello-world.b result.asm
nasm -f elf32 result.asm && ld -m elf_i386 -s -o result result.o && ./result