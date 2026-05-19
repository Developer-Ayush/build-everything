#ifndef DISPLAY_H
#define DISPLAY_H

#include <stdint.h>
#include <stdbool.h>

#define DISPLAY_WIDTH 64
#define DISPLAY_HEIGHT 32

typedef struct {
    uint8_t pixels[DISPLAY_WIDTH * DISPLAY_HEIGHT];
} Display;

void display_init(Display *display);
void display_clear(Display *display);
bool display_draw_pixel(Display *display, uint8_t x, uint8_t y);
void display_render(const Display *display);

#endif
