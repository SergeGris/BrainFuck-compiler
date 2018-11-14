
/*
IDIR =
LDIR =
ODIR = obj
FLAGS = -std=c11 -O3 -Wall -lm

_OBJ = main.o tokenizer.o compiler.o compile_asm_i386_linux.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))
DEPS = tokenizer.h compiler.h compile_asm_i386_linux.h

all: main tests

$(ODIR)/%.o: %.c $(DEPS)
	mkdir -p $(ODIR)
	gcc -c -o $@ $< $(IDIR) $(LDIR) $(FLAGS)

main: $(OBJ)
	gcc -o brainfuck $(OBJ) $(IDIR) $(LDIR) $(FLAGS)

examples:
	cd examples && $(MAKE) -B

#tests:
#	cd tests && $(MAKE) -B

.PHONY: clean examples all tests

clean:
	rm -rf $(ODIR)
	rm -f brainfuck


ODIR = obj
COMPILER=../brainfuck
ASSEMBLER=nasm -f elf32
LINKER=ld -m elf_i386 -s -o
OUTFILES=hello-world print-a cat infinite-loop

all: $(OUTFILES)

hello-world: hello-world.o
	$(LINKER) $@ $(ODIR)/$<

print-a: print-a.o
	$(LINKER) $@ $(ODIR)/$<

cat: cat.o
	$(LINKER) $@ $(ODIR)/$<

infinite-loop: infinite-loop.o
	$(LINKER) $@ $(ODIR)/$<

%.o: $(ODIR)/%.asm
	$(ASSEMBLER) $<

$(ODIR)/%.asm: %.b $(COMPILER)
	mkdir -p $(ODIR)
	$(COMPILER) $< $@

.PHONY: clean all

.PRECIOUS: $(ODIR)/%.asm

clean:
	rm -rf $(ODIR)
	rm -f $(OUTFILES)
    
    
    
cd examples && make -B
make[1]: вход в каталог «/home/user/Загрузки/brainfuck/examples»
mkdir -p obj
../brainfuck hello-world.b obj/hello-world.asm
nasm -f elf32 obj/hello-world.asm
ld -m elf_i386 -s -o hello-world obj/hello-world.o
mkdir -p obj
../brainfuck print-a.b obj/print-a.asm
nasm -f elf32 obj/print-a.asm
ld -m elf_i386 -s -o print-a obj/print-a.o
mkdir -p obj
../brainfuck cat.b obj/cat.asm
nasm -f elf32 obj/cat.asm
ld -m elf_i386 -s -o cat obj/cat.o
mkdir -p obj
../brainfuck infinite-loop.b obj/infinite-loop.asm
nasm -f elf32 obj/infinite-loop.asm
ld -m elf_i386 -s -o infinite-loop obj/infinite-loop.o
*/

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
        printf("\nCount of arguments can not be more than 2.\nIf you want to compile more than 1 BrainFuck source code, you have to ...\nFUCK!\nYou seriously want to compile more than 1 file?!\nIf yes - fuck you")
    }
    char *in_filename = argv[1];
    char *out_filename = argv[2];
    
    // ../brainfuck nameofbf.bf obj/nameofasm.asm
    // nasm -f elf32 obj/nameofasm.asm
    // ld -m elf_i386 -s -o nameofelf nameofobj.o
    const char *bfcompiler = "./brainfuck ";
    const char *asmdir = " ";
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
    strcat(asm, " ");
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
