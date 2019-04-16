CC=gcc
IDIR=
LDIR=
ODIR=obj
CFLAGS=-std=c11 -O2 -Wall # -g

_OBJ=main.o tokenizer.o compiler.o compile_asm_i386_linux.o
OBJ=$(patsubst %,$(ODIR)/%,$(_OBJ))
DEPS=tokenizer.h compiler.h compile_asm_i386_linux.h
TARGET=bfc

all: main

$(ODIR)/%.o: %.c $(DEPS)
	mkdir -p $(ODIR)
	$(CC) -c -o $@ $< $(IDIR)$(LDIR)$(CFLAGS)

main: $(OBJ)
	$(CC) -o $(TARGET) $(OBJ) $(IDIR)$(LDIR)$(CFLAGS)
	rm -rf $(ODIR)

