# Brainfuck compiler
Compiles Brainfuck source code to executable file.

Brainfuck programming language is described at Wikipedia: https://en.wikipedia.org/wiki/Brainfuck

Current version of this compiler produces assembly source code,
which is targeted to x86 CPU and uses Linux system calls.
Assembly source code has to be compiled with NASM to create executable binary.

Usage:

    ./bfc [source filename] [output filename]

If you see error that NASM is not installed, you should install it:

    In Debian and Debian based:
    
        sudo apt install nasm
        
    In Fedora and Rad-Hat based:
    
        sudo yum install nasm

    In Arch Linux and Arch Linux based:
    
        sudo pacman install nasm

### In the future, I will change the NASM to GAS for greater portability.
