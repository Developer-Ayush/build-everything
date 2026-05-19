#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "sha256.h"
#include "rudra512.h"

void print_hex(const char *label, const char *hex) {
    printf("%s: %s\n", label, hex);
}

int count_differing_bits(const char *hex1, const char *hex2) {
    int diff_bits = 0;
    size_t len1 = strlen(hex1);
    size_t len2 = strlen(hex2);
    size_t min_len = (len1 < len2) ? len1 : len2;

    for (size_t i = 0; i < min_len; ++i) {
        int v1, v2;
        sscanf(hex1 + i, "%1x", &v1);
        sscanf(hex2 + i, "%1x", &v2);
        int diff = v1 ^ v2;
        while (diff) {
            if (diff & 1) diff_bits++;
            diff >>= 1;
        }
    }
    // Add bits for length difference (each hex char is 4 bits)
    size_t diff_len = (len1 > len2) ? (len1 - len2) : (len2 - len1);
    diff_bits += diff_len * 4;

    return diff_bits;
}

int main() {
    char hex_out[129];

    printf("--- SHA-256 Demonstrations ---\n");
    sha256_hex((const uint8_t *)"", 0, hex_out);
    print_hex("SHA-256 (empty string)", hex_out);
    printf("Expected: e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855\n\n");

    sha256_hex((const uint8_t *)"abc", 3, hex_out);
    print_hex("SHA-256 (\"abc\")", hex_out);
    printf("Expected: ba7816bf8f01cfea414140de5dae2ec73b00361bbef0469348423f656b5c1d5\n\n");

    sha256_hex((const uint8_t *)"hello world", 11, hex_out);
    print_hex("SHA-256 (\"hello world\")", hex_out);
    printf("\n");

    printf("--- RUDRA-512 Demonstrations ---\n");
    rudra512_hex((const uint8_t *)"hello world", 11, 32, NULL, hex_out);
    print_hex("RUDRA-512 (\"hello world\", 32 rounds)", hex_out);
    printf("\n");

    printf("--- Avalanche Demo ---\n");
    char sha_hw[65], sha_hw_ex[65];
    sha256_hex((const uint8_t *)"hello world", 11, sha_hw);
    sha256_hex((const uint8_t *)"hello world!", 12, sha_hw_ex);
    printf("SHA-256 Avalanche: %d bits differ between \"hello world\" and \"hello world!\"\n",
           count_differing_bits(sha_hw, sha_hw_ex));

    char rudra_hw[129], rudra_hw_ex[129];
    rudra512_hex((const uint8_t *)"hello world", 11, 32, NULL, rudra_hw);
    rudra512_hex((const uint8_t *)"hello world!", 12, 32, NULL, rudra_hw_ex);
    printf("RUDRA-512 Avalanche: %d bits differ between \"hello world\" and \"hello world!\"\n",
           count_differing_bits(rudra_hw, rudra_hw_ex));
    printf("\n");

    printf("--- Performance Test (1MB zero buffer) ---\n");
    size_t size = 1024 * 1024;
    uint8_t *buffer = (uint8_t *)calloc(1, size);

    clock_t start = clock();
    sha256_hex(buffer, size, hex_out);
    clock_t end = clock();
    printf("SHA-256: %.2f ms\n", (double)(end - start) * 1000 / CLOCKS_PER_SEC);

    start = clock();
    rudra512_hex(buffer, size, 32, NULL, hex_out);
    end = clock();
    printf("RUDRA-512: %.2f ms\n", (double)(end - start) * 1000 / CLOCKS_PER_SEC);

    free(buffer);

    return 0;
}
