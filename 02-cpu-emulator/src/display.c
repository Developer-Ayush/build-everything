#include "display.h"
#include <stdio.h>
#include <string.h>

void display_init(Display *display) {
    display_clear(display);
}

void display_clear(Display *display) {
    memset(display->pixels, 0, sizeof(display->pixels));
}

bool display_draw_pixel(Display *display, uint8_t x, uint8_t y) {
    // Wrap around coordinates
    x %= DISPLAY_WIDTH;
    y %= DISPLAY_HEIGHT;

    uint32_t index = x + (y * DISPLAY_WIDTH);
    bool flipped_off = (display->pixels[index] == 1);

    // XOR the pixel
    display->pixels[index] ^= 1;

    return flipped_off && (display->pixels[index] == 0);
}

void display_render(const Display *display) {
    // Clear screen and reset cursor to top-left
    printf("\033[2J\033[H");

    for (int y = 0; y < DISPLAY_HEIGHT; y++) {
        for (int x = 0; x < DISPLAY_WIDTH; x++) {
            if (display->pixels[x + (y * DISPLAY_WIDTH)]) {
                putchar('#');
            } else {
                putchar(' ');
            }
        }
        putchar('\n');
    }
}
