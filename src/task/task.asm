
[bits 64]
global TaskGetContext
TaskGetContext:
	push rbp
	mov rbp,rsp
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
	push rsp
	mov rax,[rbp-8]
	;mov rax,100
	push rax
	pushf
	mov rax,cr3
	push rax

	mov rax,rsp	;;PASS FIRST ARGUMENT
	mov rsp,rbp
	pop rbp
	ret

global TaskSetContext
TaskSetContext:
	push rbp
	mov rbp,rsp
	mov rax,[rdi+0x00]
	mov rbx,[rdi+0x08]
	mov rcx,[rdi+0x10]
	;mov rdx,[rdi+0x18]
	mov rsi,[rdi+0x20]
	mov rdi,[rdi+0x28]
	mov rbp,[rdi+0x30]
	mov r8 ,[rdi+0x38]
	mov r9 ,[rdi+0x40]
	mov r10,[rdi+0x48]
	mov r11,[rdi+0x50]
	mov r12,[rdi+0x58]
	mov r13,[rdi+0x60]
	mov r14,[rdi+0x68]
	mov r15,[rdi+0x70]
	mov rsp,[rdi+0x78]
	;mov rax,[rdi+0x80]
	;mov [rbp-0x8],rax
	;mov rax,[rdi+0x88]
	;popf
	;;TODO: FLAGS AND RIP
	mov rax,[rdi+0x90]
	mov cr3,rax
	mov rax,[rdi]
	mov rdi,[rdi+0x28]
	pop rbp
	ret
