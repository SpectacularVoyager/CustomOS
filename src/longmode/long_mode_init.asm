global long_mode_start

section .text
bits 64
long_mode_start:
  mov ax, 0
  mov ss, ax
  mov ds, ax
  mov es, ax
  mov fs, ax
  mov gs, ax

  extern kernel_main
  mov byte [0xb8000],'a'
  mov byte [0xb8002],'b'
  call kernel_main
  hlt
