[bits 16]
[extern kernel_main]
global _start

_start:
    cli

    ; Setup DS to 0 to avoid relocation issues if possible,
    ; but we know we are at 0x10000.
    xor ax, ax
    mov ds, ax

    ; We need to load GDT with a physical address.
    ; gdt_descriptor_rel is at some offset from 0x10000.
    ; But since it's an ELF, the linker thinks it's at 0x10000 + offset.
    ; 16-bit instructions expect 16-bit addresses.

    ; Let's use a register to hold the address
    mov ebx, gdt_descriptor_rel
    lgdt [ebx]

    mov eax, cr0
    or eax, 0x1
    mov cr0, eax

    ; Use a 32-bit jump to reach the address
    jmp dword 0x08:init_pm

[bits 32]
init_pm:
    mov ax, 0x10
    mov ds, ax
    mov ss, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    mov esp, 0x90000
    call kernel_main
    hlt
    jmp $

align 8
gdt_start:
    dq 0x0
gdt_code:
    dw 0xffff
    dw 0x0
    db 0x0
    db 10011010b
    db 11001111b
    db 0x0
gdt_data:
    dw 0xffff
    dw 0x0
    db 0x0
    db 10010010b
    db 11001111b
    db 0x0
gdt_end:

gdt_descriptor_rel:
    dw gdt_end - gdt_start - 1
    dd gdt_start
