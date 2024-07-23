[bits 32]

;;void __attribute__((cdecl)) GDT_Load(GDTDescriptor* descriptor,uint16_t codeSegment,uint16_t dataSegment);
global GDT_Load
GDT_Load:
	push ebp
	mov ebp,esp
	
	; load gdt in eax
	mov eax,[ebp+8]
	lgdt [eax]
	; reload code segment
	mov eax,[ebp+12]
	push eax
	push .reload_cs
	retf

	.reload_cs:
	; reload data segment
	mov ax,[ebp+16]
	mov ds,ax
	mov es,ax
	mov fs,ax
	mov gs,ax
	mov ss,ax


	mov esp,ebp
	pop ebp
	ret
