SOURCE=src
OUT=out
OUT32=out/x86
BUILD=ISO/boot
ISO=$(BUILD)/os.bin
# Default CFLAGS:
CFLAGS?=-O2 -g
# Add mandatory options to CFLAGS:
CFLAGS:=$(CFLAGS) -Wall -Wextra
QEMU=qemu-system-x86_64
CC=x86_64-elf-gcc
CC32=i686-elf-gcc

all: clean boot kernel link build isMultiBoot

clean:
	@rm -r $(OUT)
boot:
	@mkdir -p out/x86
	@nasm -felf64 $(SOURCE)/boot.asm -o $(OUT)/boot.o

kernel:
	@$(CC) -c $(SOURCE)/kernel32.c -o $(OUT)/kernel32.o -std=gnu99 -ffreestanding $(CFLAGS)
	@$(CC) -c $(SOURCE)/kernel.c -o $(OUT)/kernel.o -std=gnu99 -ffreestanding $(CFLAGS)
	@$(CC) -c $(SOURCE)/vga/term.c -o $(OUT)/term.o -std=gnu99 -ffreestanding $(CFLAGS)
	@$(CC) -c $(SOURCE)/printf/printf.c -o $(OUT)/printf.o -std=gnu99 -ffreestanding $(CFLAGS)
	@$(CC) -c $(SOURCE)/interrupts/idt.c -o $(OUT)/idt.o -std=gnu99 -ffreestanding $(CFLAGS)
	@$(CC) -c $(SOURCE)/interrupts/isr.c -o $(OUT)/isr.o -std=gnu99 -ffreestanding $(CFLAGS)
	@$(CC) -c $(SOURCE)/interrupts/isr.c -o $(OUT)/isr.o -std=gnu99 -ffreestanding $(CFLAGS)
	@$(CC) -c $(SOURCE)/interrupts/irq.c -o $(OUT)/irq.o -std=gnu99 -ffreestanding $(CFLAGS)
	@$(CC) -c $(SOURCE)/interrupts/isrgen.c -o $(OUT)/isrgen.o -std=gnu99 -ffreestanding $(CFLAGS)
	@$(CC) -c $(SOURCE)/drivers/pic.c -o $(OUT)/pic.o -std=gnu99 -ffreestanding $(CFLAGS)

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
