# CHIP-8 CPU Emulator

## What is CHIP-8?
CHIP-8 is an interpreted programming language, first used on the COSMAC VIP and Telmac 1800 8-bit microcomputers in the mid-1970s. CHIP-8 programs are run on a CHIP-8 virtual machine. It was made to allow video games to be more easily programmed for these computers.

It is the perfect "first" CPU emulator because the instruction set is small (35 opcodes), the architecture is simple (no complex memory mapping or interrupts), and it provides immediate visual feedback.

## Fetch-Decode-Execute Cycle
The emulator follows the classic CPU cycle:

```
    +-----------------+
    |      Fetch      | <-- Read 2 bytes from memory at PC
    +-----------------+
            |
            v
    +-----------------+
    |     Decode      | <-- Parse opcode, X, Y, N, NN, NNN
    +-----------------+
            |
            v
    +-----------------+
    |     Execute     | <-- Perform the operation (update regs, memory, etc.)
    +-----------------+
            |
            v
    +-----------------+
    |    Update PC    | <-- Usually PC += 2 (unless jump/skip)
    +-----------------+
```

## Opcodes
1.  `00E0` - CLS: Clear the display.
2.  `00EE` - RET: Return from a subroutine.
3.  `1NNN` - JP addr: Jump to location NNN.
4.  `2NNN` - CALL addr: Call subroutine at NNN.
5.  `3XNN` - SE Vx, byte: Skip next instruction if Vx == NN.
6.  `4XNN` - SNE Vx, byte: Skip next instruction if Vx != NN.
7.  `5XY0` - SE Vx, Vy: Skip next instruction if Vx == Vy.
8.  `6XNN` - LD Vx, byte: Set Vx = NN.
9.  `7XNN` - ADD Vx, byte: Set Vx = Vx + NN.
10. `8XY0` - LD Vx, Vy: Set Vx = Vy.
11. `8XY1` - OR Vx, Vy: Set Vx = Vx OR Vy.
12. `8XY2` - AND Vx, Vy: Set Vx = Vx AND Vy.
13. `8XY3` - XOR Vx, Vy: Set Vx = Vx XOR Vy.
14. `8XY4` - ADD Vx, Vy: Set Vx = Vx + Vy, set VF = carry.
15. `8XY5` - SUB Vx, Vy: Set Vx = Vx - Vy, set VF = NOT borrow.
16. `8XY6` - SHR Vx {, Vy}: Set Vx = Vy >> 1, set VF = LSB. (COSMAC VIP quirk)
17. `8XY7` - SUBN Vx, Vy: Set Vx = Vy - Vx, set VF = NOT borrow.
18. `8XYE` - SHL Vx {, Vy}: Set Vx = Vy << 1, set VF = MSB. (COSMAC VIP quirk)
19. `9XY0` - SNE Vx, Vy: Skip next instruction if Vx != Vy.
20. `ANNN` - LD I, addr: Set I = NNN.
21. `BNNN` - JP V0, addr: Jump to location NNN + V0.
22. `CXNN` - RND Vx, byte: Set Vx = random byte AND NN.
23. `DXYN` - DRW Vx, Vy, nibble: Display n-byte sprite starting at memory location I at (Vx, Vy), set VF = collision.
24. `EX9E` - SKP Vx: Skip next instruction if key with the value of Vx is pressed. (Stubbed)
25. `EXA1` - SKNP Vx: Skip next instruction if key with the value of Vx is not pressed. (Stubbed)
26. `FX07` - LD Vx, DT: Set Vx = delay timer value.
27. `FX0A` - LD Vx, K: Wait for a key press, store the value of the key in Vx. (Stubbed)
28. `FX15` - LD DT, Vx: Set delay timer = Vx.
29. `FX18` - LD ST, Vx: Set sound timer = Vx.
30. `FX1E` - ADD I, Vx: Set I = I + Vx.
31. `FX29` - LD F, Vx: Set I = location of sprite for digit Vx.
32. `FX33` - LD B, Vx: Store BCD representation of Vx in memory locations I, I+1, and I+2.
33. `FX55` - LD [I], Vx: Store registers V0 through Vx in memory starting at location I. (COSMAC VIP quirk: increments I)
34. `FX65` - LD Vx, [I]: Read registers V0 through Vx from memory starting at location I. (COSMAC VIP quirk: increments I)
35. `0NNN` - SYS addr: Jump to a machine code routine at NNN. (Ignored)

## Compilation and Running

### Build and Run Emulator
```bash
gcc -o chip8 src/chip8.c src/display.c src/main.c
./chip8
```

### Build and Run Tests
```bash
gcc -o tests tests/test_chip8.c src/chip8.c src/display.c
./tests
```

## Lessons Learned
- **Binary Data Handling:** Managing memory as a byte array and interpreting it as 16-bit opcodes.
- **Instruction Decoding:** Using bitwise operators (`&`, `>>`) to extract meaning from raw bytes.
- **Hardware Abstraction:** How a virtual CPU interacts with memory, registers, and display buffers.
- **Legacy Quirks:** Understanding that "standard" behavior can vary between different implementations of the same architecture.
