# Brainfuck compiler
Compiles Brainfuck source code to executable file.

Brainfuck programming language is described at Wikipedia: https://en.wikipedia.org/wiki/Brainfuck

Current version of this compiler produces assembly source code,
which is targeted to x86 CPU and uses Linux system calls.
Assembly source code has to be compiled with NASM to create executable binary.

Usage, if you wanna to compile the BrainFuck code:

    ./bfc [source filename] [output filename]
    
Usage, if you wanna to translate the BrainFuck code to NASM, without compile and link:

    ./bft [source filename] [output filename]

**NOTICE:
bf*c* is BrainFuck Compiler, and
bf*t* is BrainFuck Translator**

If you see error that NASM is not installed, you should install it:

    In Debian and Debian based:
    
        sudo apt install nasm
        
    In Fedora and Rad-Hat based:
    
        sudo yum install nasm

    In Arch Linux and Arch Linux based:
    
        sudo pacman install nasm

###### In the future, I will change the NASM to GAS for greater portability.
