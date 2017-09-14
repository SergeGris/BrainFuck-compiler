# Brainfuck compiler
Compiles Brainfuck source code to executable file.

Brainfuck programming language is described at Wikipedia: https://en.wikipedia.org/wiki/Brainfuck

Current version of this compiler produces assembly source code, which is targeted to x86 CPU and uses Linux system calls. Assembly source code has to be compiled with NASM to create executable binary.

Usage:

    ./brainfuck [source filename] [output filename]

Compiling and running Hello World example (requires NASM):

    make
    make examples
    ./examples/hello-world
