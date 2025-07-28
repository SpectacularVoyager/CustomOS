[bits 64]
global USER_JUMP_ASM
extern func
global TEST_USER

USER_PREV equ 3

global USER_PRIV_LOOP
USER_PRIV_LOOP:
	mov rax,11
	jmp USER_PRIV_LOOP

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
;USER_JUMP_ASM(registers* r,void* entry,void* stack,void* tib);
USER_JUMP_ASM:
	mov rax,0x20 | USER_PREV
	mov ds,ax
	mov es,ax

    ; Build a fake iret frame
	push rax							;
	push rdx							; USER STACK
	push 0x202|0x3000					; RFLAGS INT ENABLE AND RESERVED
	push 0x18 | USER_PREV				; Selector
	push rsi							; ENTRY POINT
	
	mov	rax,0xC0000100
	mov rdx,rcx
	shr rdx,32
	wrmsr

	mov r15,[rdi+0x8]
	mov r14,[rdi+0x10]
	mov r13,[rdi+0x18]
	mov r12,[rdi+0x20]
	mov r11,[rdi+0x28]
	mov r10,[rdi+0x30]
	mov r9 ,[rdi+0x38]
	mov r8 ,[rdi+0x40]
	mov rbp,[rdi+0x48]
	mov rsi,[rdi+0x58]
	mov rdx,[rdi+0x60]
	mov rcx,[rdi+0x68]
	mov rbx,[rdi+0x70]
	mov rax,[rdi+0x78]

	mov rdi,[rdi+0x50]
	iretq
