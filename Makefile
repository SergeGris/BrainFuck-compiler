IDIR =
LDIR =
ODIR = obj
FLAGS = -std=c99 -O2 -Wall -lm

_OBJ = main.o tokenizer.o compiler.o compile_asm_x86_linux.o compile_elf_x86_linux.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))
DEPS = tokenizer.h compiler.h compile_asm_x86_linux.h compile_elf_x86_linux.h

all: main

$(ODIR)/%.o: %.c $(DEPS)
	mkdir -p $(ODIR)
	gcc -c -o $@ $< $(IDIR) $(LDIR) $(FLAGS)

main: $(OBJ)
	gcc -o brainfuck $(OBJ) $(IDIR) $(LDIR) $(FLAGS)

examples:
	cd examples && $(MAKE) -B

.PHONY: clean examples all

clean:
	rm -rf $(ODIR)
	rm -f brainfuck
