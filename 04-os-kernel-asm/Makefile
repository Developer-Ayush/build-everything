CC = gcc
LD = ld
NASM = nasm

CFLAGS = -m32 -ffreestanding -fno-stack-protector -nostdlib -nostdinc -fno-builtin -fno-pic -c
LDFLAGS = -m elf_i386 -T linker.ld

all: os.img

boot.bin: src/boot.asm
	$(NASM) -f bin $< -o $@

kernel_entry.o: src/kernel_entry.asm
	$(NASM) -f elf32 $< -o $@

kernel.o: src/kernel.c src/kernel.h
	$(CC) $(CFLAGS) $< -o $@

kernel.bin: kernel_entry.o kernel.o
	$(LD) $(LDFLAGS) $^ -o $@ --oformat binary

os.img: boot.bin kernel.bin
	cat boot.bin kernel.bin > os.img

clean:
	rm -f *.bin *.o os.img

.PHONY: all clean
