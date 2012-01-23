OBJECTS=loader.o main.o console.o panic.o memory.o itoa.o random.o dtables.o flushtables.o io.o interrupt.o isr.o pci.o

CC=gcc
CFLAGS=-Wall -Wextra -Werror -nostdlib -fno-builtin -nostartfiles -nodefaultlibs

demo: kernel.bin
	qemu-system-i386 -m 512 -kernel kernel.bin

kernel.bin: $(OBJECTS) link.ld
	ld -T link.ld -o kernel.bin $(OBJECTS)

.s.o:
	nasm -f elf $<

.PHONY: clean

clean:
	rm -f *.o kernel.bin
