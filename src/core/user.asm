[bits 64]
global USER_JUMP_ASM
extern func
USER_JUMP_ASM:
	mov ax, gdt64.userdata | 3 ; ring 3 data with bottom 2 bits set for ring 3
	mov ds, ax
	mov es, ax 
	mov fs, ax 
	mov gs, ax ; SS is handled by iret

	;; set up the stack frame iret expects
	mov rax, rsp
	push gdt64.userdata | 3 ; data selector
	push rax ; current esp
	pushf ; eflags
	push gdt64.usercode | 3 ; code selector (ring 3 code with bottom 2 bits set for ring 3)
	push func ; instruction address to return to
	o64 iretq
