[bits 64]
global USER_JUMP_ASM
extern func
global TEST_USER

USER_PREV equ 3

TEST_USER:
	mov rbx,0xFD000000
	mov dword [rbx],0xFF00FF
	hlt
global TEST_HALT
extern sayHello
TEST_HALT:
	; mov rbx,0xFD000000
	; mov dword [rbx],0x00ff00
	; int 0x80
	; int 0x80
	hlt
;; https://f.osdev.org/viewtopic.php?t=40894
;USER_JUMP_ASM(void* args,void* entry,void* stack);
USER_JUMP_ASM:

	;;;;mov ax, gdt64.userdata | 3 ; ring 3 data with bottom 2 bits set for ring 3
	;;;;mov ds, ax
	;;;;mov es, ax 
	;;;;mov fs, ax 
	;;;;mov gs, ax ; SS is handled by iret

	;;;;;; set up the stack frame iret expects
	;;;;mov rax, rsp
	;;;;push gdt64.userdata | 3 ; data selector
	;;;;push rax ; current esp
	;;;;pushf ; eflags
	;;;;push gdt64.usercode | 3 ; code selector (ring 3 code with bottom 2 bits set for ring 3)
	;;;;push func ; instruction address to return to
	;;;;o64 iretq

    ; rdi = user args
    ; rsi = entry point in user space
    ; rdx = user space stack

	mov rax,0x20 | USER_PREV
	mov ds,ax
	mov es,ax

    ; Build a fake iret frame
	push rax							; 
	push rdx							; USER STACK
	push 0x202							; RFLAGS INT ENABLE AND RESERVED
	push 0x18 | USER_PREV		; Selector
	push rsi							; ENTRY POINT

	iretq
