.text
global loadPageDirectory
loadPageDirectory:
	push ebp
	mov ebp,esp

	;; SET PAE BIT
	mov eax,cr4
	bts eax,5
	mov cr4,eax

	mov eax,[esp+8]
	mov cr3,eax
	mov esp,ebp
	pop ebp
	ret

global enablePaging
enablePaging:
	push ebp
	mov ebp,esp

	mov eax,cr0
	or eax,0x080000000
	mov cr0,eax

	mov esp,ebp
	pop ebp
	ret
