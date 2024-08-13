section .bss
align 4096
p4_table:
  resb 4096
p3_table:
  resb 4096
p2_table:
  resb 4096

section .rodata
gdt64:
    dq 0 ; zero entry
.code: equ $ - gdt64 ; new
    ;dq (1<<43) | (1<<44) | (1<<47) | (1<<53) ; code segment
	dq (0xA)<<52 | (0x9A)<<40
.data: equ $ - gdt64 ; new
    ;dq (1<<43) | (1<<44) | (1<<47) | (1<<53) ; code segment
	dq (0xC)<<52 | (0x92)<<40
.pointer:
  dw $ - gdt64 - 1
  dq gdt64

[bits 32]
global Paging_Disable
Paging_Disable:
    mov eax, cr0                                   ; Set the A-register to control register 0.
    and eax, 01111111111111111111111111111111b     ; Clear the PG-bit, which is bit 31.
    mov cr0, eax                                   ; Set control register 0 to the A-register.
	ret


global Paging_Enable
Paging_Enable:
  ; load P4 to cr3 register (cpu uses this to access the P4 table)
  mov eax, p4_table

  mov cr3, eax

  ; enable PAE-flag in cr4 (Physical Address Extension)
  mov eax, cr4
  or eax, 1 << 5
  mov cr4, eax

  ; set the long mode bit in the EFER MSR (model specific register)
  mov ecx, 0xC0000080
  rdmsr
  or eax, 1 << 8
  wrmsr

  ; enable paging in the cr0 register
  mov eax, cr0
  or eax, 1 << 31
  mov cr0, eax

  ret
Paging_SetUpTables:
  ; map first P4 entry to P3 table
  mov eax, p3_table
  or eax, 0b11 ; present + writable
  mov [p4_table], eax

  ; map first P3 entry to P2 table
  mov eax, p2_table
  or eax, 0b11 ; present + writable
  mov [p3_table], eax

  mov ecx, 0

; map ecx-th P2 entry to a huge page that starts at address 2MiB*ecx
.map_p2_table:
  mov eax, 0x200000  ; 2MiB
  mul ecx            ; start address of ecx-th page
  or eax, 0b10000011 ; present + writable + huge
  mov [p2_table + ecx * 8], eax ; map ecx-th entry

  inc ecx            ; increase counter
  cmp ecx, 512       ; if counter == 512, the whole P2 table is mapped
  jne .map_p2_table  ; else map the next entry

  ret
