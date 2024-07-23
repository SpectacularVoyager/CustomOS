;;https://osdev.wiki/wiki/Multiboot1_Bare_Bones_with_NASM

%include "src/gdt.asm"
MBALIGN  equ  1 << 0            ; align loaded modules on page boundaries
MEMINFO  equ  1 << 1            ; provide memory map
MBFLAGS  equ  MBALIGN | MEMINFO ; this is the Multiboot 'flag' field
MAGIC    equ  0x1BADB002        ; 'magic number' lets bootloader find the header
CHECKSUM equ -(MAGIC + MBFLAGS)   ; checksum of above, to prove we are multiboot

section .multiboot
align 4
	dd MAGIC
	dd MBFLAGS
	dd CHECKSUM

section .bss
align 16
	stack_bottom:
	resb 16384 ; 16 KiB
	stack_top:
section .text

global _start:function (_start.end - _start)
_start:
	mov esp, stack_top
	cli
	extern kernel_main
	call kernel_main
	cli
.hang:	hlt
	jmp .hang
.end:
