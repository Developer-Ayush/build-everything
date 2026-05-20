# Project 04: x86 OS Kernel (Assembly & C)

## Overview
This project implements a basic bootable x86 OS kernel. It consists of a Stage 1 bootloader (MBR) written in 16-bit assembly, a Stage 2 kernel entry point that transitions the CPU into 32-bit protected mode, and a Stage 3 kernel written in C that interacts with the VGA text buffer.

## Memory Map
| Address | Description |
|---------|-------------|
| 0x00007C00 | Bootloader (MBR) |
| 0x00009000 | Real Mode Stack Top |
| 0x00010000 | Kernel Entry & Kernel Code |
| 0x00090000 | Protected Mode Stack Top |
| 0x000B8000 | VGA Text Buffer |

## How the Bootloader Works
The BIOS loads the first sector of the bootable disk (the MBR) into memory at physical address `0x7C00`.
- **Real Mode:** The CPU starts in 16-bit real mode.
- **Segment Setup:** `DS`, `ES`, and `SS` are initialized to 0.
- **INT 0x10:** Used for BIOS teletype output to print "Booting...".
- **INT 0x13:** Used to read 15 sectors from the disk (starting at Sector 2) into memory at `0x1000:0x0000` (physical `0x10000`).
- **Handoff:** The bootloader performs a far jump to the loaded kernel code.

## Protected Mode Switch
The `kernel_entry.asm` handles the transition from 16-bit real mode to 32-bit protected mode:
1. **CLI:** Disable interrupts.
2. **GDT:** Load a Global Descriptor Table with three entries:
   - Null Descriptor
   - Code Segment (Base 0, Limit 0xFFFFF, 32-bit, Ring 0)
   - Data Segment (Base 0, Limit 0xFFFFF, 32-bit, Ring 0)
3. **CR0:** Set the Protection Enable (PE) bit in the `CR0` register.
4. **Far Jump:** A far jump to the 32-bit code segment flushes the instruction pipeline.
5. **Registers:** Initialize 32-bit segment registers (`DS`, `ES`, `FS`, `GS`, `SS`) and set `ESP` to `0x90000`.

## VGA Text Mode
The kernel writes directly to the VGA text buffer at `0xB8000`.
- Each character on the 80x25 screen occupies 2 bytes:
  - Byte 0: ASCII character code.
  - Byte 1: Attribute byte (e.g., `0x0F` for white text on a black background).
- The `vga_print` helper calculates the memory offset based on the requested row and column.

## Connection to Previous Projects
- **Project 01 (Memory Allocator):** Concepts of heap layout and pointer arithmetic informed the design of the memory map (0x7C00, 0x10000, 0x90000) and how we manually manage memory locations for the stack and kernel.
- **Project 02 (CHIP-8 Emulator):** The fetch-decode-execute cycle of the CHIP-8 maps directly to the x86 instruction pipeline. Concepts like the stack pointer (`SP`), `CALL`, and `RET` instructions in CHIP-8 are directly analogous to `ESP`, `PUSH`, and `POP` in the x86 environment.
- **Project 03 (Encryption Algorithm):** The SHA-256 implementation will be integrated into future stages (Project 07) once a full C runtime is available.

## How to Build and Run
### Build
```bash
make
```

### Run (Headless/Serial)
```bash
qemu-system-i386 -drive format=raw,file=os.img -nographic
```

### Run (With Display)
```bash
qemu-system-i386 -drive format=raw,file=os.img
```

## What was learned
- Low-level x86 architecture and the boot process.
- Transitioning from real mode to protected mode.
- Writing to memory-mapped hardware (VGA buffer).
- Linker scripts and manual memory layout.
- Managing stacks and segments across different CPU modes.
