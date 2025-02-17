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
; 	mov fs,ax
; 	mov gs,ax
;
;     ; Build a fake iret frame
; 	mov rax,0x20 | USER_PREV
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

	;;USERSPACE STACK
	;mov rsp,rdx
	o64 sysret

global restore_kernel_stack
restore_kernel_stack:
	mov rsp,stack_top_syscall
	ret


global syscall_inst_asm
extern syscall_inst
syscall_inst_asm:

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

	mov rdi,registers
	mov rsi,rsp

	mov rsp,stack_top_syscall
	call syscall_inst

	mov rsp,rsi


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

	mov rcx,0xC0000081
	rdmsr
	mov edx,(0x18<<16)|(0x8)
	wrmsr
	o64 sysret

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
