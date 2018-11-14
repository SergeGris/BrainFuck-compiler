#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv)
{
    if (argc <= 2)
    {
        printf("Usage: ./bfc [source filename] [output filename]\n");
        exit(1);
    }
    if (argc > 3)
    {
        printf("\nCount of arguments can not be more than 2.\nIf you want to compile more than 1 BrainFuck source code, you have to ...\nFUCK!\nYou seriously want to compile more than 1 file?\nIf yes -- fuck you\n");
    }
    char *in_filename = argv[1];
    char *out_filename = argv[2];
    
    // ../brainfuck nameofbf.bf obj/nameofasm.asm
    // nasm -f elf32 obj/nameofasm.asm
    // ld -m elf_i386 -s -o nameofelf nameofobj.o
    const char *bfcompiler = "./brainfuck ";
    const char *nasmflags = "nasm -f elf32 ";
    const char *ldflags = "ld -m elf_i386 -s -o ";
    const char *objext = ".o ";
    
    char compile[1024];
    strcpy(compile, bfcompiler);
    strcat(compile, in_filename);
    strcat(compile, " ");
    strcat(compile, out_filename);
    printf("%s\n", compile);
    system(compile);
    char asm[1024];
    strcpy(asm, nasmflags);
    strcat(asm, out_filename);
    printf("%s\n", asm);
    system(asm);
    char link[1024];
    strcpy(link, ldflags);
    strcat(link, out_filename);
    strcat(link, " ");
    strcat(link, out_filename);
    strcat(link, objext);
    printf("%s\n", link);
    system(link);
    
    return 0;
}
