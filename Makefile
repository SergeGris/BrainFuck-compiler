IDIR =
LDIR =
ODIR = obj
FLAGS = -std=c99 -O2 -Wall -lm

_OBJ = main.o tokenizer.o compiler.o compile_asm_x86_linux.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))
DEPS = tokenizer.h compiler.h compile_asm_x86_linux.h

all: main tests

$(ODIR)/%.o: %.c $(DEPS)
	mkdir -p $(ODIR)
	gcc -c -o $@ $< $(IDIR) $(LDIR) $(FLAGS)

main: $(OBJ)
	gcc -o brainfuck $(OBJ) $(IDIR) $(LDIR) $(FLAGS)

examples:
	cd examples && $(MAKE) -B

tests:
	cd tests && $(MAKE) -B

.PHONY: clean examples all tests

clean:
	rm -rf $(ODIR)
	rm -f brainfuck
