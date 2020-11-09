AS=gcc -m32 -c -g -mgeneral-regs-only -mno-red-zone
CC=gcc -m32 -g -mgeneral-regs-only -mno-red-zone
LD=gcc -m32

build: kernel.bin

run: build
	qemu-system-i386 -kernel kernel.bin

boot.o: boot.s
	$(AS) boot.s -o boot.o

kernel.o: kernel.c
	$(CC) -c kernel.c -o kernel.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra

interrupts.o: interrupts.c
	$(CC) -c interrupts.c -o interrupts.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra

printf.o: printf.c
	$(CC) -c printf.c -o printf.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra

kernel.bin: kernel.o boot.o interrupts.o printf.o
	$(LD) -T linker.ld -o kernel.bin -ffreestanding -O2 -nostdlib boot.o interrupts.o kernel.o printf.o -lgcc

.PHONY: build


# For CLion

clean:
	rm boot.o kernel.o interrupts.o printf.o kernel.bin

all: build
