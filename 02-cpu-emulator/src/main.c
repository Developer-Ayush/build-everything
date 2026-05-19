#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "chip8.h"
#include "../roms/test_rom.h"

void print_opcode_name(uint16_t opcode) {
    uint8_t x = (opcode & 0x0F00) >> 8;
    uint8_t y = (opcode & 0x00F0) >> 4;
    uint8_t kk = opcode & 0x00FF;
    uint8_t n = opcode & 0x000F;
    uint16_t nnn = opcode & 0x0FFF;

    switch (opcode & 0xF000) {
        case 0x0000:
            if (opcode == 0x00E0) printf("CLS");
            else if (opcode == 0x00EE) printf("RET");
            else printf("SYS 0x%03X", nnn);
            break;
        case 0x1000: printf("JP 0x%03X", nnn); break;
        case 0x2000: printf("CALL 0x%03X", nnn); break;
        case 0x3000: printf("SE V%X, 0x%02X", x, kk); break;
        case 0x4000: printf("SNE V%X, 0x%02X", x, kk); break;
        case 0x5000: printf("SE V%X, V%X", x, y); break;
        case 0x6000: printf("LD V%X, 0x%02X", x, kk); break;
        case 0x7000: printf("ADD V%X, 0x%02X", x, kk); break;
        case 0x8000:
            switch (opcode & 0x000F) {
                case 0x0: printf("LD V%X, V%X", x, y); break;
                case 0x1: printf("OR V%X, V%X", x, y); break;
                case 0x2: printf("AND V%X, V%X", x, y); break;
                case 0x3: printf("XOR V%X, V%X", x, y); break;
                case 0x4: printf("ADD V%X, V%X", x, y); break;
                case 0x5: printf("SUB V%X, V%X", x, y); break;
                case 0x6: printf("SHR V%X, V%X", x, y); break;
                case 0x7: printf("SUBN V%X, V%X", x, y); break;
                case 0xE: printf("SHL V%X, V%X", x, y); break;
            }
            break;
        case 0x9000: printf("SNE V%X, V%X", x, y); break;
        case 0xA000: printf("LD I, 0x%03X", nnn); break;
        case 0xB000: printf("JP V0, 0x%03X", nnn); break;
        case 0xC000: printf("RND V%X, 0x%02X", x, kk); break;
        case 0xD000: printf("DRW V%X, V%X, %d", x, y, n); break;
        case 0xE000:
            if (kk == 0x9E) printf("SKP V%X", x);
            else if (kk == 0xA1) printf("SKNP V%X", x);
            break;
        case 0xF000:
            switch (kk) {
                case 0x07: printf("LD V%X, DT", x); break;
                case 0x0A: printf("LD V%X, K", x); break;
                case 0x15: printf("LD DT, V%X", x); break;
                case 0x18: printf("LD ST, V%X", x); break;
                case 0x1E: printf("ADD I, V%X", x); break;
                case 0x29: printf("LD F, V%X", x); break;
                case 0x33: printf("LD B, V%X", x); break;
                case 0x55: printf("LD [I], V%X", x); break;
                case 0x65: printf("LD V%X, [I]", x); break;
            }
            break;
    }
}

int main() {
    CHIP8 chip8;
    chip8_init(&chip8);
    chip8_load_rom(&chip8, test_rom, test_rom_size);

    srand(time(NULL));

    int cycles = 0;
    const int max_cycles = 100;
    const int instructions_per_second = 500;
    const int timer_frequency = 60;
    const int instructions_per_timer_update = instructions_per_second / timer_frequency;

    while (cycles < max_cycles) {
        uint16_t pc = chip8.pc;
        uint16_t opcode = (chip8.memory[pc] << 8) | chip8.memory[pc + 1];

        printf("[0x%03X] 0x%04X  ", pc, opcode);
        print_opcode_name(opcode);
        printf("\n");

        chip8_step(&chip8);

        if ((opcode & 0xF000) == 0xD000) {
            display_render(&chip8.display);
        }

        cycles++;

        if (cycles % instructions_per_timer_update == 0) {
            chip8_update_timers(&chip8);
        }

        // Pace the emulation to 500Hz
        clock_t wait_until = clock() + (CLOCKS_PER_SEC / instructions_per_second);
        while (clock() < wait_until);
    }

    printf("\nFinal Register State:\n");
    for (int i = 0; i < 16; i++) {
        printf("V%X: 0x%02X  ", i, chip8.v[i]);
        if ((i + 1) % 4 == 0) printf("\n");
    }
    printf("I: 0x%03X  PC: 0x%03X  SP: 0x%X\n", chip8.i, chip8.pc, chip8.sp);

    return 0;
}
