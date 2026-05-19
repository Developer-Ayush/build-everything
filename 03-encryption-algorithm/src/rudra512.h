#ifndef RUDRA512_H
#define RUDRA512_H

#include <stddef.h>
#include <stdint.h>

typedef struct {
    uint8_t *data;
    size_t len;
    size_t capacity;
    int rounds;
    char *salt;
} RUDRA512_CTX;

void rudra512_init_ctx(RUDRA512_CTX *ctx, int rounds, const char *salt);
void rudra512_update(RUDRA512_CTX *ctx, const uint8_t *data, size_t len);
void rudra512_final(RUDRA512_CTX *ctx, char out[129]);
void rudra512_hex(const uint8_t *data, size_t len, int rounds, const char *salt, char out[129]);

#endif
