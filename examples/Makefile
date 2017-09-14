ODIR = obj

LINKER=ld -m elf_i386 -s -o

hello-world: hello-world.o
	$(LINKER) $@ $(ODIR)/$<

%.o: $(ODIR)/%.asm
	nasm -f elf32 $<

$(ODIR)/%.asm: %.b
	mkdir -p $(ODIR)
	../brainfuck $< $@

all: hello-world

.PHONY: clean

clean:
	rm -rf $(ODIR)
	rm -f hello-world
