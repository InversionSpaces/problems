CC = g++

INCDIR = inc
BASESRC = src/binaryfile.c src/command.c src/cpu.c src/exitingalloc.c src/files.c src/memory.c src/stack.c src/tokenizer.c
ASMSRC = src/asmmain.c
CPUSRC = src/cpumain.c
DISASMSRC = src/disasmmain.c

all: clean asm cpu

clean:
	rm -f asm cpu

run: all
	./asm test.vm test.bin
	./cpu test.bin
	
asm:
	$(CC) -I$(INCDIR) $(BASESRC) $(ASMSRC) -o asm

cpu:
	$(CC) -I$(INCDIR) $(BASESRC) $(CPUSRC) -o cpu
