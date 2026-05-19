#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "../src/chip8.h"

void test_init() {
    CHIP8 chip8;
    chip8_init(&chip8);

    // RAM is zeroed (mostly)
    // Actually font is at 0x050, so check around it
    for (int i = 0; i < 0x050; i++) assert(chip8.memory[i] == 0);

    // Font loaded at 0x050
    assert(chip8.memory[0x050] == 0xF0); // Start of '0'
    assert(chip8.memory[0x050 + 4] == 0xF0); // End of '0'

    // PC starts at 0x200
    assert(chip8.pc == 0x200);

    printf("test_init passed\n");
}

void test_6xnn() {
    CHIP8 chip8;
    chip8_init(&chip8);
    chip8.memory[0x200] = 0x61; // LD V1, 0xAB
    chip8.memory[0x201] = 0xAB;
    chip8_step(&chip8);
    assert(chip8.v[1] == 0xAB);
    printf("test_6xnn passed\n");
}

void test_7xnn() {
    CHIP8 chip8;
    chip8_init(&chip8);
    chip8.v[2] = 0x10;
    chip8.memory[0x200] = 0x72; // ADD V2, 0x05
    chip8.memory[0x201] = 0x05;
    chip8_step(&chip8);
    assert(chip8.v[2] == 0x15);
    printf("test_7xnn passed\n");
}

void test_8xy4() {
    CHIP8 chip8;
    chip8_init(&chip8);

    // No carry
    chip8.v[0] = 0x10;
    chip8.v[1] = 0x20;
    chip8.memory[0x200] = 0x80; // ADD V0, V1
    chip8.memory[0x201] = 0x14;
    chip8_step(&chip8);
    assert(chip8.v[0] == 0x30);
    assert(chip8.v[0xF] == 0);

    // Carry
    chip8.pc = 0x200;
    chip8.v[0] = 0xFF;
    chip8.v[1] = 0x01;
    chip8_step(&chip8);
    assert(chip8.v[0] == 0x00);
    assert(chip8.v[0xF] == 1);

    printf("test_8xy4 passed\n");
}

void test_00e0() {
    CHIP8 chip8;
    chip8_init(&chip8);
    memset(chip8.display.pixels, 1, sizeof(chip8.display.pixels));
    chip8.memory[0x200] = 0x00; // CLS
    chip8.memory[0x201] = 0xE0;
    chip8_step(&chip8);
    for (int i = 0; i < 64 * 32; i++) assert(chip8.display.pixels[i] == 0);
    printf("test_00e0 passed\n");
}

void test_1nnn() {
    CHIP8 chip8;
    chip8_init(&chip8);
    chip8.memory[0x200] = 0x15; // JP 0x555
    chip8.memory[0x201] = 0x55;
    chip8_step(&chip8);
    assert(chip8.pc == 0x555);
    printf("test_1nnn passed\n");
}

void test_2nnn_00ee() {
    CHIP8 chip8;
    chip8_init(&chip8);
    chip8.memory[0x200] = 0x23; // CALL 0x300
    chip8.memory[0x201] = 0x00;
    chip8.memory[0x300] = 0x00; // RET
    chip8.memory[0x301] = 0xEE;

    chip8_step(&chip8);
    assert(chip8.pc == 0x300);
    assert(chip8.sp == 1);
    assert(chip8.stack[0] == 0x202);

    chip8_step(&chip8);
    assert(chip8.pc == 0x202);
    assert(chip8.sp == 0);

    printf("test_2nnn_00ee passed\n");
}

void test_3xnn() {
    CHIP8 chip8;
    chip8_init(&chip8);
    chip8.v[3] = 0x42;

    // Equal
    chip8.memory[0x200] = 0x33; // SE V3, 0x42
    chip8.memory[0x201] = 0x42;
    chip8_step(&chip8);
    assert(chip8.pc == 0x204);

    // Not Equal
    chip8.pc = 0x200;
    chip8.memory[0x200] = 0x33; // SE V3, 0x43
    chip8.memory[0x201] = 0x43;
    chip8_step(&chip8);
    assert(chip8.pc == 0x202);

    printf("test_3xnn passed\n");
}

void test_stack() {
    CHIP8 chip8;
    chip8_init(&chip8);

    // Test multiple calls
    chip8.memory[0x200] = 0x23; // CALL 0x300
    chip8.memory[0x201] = 0x00;
    chip8.memory[0x300] = 0x24; // CALL 0x400
    chip8.memory[0x301] = 0x00;

    chip8_step(&chip8);
    assert(chip8.sp == 1);
    chip8_step(&chip8);
    assert(chip8.sp == 2);
    assert(chip8.stack[0] == 0x202);
    assert(chip8.stack[1] == 0x302);

    printf("test_stack passed\n");
}

int main() {
    test_init();
    test_6xnn();
    test_7xnn();
    test_8xy4();
    test_00e0();
    test_1nnn();
    test_2nnn_00ee();
    test_3xnn();
    test_stack();
    printf("All tests passed!\n");
    return 0;
}
