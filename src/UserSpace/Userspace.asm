[bits 64]

global __SYSRET
__SYSRET:
	mov rdi,rcx
	mov rsi,r11
	o64 sysret
