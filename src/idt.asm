; void __attribute__((cdecl)) IDT_Load(IDTDescriptor* descriptor);
[bits 32]
global IDT_Load
IDT_Load:
	push ebp
	mov ebp,esp
	
	; load idt in eax
	mov eax,[ebp+8]
	lidt [eax]
	mov esp,ebp
	pop ebp
	ret
