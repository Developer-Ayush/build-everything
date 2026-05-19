#ifndef CHIP8_H
#define CHIP8_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "display.h"

#define MEMORY_SIZE 4096
#define NUM_REGISTERS 16
#define STACK_DEPTH 16
#define START_ADDRESS 0x200
#define FONT_START_ADDRESS 0x050

typedef struct {
    uint8_t memory[MEMORY_SIZE];
    uint8_t v[NUM_REGISTERS];
    uint16_t i;
    uint16_t pc;
    uint8_t sp;
    uint16_t stack[STACK_DEPTH];
    uint8_t delay_timer;
    uint8_t sound_timer;
    Display display;
} CHIP8;

void chip8_init(CHIP8 *chip8);
void chip8_load_rom(CHIP8 *chip8, const uint8_t *rom, size_t size);
void chip8_step(CHIP8 *chip8);
void chip8_update_timers(CHIP8 *chip8);

#endif
