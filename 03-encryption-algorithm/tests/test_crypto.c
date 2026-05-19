#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include "sha256.h"
#include "rudra512.h"

#define ASSERT(cond, msg) \
    if (!(cond)) { \
        printf("[FAIL] %s\n", msg); \
        exit(1); \
    } else { \
        printf("[PASS] %s\n", msg); \
    }

int count_diff_bits(const char *h1, const char *h2) {
    int diff = 0;
    for (int i = 0; i < 128; i++) {
        int v1, v2;
        sscanf(h1 + i, "%1x", &v1);
        sscanf(h2 + i, "%1x", &v2);
        int d = v1 ^ v2;
        while (d) { if (d & 1) diff++; d >>= 1; }
    }
    return diff;
}

int main() {
    char out1[129], out2[129];
    uint8_t digest[32];

    // 1. SHA-256 empty string matches known vector
    sha256_hex((const uint8_t *)"", 0, out1);
    ASSERT(strcmp(out1, "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855") == 0, "SHA-256 empty string");

    // 2. SHA-256 "abc" matches known vector
    sha256_hex((const uint8_t *)"abc", 3, out1);
    ASSERT(strcmp(out1, "ba7816bf8f01cfea414140de5dae2ec73b00361bbef0469348423f656b5c1d5") == 0, "SHA-256 \"abc\"");

    // 3. SHA-256 "hello world" matches known vector
    sha256_hex((const uint8_t *)"hello world", 11, out1);
    ASSERT(strcmp(out1, "b94d27b9934d3e08a52e52d7da7dabfac484efe04c1045bc2f95f5a3c9cb4716") == 0, "SHA-256 \"hello world\"");

    // 4. SHA-256 same input twice -> same output
    sha256_hex((const uint8_t *)"test", 4, out1);
    sha256_hex((const uint8_t *)"test", 4, out2);
    ASSERT(strcmp(out1, out2) == 0, "SHA-256 determinism");

    // 5. SHA-256 different inputs -> different outputs
    sha256_hex((const uint8_t *)"test1", 5, out1);
    sha256_hex((const uint8_t *)"test2", 5, out2);
    ASSERT(strcmp(out1, out2) != 0, "SHA-256 distinctness");

    // 6. SHA-256 streaming split "hello world"
    SHA256_CTX ctx;
    sha256_init(&ctx);
    sha256_update(&ctx, (const uint8_t *)"hello", 5);
    sha256_update(&ctx, (const uint8_t *)" ", 1);
    sha256_update(&ctx, (const uint8_t *)"world", 5);
    sha256_final(&ctx, digest);
    char stream_out[65];
    for (int i = 0; i < 32; i++) sprintf(stream_out + i*2, "%02x", digest[i]);
    stream_out[64] = '\0';
    sha256_hex((const uint8_t *)"hello world", 11, out1);
    ASSERT(strcmp(stream_out, out1) == 0, "SHA-256 streaming");

    // 7. RUDRA-512 same input twice -> same output
    rudra512_hex((const uint8_t *)"test", 4, 32, NULL, out1);
    rudra512_hex((const uint8_t *)"test", 4, 32, NULL, out2);
    ASSERT(strcmp(out1, out2) == 0, "RUDRA-512 determinism");

    // 8. RUDRA-512 different inputs -> different outputs
    rudra512_hex((const uint8_t *)"test1", 5, 32, NULL, out1);
    rudra512_hex((const uint8_t *)"test2", 5, 32, NULL, out2);
    ASSERT(strcmp(out1, out2) != 0, "RUDRA-512 distinctness");

    // 9. RUDRA-512 avalanche: flip 1 bit in input, verify at least 200 of 512 bits change
    uint8_t in1[1] = {0x00};
    uint8_t in2[1] = {0x01};
    rudra512_hex(in1, 1, 32, NULL, out1);
    rudra512_hex(in2, 1, 32, NULL, out2);
    int diff = count_diff_bits(out1, out2);
    ASSERT(diff >= 200, "RUDRA-512 avalanche");

    // 10. RUDRA-512 output is exactly 128 hex chars
    rudra512_hex((const uint8_t *)"test", 4, 32, NULL, out1);
    ASSERT(strlen(out1) == 128, "RUDRA-512 length");

    printf("\nAll 10 tests passed!\n");
    return 0;
}
