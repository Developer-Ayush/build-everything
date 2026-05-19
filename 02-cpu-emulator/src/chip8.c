#include "chip8.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

static uint8_t fontset[80] = {
    0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
    0x20, 0x60, 0x20, 0x20, 0x70, // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
    0x90, 0x90, 0xF0, 0x10, 0x10, // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
    0xF0, 0x10, 0x20, 0x40, 0x40, // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90, // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
    0xF0, 0x80, 0x80, 0x80, 0xF0, // C
    0xE0, 0x90, 0x90, 0x90, 0xE0, // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
    0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

void chip8_init(CHIP8 *chip8) {
    memset(chip8, 0, sizeof(CHIP8));
    chip8->pc = START_ADDRESS;
    memcpy(&chip8->memory[FONT_START_ADDRESS], fontset, sizeof(fontset));
    display_init(&chip8->display);
}

void chip8_load_rom(CHIP8 *chip8, const uint8_t *rom, size_t size) {
    if (size > (MEMORY_SIZE - START_ADDRESS)) {
        size = MEMORY_SIZE - START_ADDRESS;
    }
    memcpy(&chip8->memory[START_ADDRESS], rom, size);
}

void chip8_update_timers(CHIP8 *chip8) {
    if (chip8->delay_timer > 0) chip8->delay_timer--;
    if (chip8->sound_timer > 0) chip8->sound_timer--;
}

void chip8_step(CHIP8 *chip8) {
    uint16_t opcode = (chip8->memory[chip8->pc] << 8) | chip8->memory[chip8->pc + 1];
    uint16_t nnn = opcode & 0x0FFF;
    uint8_t n = opcode & 0x000F;
    uint8_t x = (opcode & 0x0F00) >> 8;
    uint8_t y = (opcode & 0x00F0) >> 4;
    uint8_t kk = opcode & 0x00FF;

    // Default: increment PC by 2
    chip8->pc += 2;

    switch (opcode & 0xF000) {
        case 0x0000:
            switch (opcode) {
                case 0x00E0: // CLS
                    display_clear(&chip8->display);
                    break;
                case 0x00EE: // RET
                    chip8->pc = chip8->stack[--chip8->sp];
                    break;
                default:
                    // 0NNN is usually ignored or used for system calls on some machines
                    break;
            }
            break;
        case 0x1000: // JP addr
            chip8->pc = nnn;
            break;
        case 0x2000: // CALL addr
            chip8->stack[chip8->sp++] = chip8->pc;
            chip8->pc = nnn;
            break;
        case 0x3000: // SE Vx, byte
            if (chip8->v[x] == kk) chip8->pc += 2;
            break;
        case 0x4000: // SNE Vx, byte
            if (chip8->v[x] != kk) chip8->pc += 2;
            break;
        case 0x5000: // SE Vx, Vy
            if (chip8->v[x] == chip8->v[y]) chip8->pc += 2;
            break;
        case 0x6000: // LD Vx, byte
            chip8->v[x] = kk;
            break;
        case 0x7000: // ADD Vx, byte
            chip8->v[x] += kk;
            break;
        case 0x8000:
            switch (opcode & 0x000F) {
                case 0x0: // LD Vx, Vy
                    chip8->v[x] = chip8->v[y];
                    break;
                case 0x1: // OR Vx, Vy
                    chip8->v[x] |= chip8->v[y];
                    break;
                case 0x2: // AND Vx, Vy
                    chip8->v[x] &= chip8->v[y];
                    break;
                case 0x3: // XOR Vx, Vy
                    chip8->v[x] ^= chip8->v[y];
                    break;
                case 0x4: // ADD Vx, Vy
                    {
                        uint16_t sum = chip8->v[x] + chip8->v[y];
                        chip8->v[0xF] = (sum > 0xFF) ? 1 : 0;
                        chip8->v[x] = sum & 0xFF;
                    }
                    break;
                case 0x5: // SUB Vx, Vy
                    chip8->v[0xF] = (chip8->v[x] >= chip8->v[y]) ? 1 : 0;
                    chip8->v[x] -= chip8->v[y];
                    break;
                case 0x6: // SHR Vx {, Vy}
                    // COSMAC VIP behavior: shift VY and store in VX
                    chip8->v[0xF] = chip8->v[y] & 0x1;
                    chip8->v[x] = chip8->v[y] >> 1;
                    break;
                case 0x7: // SUBN Vx, Vy
                    chip8->v[0xF] = (chip8->v[y] >= chip8->v[x]) ? 1 : 0;
                    chip8->v[x] = chip8->v[y] - chip8->v[x];
                    break;
                case 0xE: // SHL Vx {, Vy}
                    // COSMAC VIP behavior: shift VY and store in VX
                    chip8->v[0xF] = (chip8->v[y] & 0x80) >> 7;
                    chip8->v[x] = chip8->v[y] << 1;
                    break;
            }
            break;
        case 0x9000: // SNE Vx, Vy
            if (chip8->v[x] != chip8->v[y]) chip8->pc += 2;
            break;
        case 0xA000: // LD I, addr
            chip8->i = nnn;
            break;
        case 0xB000: // JP V0, addr
            chip8->pc = nnn + chip8->v[0];
            break;
        case 0xC000: // RND Vx, byte
            chip8->v[x] = (rand() % 256) & kk;
            break;
        case 0xD000: // DRW Vx, Vy, nibble
            {
                uint8_t vx = chip8->v[x];
                uint8_t vy = chip8->v[y];
                chip8->v[0xF] = 0;
                for (int row = 0; row < n; row++) {
                    uint8_t sprite_byte = chip8->memory[chip8->i + row];
                    for (int col = 0; col < 8; col++) {
                        if (sprite_byte & (0x80 >> col)) {
                            if (display_draw_pixel(&chip8->display, vx + col, vy + row)) {
                                chip8->v[0xF] = 1;
                            }
                        }
                    }
                }
            }
            break;
        case 0xE000:
            switch (kk) {
                case 0x9E: // SKP Vx
                    // Stubbed: never skip
                    break;
                case 0xA1: // SKNP Vx
                    // Stubbed: always skip
                    chip8->pc += 2;
                    break;
            }
            break;
        case 0xF000:
            switch (kk) {
                case 0x07: // LD Vx, DT
                    chip8->v[x] = chip8->delay_timer;
                    break;
                case 0x0A: // LD Vx, K
                    // Stubbed: store 0 and continue
                    chip8->v[x] = 0;
                    break;
                case 0x15: // LD DT, Vx
                    chip8->delay_timer = chip8->v[x];
                    break;
                case 0x18: // LD ST, Vx
                    chip8->sound_timer = chip8->v[x];
                    break;
                case 0x1E: // ADD I, Vx
                    chip8->i += chip8->v[x];
                    break;
                case 0x29: // LD F, Vx
                    chip8->i = FONT_START_ADDRESS + (chip8->v[x] * 5);
                    break;
                case 0x33: // LD B, Vx
                    chip8->memory[chip8->i] = chip8->v[x] / 100;
                    chip8->memory[chip8->i + 1] = (chip8->v[x] / 10) % 10;
                    chip8->memory[chip8->i + 2] = chip8->v[x] % 10;
                    break;
                case 0x55: // LD [I], Vx
                    for (int j = 0; j <= x; j++) {
                        chip8->memory[chip8->i + j] = chip8->v[j];
                    }
                    // COSMAC VIP: I = I + X + 1
                    chip8->i += x + 1;
                    break;
                case 0x65: // LD Vx, [I]
                    for (int j = 0; j <= x; j++) {
                        chip8->v[j] = chip8->memory[chip8->i + j];
                    }
                    // COSMAC VIP: I = I + X + 1
                    chip8->i += x + 1;
                    break;
            }
            break;
    }
}
