	global long_mode_start

	section .text
	bits 64
long_mode_start:


	mov ax, gdt64.data
	mov ss, ax
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax

	extern kernel_main
	pop rax
	mov rcx,rax
	pop rax
	xor rbx,rbx
	mov ebx,eax
	shr rax,32
	mov rsi,rbx
	;add rdi,gdt64.code
	mov rdi,rax
	xor rax,rax
	mov ax,cs
	mov rdx,rax
	mov r8,gdt64
	call kernel_main
	hlt
