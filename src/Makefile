CC=gcc
IDIR =
LDIR =
ODIR =obj
FLAGS =-std=c11 -O2 -Wall -g #-ffunction-sections -Wl,--gc-sections -fno-asynchronous-unwind-tables -Wl,--strip-all

_OBJ = main.o tokenizer.o compiler.o compile_asm_i386_linux.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))
DEPS = tokenizer.h compiler.h compile_asm_i386_linux.h

all: main

$(ODIR)/%.o: %.c $(DEPS)
	mkdir -p $(ODIR)
	$(CC) -c -o $@ $< $(IDIR) $(LDIR) $(FLAGS)

main: $(OBJ)
	$(CC) -o bfc $(OBJ) $(IDIR) $(LDIR) $(FLAGS)
	rm -rf $(ODIR)

