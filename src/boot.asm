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
	resb 0x10000 ; 16 KiB
	stack_top:

global stack_top_syscall
align 16
	stack_bottom_syscall:
	resb 0x10000 ; 16 KiB
	stack_top_syscall:
section .text

global TEST_SYSCALL
TEST_SYSCALL:
	hlt

global PagingInit
PagingInit:
	call Paging_SetUpTables

	; setup recursive paging
	mov eax, p4_table
	or eax, 0b11 ; present + writable
	mov [p4_table + 511 * 8], eax

	call Paging_Enable

	extern loadGDT
	extern GDT_Descriptor
	;call loadGDT
	;lgdt [GDT_Descriptor]
	; mov ebx,TSS
	; mov eax,0x68
	; shl ebx,16
	; or eax,ebx
	; mov [gdt64+gdt64.tss],eax
	; mov eax,0x89
	; shl eax,8
	; mov [gdt64+gdt64.tss1],eax
	lgdt [gdt64.pointer]

	ret

global _start:function (_start.end - _start)
_start:
	mov esp, stack_top
	cli
	extern load_kernel32
	call load_kernel32
	;call PagingInit
	mov esp, stack_top
	sub esp,16
	mov eax,0xC0100000
	mov dword [eax],0xFFFFFF

	;extern long_mode_start
	jmp gdt64.code:long_mode_start
	cli
.hang:	hlt
	jmp .hang
.end:

%include "src/interrupts/idt.asm"
%include "src/interrupts/isr.asm"
%include "src/core/user.asm"
