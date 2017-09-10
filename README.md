# brainfuck-compiler
Compiles Brainfuck source code to executable file.

Brainfuck programming language is described at Wikipedia: https://en.wikipedia.org/wiki/Brainfuck

Current version of this compiler produces assembly source code, which is targeted to x86 CPU and uses Linux system calls. Code uses (NASM syntax)[https://en.wikibooks.org/wiki/X86_Assembly/NASM_Syntax].

Assembly source code has to be compiled with NASM to create executable binary.