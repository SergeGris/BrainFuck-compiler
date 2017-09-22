ODIR = obj
COMPILER=../brainfuck
ASSEMBLER=nasm -f elf32
LINKER=ld -m elf_i386 -s -o

hello-world: hello-world.o
	$(LINKER) $@ $(ODIR)/$<

%.o: $(ODIR)/%.asm
	$(ASSEMBLER) $<

$(ODIR)/%.asm: %.b $(COMPILER)
	mkdir -p $(ODIR)
	$(COMPILER) $< $@

all: hello-world

.PHONY: clean

clean:
	rm -rf $(ODIR)
	rm -f hello-world
