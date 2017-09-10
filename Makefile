IDIR =
LDIR =
ODIR = obj
FLAGS = -std=c99 -O2 -Wall -lm

_OBJ = main.o tokenizer.o compiler.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))
DEPS = tokenizer.h compiler.h

$(ODIR)/%.o: %.c $(DEPS)
	gcc -c -o $@ $< $(IDIR) $(LDIR) $(FLAGS)

all: main

main: $(OBJ)
	gcc -o brainfuck $(OBJ) $(IDIR) $(LDIR) $(FLAGS)
