SOURCE=src
OUT=out
BUILD=ISO/boot
ISO=$(BUILD)/os.bin
# Default CFLAGS:
CFLAGS?=-O2 -g
# Add mandatory options to CFLAGS:
CFLAGS:=$(CFLAGS) -Wall -Wextra
QEMU=qemu-system-x86_64
CC=x86_64-elf-gcc

all: boot kernel link build isMultiBoot

boot:
	@nasm -felf64 $(SOURCE)/boot.asm -o $(OUT)/boot.o

kernel:
	@$(CC) -c $(SOURCE)/kernel.c -o $(OUT)/kernel.o -std=gnu99 -ffreestanding $(CFLAGS)

link:
	@$(CC) -T linker.ld -o $(ISO) -ffreestanding -O2 -nostdlib $(shell find -name '*.o') -lgcc

build:
	@grub-mkrescue -o iso.iso ISO

isMultiBoot:
	@./isMultiBoot.sh $(ISO)
run: all
	@$(QEMU) -net nic,model=rtl8139 -hda iso.iso
debug: all
	@$(QEMU) -net nic,model=e1000 -hda iso.iso -monitor stdio
