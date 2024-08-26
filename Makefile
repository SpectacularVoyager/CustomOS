SOURCE=src
OUT=out
OUT32=out/x86
BUILD=ISO/boot
ISO=$(BUILD)/os.bin
# Default CFLAGS:
CFLAGS?=-O2 -g -DDEBUG
# Add mandatory options to CFLAGS:
CFLAGS:=$(CFLAGS) -Wall -Wextra
QEMU=qemu-system-x86_64
CC=x86_64-elf-gcc
CC32=i686-elf-gcc

QEMU_FLAGS= -serial file:logs/serial.log -net nic,model=rtl8139 -m 512M -vga std -hda 

all: clean boot kernel link build isMultiBoot

clean:
	@rm -r $(OUT)
boot:
	@mkdir -p out/x86
	@nasm -g -felf64 $(SOURCE)/boot.asm -o $(OUT)/boot.o

kernel:
	@$(CC) -c $(SOURCE)/kernel32.c -o $(OUT)/kernel32.o -std=gnu99 -ffreestanding $(CFLAGS)
	@$(CC) -c $(SOURCE)/kernel.c -o $(OUT)/kernel.o -std=gnu99 -ffreestanding $(CFLAGS)
	@$(CC) -c $(SOURCE)/vga/term.c -o $(OUT)/term.o -std=gnu99 -ffreestanding $(CFLAGS)
	@$(CC) -c $(SOURCE)/printf/printf.c -o $(OUT)/printf.o -std=gnu99 -ffreestanding $(CFLAGS)
	@$(CC) -c $(SOURCE)/SerialPrintf/printf.c -o $(OUT)/SerialPrintf.o -std=gnu99 -ffreestanding $(CFLAGS)
	@$(CC) -c $(SOURCE)/stdlib/string.c -o $(OUT)/string.o -std=gnu99 -ffreestanding $(CFLAGS)
	@$(CC) -c $(SOURCE)/paging/paging.c -o $(OUT)/paging.o -std=gnu99 -ffreestanding $(CFLAGS)
	@$(CC) -c $(SOURCE)/interrupts/idt.c -o $(OUT)/idt.o -std=gnu99 -ffreestanding $(CFLAGS)
	@$(CC) -c $(SOURCE)/interrupts/isr.c -o $(OUT)/isr.o -std=gnu99 -ffreestanding $(CFLAGS)
	@$(CC) -c $(SOURCE)/interrupts/isr.c -o $(OUT)/isr.o -std=gnu99 -ffreestanding $(CFLAGS)
	@$(CC) -c $(SOURCE)/interrupts/irq.c -o $(OUT)/irq.o -std=gnu99 -ffreestanding $(CFLAGS)
	@$(CC) -c $(SOURCE)/exceptions/exceptions.c -o $(OUT)/exceptions.o -std=gnu99 -ffreestanding $(CFLAGS)
	@$(CC) -c $(SOURCE)/interrupts/isrgen.c -o $(OUT)/isrgen.o -std=gnu99 -ffreestanding $(CFLAGS)
	@$(CC) -c $(SOURCE)/drivers/pic.c -o $(OUT)/pic.o -std=gnu99 -ffreestanding $(CFLAGS)
	@$(CC) -c $(SOURCE)/drivers/pci.c -o $(OUT)/pci.o -std=gnu99 -ffreestanding $(CFLAGS)
	@$(CC) -c $(SOURCE)/drivers/serial.c -o $(OUT)/serial.o -std=gnu99 -ffreestanding $(CFLAGS)

	@$(CC) -c $(SOURCE)/graphics/graphics.c -o $(OUT)/graphics.o -std=gnu99 -ffreestanding $(CFLAGS)
	@$(CC) -c $(SOURCE)/devices/keyboard.c -o $(OUT)/keyboard.o -std=gnu99 -ffreestanding $(CFLAGS)
	@$(CC) -c $(SOURCE)/disk/atapio/atapio.c -o $(OUT)/atapio.o -std=gnu99 -ffreestanding $(CFLAGS)
	@$(CC) -c $(SOURCE)/disk/iso/iso.c -o $(OUT)/iso.o -std=gnu99 -ffreestanding $(CFLAGS)

	@$(CC) -c $(SOURCE)/processes/processes.c -o $(OUT)/process.o -std=gnu99 -ffreestanding $(CFLAGS)

link:
	@$(CC) -T linker.ld -o $(ISO) -ffreestanding -O2 -nostdlib $(shell find -name '*.o') -lgcc

build:
	@grub-mkrescue -o iso.iso ISO

isMultiBoot:
	@./isMultiBoot.sh $(ISO)
run: all
	@$(QEMU) $(QEMU_FLAGS) iso.iso
debug: all
	@$(QEMU) $(QEMU_FLAGS) iso.iso -monitor stdio
gdb: all
	@$(QEMU) -s -S -net nic,model=e1000 -hda iso.iso
	# TO RUN
	# make gdb
	# gdb ISO/boot/os.bin
	# In GDB: target remote :1234
drive:
	dd if=iso.iso of=/dev/sda status=progress

log:
	@tail -f logs/serial.log 2> /dev/null
