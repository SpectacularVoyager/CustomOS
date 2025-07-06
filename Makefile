SOURCE=src
OUT=out
OUT32=out/x86
BUILD=ISO/boot
ISO=$(BUILD)/os.bin


# Default CFLAGS:
CFLAGS?=-O2 -g -DDEBUG_PRINTF
# Add mandatory options to CFLAGS:
CFLAGS:=$(CFLAGS) -Wall -Wextra -mfpmath=sse -Wno-unused-variable -Wno-unused-parameter 
QEMU=qemu-system-x86_64
CC=x86_64-elf-gcc
CC32=686-elf-gcc

QEMU_FLAGS= -cpu qemu64,+ssse3,+fpu 
QEMU_FLAGS:= $(QEMU_FLAGS)-M q35
USB?=3

USB_MEDIA=/media/ankush/USBBoot1

IMAGE=iso.iso
#QEMU_FLAGS:= $(QEMU_FLAGS) -device usb-storage,drive=fat32
ifeq ($(USB),3)
	QEMU_FLAGS:=$(QEMU_FLAGS) \
		-device nec-usb-xhci,id=xhci	\
		-device usb-kbd,bus=xhci.0\
		-device usb-mouse,bus=xhci.0
	#		-device usb-uas,id=uas,bus=xhci.0	\

endif
ifeq ($(USB),2)
	QEMU_FLAGS:=$(QEMU_FLAGS) \
		-usb -device usb-ehci,id=ehci		\
        -device usb-host,bus=usb-bus.0,hostbus=3,hostport=1 \
        -device usb-host,bus=ehci.0,hostbus=1,hostport=1	\
		-device usb-kbd \
		-device usb-mouse
endif
QEMU_FLAGS:=$(QEMU_FLAGS) -serial file:logs/serial.log -net nic,model=rtl8139 -m 2G -vga std 
QEMU_FLAGS:=$(QEMU_FLAGS) 

SRC_FILES=$(shell find -wholename "./src/*.c")
OBJECT_FILES=$(SRC_FILES:./src/%.c=out/%.o)

ASM_FILES=./src/boot.asm
OBJECT_FILES_ASM=$(ASM_FILES:./src/%.asm=out/%.o)

all: boot $(OBJECT_FILES) link build isMultiBoot

clean:
	@rm -rf $(OUT)
	@mkdir -p $(OUT)
	@mkdir -p logs
	@touch logs/serial.log

boot:$(OBJECT_FILES_ASM)

$(OBJECT_FILES_ASM):out/%.o: src/%.asm
	@echo "\033[0;32mCOMPILING ASM\t\033[0m" $<
	@nasm -g -felf64 $< -o $@

c:$(OBJECT_FILES)

$(OBJECT_FILES):out/%.o: src/%.c
	@mkdir -p $(shell dirname $@)
	@$(CC) -c $< -o $@ -std=gnu99 -ffreestanding -Isrc/include -Isrc $(CFLAGS)
	@echo "\033[0;32mCOMPILING\t\033[0m" $<

link :$(ISO)
$(ISO):$(OBJECT_FILES) $(OBJECT_FILES_ASM)
	@$(CC) -T linker.ld -o $(ISO) -ffreestanding -O2 -nostdlib $(OBJECT_FILES) $(OBJECT_FILES_ASM) -lgcc
	@echo "\033[0;34mLINKING\t\033[0m" $@


$(IMAGE):$(ISO)
	grub-mkrescue -o $(IMAGE) ISO
	@echo "\033[0;34mBUILDING\t\033[0m" $@

build: $(IMAGE)


isMultiBoot:
	@./scripts/isMultiBoot.sh $(ISO)
run: all
	@if [ -f disks/ext2.img ]; then \
		$(QEMU) $(QEMU_FLAGS) -drive file=$(IMAGE),format=raw -drive file=disks/ext2.img,format=raw; \
	else \
		$(QEMU) $(QEMU_FLAGS) -hda $(IMAGE); \
	fi

run_cfg: build isMultiBoot
	@sudo $(QEMU) $(QEMU_FLAGS) -hda $(IMAGE)

part:
	@$(QEMU) $(QEMU_FLAGS) -bios /usr/share/ovmf/OVMF.fd /dev/sdc

# INSPECT MEM x/128b 0xfee00000
debug: all
	@$(QEMU) $(QEMU_FLAGS) -hda $(IMAGE) -hdb disks/fat.img -monitor stdio
gdb: all
	# @$(QEMU) -s -S -net nic,model=e1000 -hda $(IMAGE)

	@if [ -f disks/ext2.img ]; then \
		$(QEMU) -s -S $(QEMU_FLAGS) -drive file=$(IMAGE),format=raw -drive file=disks/ext2.img,format=raw; \
	else \
		$(QEMU) -s -S $(QEMU_FLAGS) -hda $(IMAGE); \
	fi
	# TO RUN
	# make gdb
	# gdb ISO/boot/os.bin
	# In GDB: target remote :1234
drive:
	dd if=$(IMAGE) of=/dev/sda status=progress

log:
	@truncate -s 0 logs/serial.log
	@tail -f logs/serial.log 2> /dev/null

install:
	sudo grub-install --root-directory=/mnt --no-floppy --modules="normal part_msdos ext2 multiboot " /dev/loop40

usb:
	@cp ISO/boot/grub/grub.cfg $(USB_MEDIA)/boot/grub/grub.cfg
	@cp ISO/boot/os.bin $(USB_MEDIA)/boot/os.bin

losetup:
	@sudo losetup -Pf disks/ext2.img
errors:
	make 2>&1 >/dev/null | grep -i 'error'

##### TO MAKE USB BOOTABLE #####
##		sudo mkfs.vfat -F 32 -n USBBoot -I /dev/sda
##		sudo grub-install --root-directory=/media/ankush/USBBoot/ --no-floppy --recheck --force /dev/sda
################################

####	TO CREATE DISK IMAGE FOR FS	####
##	dd if=/dev/zero of=ext2.img bs=1G count=1
