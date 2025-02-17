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

syscall_push_all:
	push rax
	push rbx
	push rcx
	push rdx
	push rsi
	push rdi
	push rbp
	push r8
	push r9
	push r10
	push r11
	push r12
	push r13
	push r14
	push r15
	ret
syscall_pop_all:
	pop r15
	pop r14
	pop r13
	pop r12
	pop r11
	pop r10
	pop r9
	pop r8
	pop rbp
	pop rdi
	pop rsi
	pop rdx
	pop rcx
	pop rbx
	pop rax
	ret


global syscall_inst_asm
extern syscall_inst
syscall_inst_asm:
	cli

	mov [registers.rax],rax
	mov [registers.rbx],rbx
	mov [registers.rcx],rcx
	mov [registers.rdx],rdx
	mov [registers.rsi],rsi
	mov [registers.rdi],rdi
	mov [registers.rbp],rbp
	mov [registers.r8] ,r8
	mov [registers.r9] ,r9
	mov [registers.r10],r10
	mov [registers.r11],r11
	mov [registers.r12],r12
	mov [registers.r13],r13
	mov [registers.r14],r14
	mov [registers.r15],r15	

	mov rcx,rsp
	mov rsp,stack_top_syscall
	mov rdi,registers
	call syscall_inst
	mov rsp,rcx


	mov rax,[registers.rax]
	mov rbx,[registers.rbx]
	mov rcx,[registers.rcx]
	mov rdx,[registers.rdx]
	mov rsi,[registers.rsi]
	mov rdi,[registers.rdi]
	mov rbp,[registers.rbp]
	mov r8 ,[registers.r8]
	mov r9 ,[registers.r9]
	mov r10,[registers.r10]
	mov r11,[registers.r11]
	mov r12,[registers.r12]
	mov r13,[registers.r13]
	mov r14,[registers.r14]
	mov r15,[registers.r15]	


	sti
	jmp USER_MODE_RETURN

global USER_MODE_RETURN
USER_MODE_RETURN:
	mov rcx,rdi
	mov r11,rsi
	o64 sysret

SECTION .bss
registers:
	.rax: resb 0x8
	.rbx: resb 0x8
	.rcx: resb 0x8
	.rdx: resb 0x8
	.rsi: resb 0x8
	.rdi: resb 0x8
	.rbp: resb 0x8
	.r8 : resb 0x8
	.r9 : resb 0x8
	.r10: resb 0x8
	.r11: resb 0x8
	.r12: resb 0x8
	.r13: resb 0x8
	.r14: resb 0x8
	.r15: resb 0x8
