IDIR=
LDIR=
ODIR=obj
FLAGS=-std=c11 -O3 -Wall -march=native -flto -funroll-loops -ffunction-sections -Wl,--gc-sections -fno-asynchronous-unwind-tables -Wl,--strip-all

_OBJ = main.o tokenizer.o compiler.o compile_asm_i386_linux.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))
DEPS = tokenizer.h compiler.h compile_asm_i386_linux.h

all: main

$(ODIR)/%.o: %.c $(DEPS)
	mkdir -p $(ODIR)
	gcc -c -o $@ $< $(IDIR) $(LDIR) $(FLAGS)

main: $(OBJ)
	gcc -o bft $(OBJ) $(IDIR) $(LDIR) $(FLAGS)
	gcc bfc.c -o bfc $(IDIR) $(LDIR) $(FLAGS)
	mv bfc ../
	mv bft ../
	rm -rf $(ODIR)

