;;https://osdev.wiki/wiki/Multiboot1_Bare_Bones_with_NASM
bits 32
%include "src/longmode/longmode.asm"
%include "src/paging/paging.asm"
%include "src/longmode/long_mode_init.asm"
%include "src/multiboot.asm"

bits 32

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
	push ebx
	push eax			;;GRUB DATA
	cli
	extern load_kernel32
	call load_kernel32
	;call PagingInit
	mov esp, stack_top
	sub esp,8

	;extern long_mode_start
	jmp gdt64.code:long_mode_start
	cli
.hang:	hlt
	jmp .hang
.end:

%include "src/interrupts/idt.asm"
%include "src/interrupts/isr.asm"
%include "src/task/task.asm"
