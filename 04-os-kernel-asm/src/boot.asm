[org 0x7c00]

KERNEL_OFFSET equ 0x1000

start:
    ; Set up segment registers
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x9000

    ; Print "Booting..."
    mov si, msg_booting
    call print_string

    ; Load kernel from disk
    mov ax, KERNEL_OFFSET
    mov es, ax              ; ES = 0x1000
    mov bx, 0x0000          ; BX = 0x0000 -> Buffer at 0x1000:0x0000

    mov ah, 0x02            ; BIOS read sector
    mov al, 15              ; Read 15 sectors
    mov ch, 0x00            ; Cylinder 0
    mov dh, 0x00            ; Head 0
    mov cl, 0x02            ; Sector 2 (Sector 1 is MBR)
    ; DL is set by BIOS to the boot drive index
    int 0x13

    jc disk_error           ; Jump if carry flag set (error)

    ; Jump to kernel
    jmp KERNEL_OFFSET:0x0000

disk_error:
    mov si, msg_disk_error
    call print_string
    jmp $

print_string:
    mov ah, 0x0e            ; BIOS teletype output
.loop:
    lodsb                   ; Load byte from SI into AL, increment SI
    or al, al               ; Check if AL is 0 (null terminator)
    jz .done
    int 0x10                ; Print character in AL
    jmp .loop
.done:
    ret

msg_booting db "Booting...", 13, 10, 0
msg_disk_error db "Disk read error!", 0

; Padding and boot signature
times 510-($-$$) db 0
dw 0xaa55
