;;https://osdev.wiki/wiki/Multiboot1_Bare_Bones_with_NASM

%include "src/longmode/longmode.asm"
%include "src/paging/paging.asm"
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

global PagingInit
PagingInit:
	call Paging_SetUpTables

	; setup recursive paging
	mov eax, p4_table
	or eax, 0b11 ; present + writable
	mov [p4_table + 511 * 8], eax

	call Paging_Enable
	lgdt [gdt64.pointer]

	ret

global _start:function (_start.end - _start)
_start:
	mov esp, stack_top
	push eax			;;GRUB DATA
	push ebx			;;GRUB DATA
	cli
	extern load_kernel32
	call load_kernel32
	call PagingInit
	extern kernel_main
	jmp gdt64.code:long_mode_start
	cli
.hang:	hlt
	jmp .hang
.end:

%include "src/longmode/long_mode_init.asm"
