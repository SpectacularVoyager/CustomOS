global long_mode_start

section .text
bits 64
long_mode_start:
  mov ax, 0
  mov ss, ax
  mov ds, ax
  mov es, ax
  mov fs, ax
  mov gs, ax

  extern kernel_main
	pop rax
	xor rbx,rbx
	mov ebx,eax
	shr rax,32
	mov rsi,rbx
	;add rdi,gdt64.code
	mov rdi,rax
  call kernel_main
  hlt
