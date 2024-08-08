bits 64
global IDT_Load
IDT_Load:
	lidt [rdi]
	ret
