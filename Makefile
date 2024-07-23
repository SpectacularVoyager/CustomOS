SOURCE=src
OUT=out
BUILD=ISO/boot
ISO=$(BUILD)/os.bin
# Default CFLAGS:
CFLAGS?=-O2 -g
# Add mandatory options to CFLAGS:
CFLAGS:=$(CFLAGS) -Wall -Wextra

all: boot kernel link build isMultiBoot

boot:
	@nasm -felf32 $(SOURCE)/boot.asm -o $(OUT)/boot.o

kernel:
	@i686-elf-gcc -c $(SOURCE)/kernel.c -o $(OUT)/kernel.o -std=gnu99 -ffreestanding $(CFLAGS)
	@i686-elf-gcc -c $(SOURCE)/printf.c -o $(OUT)/printf.o -std=gnu99 -ffreestanding $(CFLAGS)
	@#i686-elf-g++ -c kernel.c++ -o kernel.o -ffreestanding -O2 -Wall -Wextra -fno-exceptions -fno-rtti

link:
	@i686-elf-gcc -T linker.ld -o $(ISO) -ffreestanding -O2 -nostdlib $(OUT)/boot.o $(OUT)/kernel.o $(OUT)/printf.o -lgcc

build:
	@grub-mkrescue -o iso.iso ISO

isMultiBoot:
	@./isMultiBoot.sh $(ISO)
run: all
	@qemu-system-i386 -cdrom iso.iso
debug: all
	@qemu-system-i386 -cdrom iso.iso -monitor stdio
