IDIR =
LDIR =
ODIR =obj
FLAGS =-std=c11 -O3 -Wall

_OBJ = main.o tokenizer.o compiler.o compile_asm_i386_linux.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))
DEPS = tokenizer.h compiler.h compile_asm_i386_linux.h

all: main examples

$(ODIR)/%.o: %.c $(DEPS)
	mkdir -p $(ODIR)
	gcc -c -o $@ $< $(IDIR) $(LDIR) $(FLAGS)

main: $(OBJ)
	gcc -o brainfuck $(OBJ) $(IDIR) $(LDIR) $(FLAGS)
	gcc bfc.c -o bfc $(IDIR) $(LDIR) $(FLAGS)
examples:
	cd examples && $(MAKE) -B

#tests:
#	cd tests && $(MAKE) -B

.PHONY: clean examples all

clean:
	rm -rf $(ODIR)
	rm -f brainfuck
