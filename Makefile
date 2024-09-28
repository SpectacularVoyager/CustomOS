SOURCE=src
OUT=out
OUT32=out/x86
BUILD=ISO/boot
ISO=$(BUILD)/os.bin
# Default CFLAGS:
CFLAGS?=-O2 -g -DDEBUG_PRINTF
# Add mandatory options to CFLAGS:
CFLAGS:=$(CFLAGS) -Wall -Wextra -mfpmath=sse
QEMU=qemu-system-x86_64
CC=x86_64-elf-gcc
CC32=686-elf-gcc

QEMU_FLAGS= -cpu qemu64,+ssse3,+fpu 
QEMU_FLAGS:= -M q35
QEMU_FLAGS:=$(QEMU_FLAGS) -usb -device usb-ehci,id=ehci		\
        -device usb-host,bus=usb-bus.0,hostbus=3,hostport=1 \
        -device usb-host,bus=ehci.0,hostbus=1,hostport=1
QEMU_FLAGS:=$(QEMU_FLAGS) -serial file:logs/serial.log -net nic,model=rtl8139 -m 2G -vga std -hda

objects = $(shell find -name "*.c")
objects := ${objects:.c=.o}
all: clean boot $(objects) link build isMultiBoot

clean:
	@rm -r $(OUT)
	mkdir -p $(OUT)
boot:
	@nasm -g -felf64 $(SOURCE)/boot.asm -o $(OUT)/boot.o

$(objects): %.o: %.c
	@mkdir -p $(shell dirname $(patsubst src/%,out/%,$@))
	@$(CC) -c $^ -o $(patsubst src/%,out/%,$@) -std=gnu99 -ffreestanding $(CFLAGS)

link:
	@$(CC) -T linker.ld -o $(ISO) -ffreestanding -O2 -nostdlib $(shell find -name '*.o') -lgcc

build:
	@grub-mkrescue -o iso.iso ISO

isMultiBoot:
	@./isMultiBoot.sh $(ISO)
run: all
	@$(QEMU) $(QEMU_FLAGS) iso.iso

# INSPECT MEM x/128b 0xfee00000
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
	@truncate -s 0 logs/serial.log
	@tail -f logs/serial.log 2> /dev/null

install:
	sudo grub-install --root-directory=/mnt --no-floppy --modules="normal part_msdos ext2 multiboot " /dev/loop40

usb:
	@cp ISO/boot/os.bin /media/ankush/EFI\ SYSTEM/boot/os.bin

