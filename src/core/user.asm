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
	mov rbx,0xFD000000
	mov dword [rbx],0x00FF00
	mov rbx,0xFD000004
	mov dword [rbx],0xFF0000
	mov rax,1
	int 0x80
	mov rax,60
	mov rdi,0
	int 0x80
	hlt
;; https://f.osdev.org/viewtopic.php?t=40894
;USER_JUMP_ASM(void* args,void* entry,void* stack);
USER_JUMP_ASM:
	mov rax,0x20 | USER_PREV
	mov ds,ax
	mov es,ax

    ; Build a fake iret frame
	push rax							; 
	push rdx							; USER STACK
	push 0x202							; RFLAGS INT ENABLE AND RESERVED
	push 0x18 | USER_PREV				; Selector
	push rsi							; ENTRY POINT

	iretq
