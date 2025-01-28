[bits 64]
; Define variables in the data section
SECTION .DATA

; Code goes in the text section
SECTION .TEXT
	GLOBAL _start 

_start:

	mov rax,10
	mov rdi,10
	int 0x80
	; Terminate program
	mov rax,60            ; 'exit' system call
	mov rdi,1            ; 'exit' system call
	int 0x80              ; call the kernel
