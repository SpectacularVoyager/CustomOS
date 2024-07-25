global outb
outb:
    [bits 32]
    mov dx, [esp + 4]
    mov al, [esp + 8]
    out dx, al
    ret

global inb
inb:
    [bits 32]
    mov dx, [esp + 4]
    xor eax, eax
    in al, dx
    ret

global outportw
outportw:
    [bits 32]
    mov dx, [esp + 4]
    mov al, [esp + 8]
    out dx, ax
    ret

global inportw
inportw:
    [bits 32]
    mov dx, [esp + 4]
    xor eax, eax
    in ax, dx
    ret

global EnableInterrupts
EnableInterrupts:
	sti
    ret

global DisableInterrupts
DisableInterrupts:
	cli
    ret

global Panic
Panic:
    cli
    hlt

global crash_me
crash_me:
    ; div by 0
    ; mov ecx, 0x1337
    ; mov eax, 0
    ; div eax
    int 0x80
    ret
