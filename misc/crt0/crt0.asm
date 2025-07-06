[bits 64]

extern main
SECTION .TEXT
    GLOBAL _start

_start:

	pop rdi
	pop rsi
	;SETING UP STACK FRAME WITH (RBP=0 AND RSP=0)
	mov rbp,0
	push rbp
	push rbp
	mov rbp,rsp


	push rsi
	push rdi
	call main
	mov rdi,rax

	mov rax,60
	int 0x80	;	EXIT(EAX);


