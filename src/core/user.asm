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
;USER_JUMP_ASM(registers* r,void* entry,void* stack);
; USER_JUMP_ASM:
; 	mov rax,0x20 | USER_PREV
; 	mov ds,ax
; 	mov es,ax
;
;     ; Build a fake iret frame
; 	push rax							; 
; 	push rdx							; USER STACK
; 	push 0x202							; RFLAGS INT ENABLE AND RESERVED
; 	push 0x18 | USER_PREV				; Selector
; 	push rsi							; ENTRY POINT
;
; 	iretq
USER_JUMP_ASM:
	mov rbx,rsi
	;;USERSPACE ADDRESS
	mov rcx,rbx
	mov	r11,0x202	;EFLAGS
	o64 sysret

global syscall_inst_asm
extern syscall_inst
syscall_inst_asm:
	push rcx
	push r11
	cli
	mov rsp,stack_top_syscall
	mov rbx,0xFD000000
	mov dword [rbx],0x00FF00
	mov rdi,rcx
	call syscall_inst
	sti
	pop r11
	pop rcx
	jmp USER_MODE_RETURN

global USER_MODE_RETURN
USER_MODE_RETURN:
	mov rcx,rdi
	mov r11,rsi
	o64 sysret
