#include "kernel.h"

void vga_print(const char* str, int row, int col, unsigned char color) {
    volatile char* vga_buffer = (volatile char*)0xB8000;
    int offset = (row * 80 + col) * 2;

    while (*str) {
        vga_buffer[offset++] = *str++;
        vga_buffer[offset++] = color;
    }
}

void kernel_main(void) {
    // Clear screen (optional but good practice, here we just print as requested)
    // The requirement says print exact text to these lines.

    unsigned char color = 0x0F; // White on black

    vga_print("build-everything | Project 04", 0, 0, color);
    vga_print("OS Kernel - Protected Mode Active", 1, 0, color);
    vga_print("Phase 2 of 25 | by Ayush Anand", 2, 0, color);
    vga_print("", 3, 0, color);
    vga_print("Memory map:", 4, 0, color);
    vga_print("  0x00007C00 - Bootloader (MBR)", 5, 0, color);
    vga_print("  0x00010000 - Kernel Entry", 6, 0, color);
    vga_print("  0x000B8000 - VGA Text Buffer", 7, 0, color);
    vga_print("", 8, 0, color);
    vga_print("Status: Protected Mode [OK]", 9, 0, color);
    vga_print("Status: VGA Output    [OK]", 10, 0, color);
    vga_print("Status: Stack Setup   [OK]", 11, 0, color);

    while (1) {
        __asm__ volatile("hlt");
    }
}
