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
	@i686-elf-gcc -c $(SOURCE)/term.c -o $(OUT)/term.o -std=gnu99 -ffreestanding $(CFLAGS)
	@i686-elf-gcc -c $(SOURCE)/gdt.c -o $(OUT)/gdt.o -std=gnu99 -ffreestanding $(CFLAGS)
	@i686-elf-gcc -c $(SOURCE)/idt.c -o $(OUT)/idt.o -std=gnu99 -ffreestanding $(CFLAGS)
	@i686-elf-gcc -c $(SOURCE)/isr.c -o $(OUT)/isr.o -std=gnu99 -ffreestanding $(CFLAGS)
	@i686-elf-gcc -c $(SOURCE)/pic.c -o $(OUT)/pic.o -std=gnu99 -ffreestanding $(CFLAGS)
	@i686-elf-gcc -c $(SOURCE)/io.c -o $(OUT)/io.o -std=gnu99 -ffreestanding $(CFLAGS)
	@i686-elf-gcc -c $(SOURCE)/irq.c -o $(OUT)/irq.o -std=gnu99 -ffreestanding $(CFLAGS)
	@i686-elf-gcc -c $(SOURCE)/pit.c -o $(OUT)/pit.o -std=gnu99 -ffreestanding $(CFLAGS)
	@i686-elf-gcc -c $(SOURCE)/paging.c -o $(OUT)/paging.o -std=gnu99 -ffreestanding $(CFLAGS)
	@i686-elf-gcc -c $(SOURCE)/drivers/keyboard.c -o $(OUT)/keyboard.o -std=gnu99 -ffreestanding $(CFLAGS)
	@i686-elf-gcc -c $(SOURCE)/drivers/timer.c -o $(OUT)/timer.o -std=gnu99 -ffreestanding $(CFLAGS)
	@i686-elf-gcc -c $(SOURCE)/drivers/pci.c -o $(OUT)/pci.o -std=gnu99 -ffreestanding $(CFLAGS)
	@i686-elf-gcc -c $(SOURCE)/drivers/8254x.c -o $(OUT)/8254x.o -std=gnu99 -ffreestanding $(CFLAGS)
	@#i686-elf-g++ -c kernel.c++ -o kernel.o -ffreestanding -O2 -Wall -Wextra -fno-exceptions -fno-rtti

link:
	@i686-elf-gcc -T linker.ld -o $(ISO) -ffreestanding -O2 -nostdlib $(shell find -name '*.o') -lgcc

build:
	@grub-mkrescue -o iso.iso ISO

isMultiBoot:
	@./isMultiBoot.sh $(ISO)
run: all
	@qemu-system-i386 -net nic,model=rtl8139 -hda iso.iso
debug: all
	@qemu-system-i386 -net nic,model=e1000 -hda iso.iso -monitor stdio
