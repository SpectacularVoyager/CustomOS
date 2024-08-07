[bits 32]
global LongMode_CheckCPUID
LongMode_CheckCPUID:
    pushfd                               ;Save EFLAGS
    pushfd                               ;Store EFLAGS
    xor dword [esp],0x00200000           ;Invert the ID bit in stored EFLAGS
    popfd                                ;Load stored EFLAGS (with ID bit inverted)
    pushfd                               ;Store EFLAGS again (ID bit may or may not be inverted)
    pop eax                              ;eax = modified EFLAGS (ID bit may or may not be inverted)
    xor eax,[esp]                        ;eax = whichever bits were changed
    popfd                                ;Restore original EFLAGS
    and eax,0x00200000                   ;eax = zero if ID bit can't be changed, else non-zero
    ret

global LongMode_GetVendor
LongMode_GetVendor:
	push ebp
	mov ebp,esp
	mov eax,0x0
	cpuid
	mov eax,[esp+8]
	mov dword [eax],ebx
	mov dword [eax+4],edx
	mov dword [eax+8],ecx
	pop ebp
	ret

global LongMode_GetFeatures
LongMode_GetFeatures:
	push ebp
	mov ebp,esp
	mov eax,0x1
	cpuid
	mov eax,[esp+8]
	mov dword [eax+0],edx
	mov dword [eax+4],ecx
	pop ebp
	ret

global LongMode_LongModeSupported
LongMode_LongModeSupported:
    mov eax, 0x80000000    ; Set the A-register to 0x80000000.
    cpuid                  ; CPU identification.
    cmp eax, 0x80000001    ; Compare the A-register with 0x80000001.
    jb .NoLongMode         ; It is less, there is no long mode.
    mov eax, 0x80000001    ; Set the A-register to 0x80000001.
    cpuid                  ; CPU identification.
    test edx, 1 << 29      ; Test if the LM-bit, which is bit 29, is set in the D-register.
    jz .NoLongMode         ; They aren't, there is no long mode.
	mov eax,1
	ret
	.NoLongMode:
	mov eax,0
	ret

global LongMode_A20STATUS
LongMode_A20STATUS:   

	pushad
	mov edi,0x112345  ;odd megabyte address.
	mov esi,0x012345  ;even megabyte address.
	mov [esi],esi     ;making sure that both addresses contain diffrent values.
	mov [edi],edi     ;(if A20 line is cleared the two pointers would point to the address 0x012345 that would contain 0x112345 (edi)) 
	cmpsd             ;compare addresses to see if the're equivalent.
	popad
	jne A20_on        ;if not equivalent , A20 line is set.
	mov eax,0
	ret               ;if equivalent , the A20 line is cleared.

A20_on:
	mov eax,1
	ret
global LongMode_EnableA20
LongMode_EnableA20:
        cli

        call    a20wait
        mov     al,0xAD
        out     0x64,al

        call    a20wait
        mov     al,0xD0
        out     0x64,al

        call    a20wait2
        in      al,0x60
        push    eax

        call    a20wait
        mov     al,0xD1
        out     0x64,al

        call    a20wait
        pop     eax
        or      al,2
        out     0x60,al

        call    a20wait
        mov     al,0xAE
        out     0x64,al

        call    a20wait
        sti
        ret

a20wait:
        in      al,0x64
        test    al,2
        jnz     a20wait
        ret


a20wait2:
        in      al,0x64
        test    al,1
        jz      a20wait2
        ret
