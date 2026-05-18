#include "allocator.h"
#include <stdio.h>

void print_heap_stats() {
    my_malloc_stats();
}

int main() {
    printf("Starting Memory Allocator Demo...\n\n");

    printf("1. Testing my_malloc and my_free:\n");
    int* arr = (int*)my_malloc(10 * sizeof(int));
    if (arr) {
        printf("  Allocated 10 ints at %p\n", (void*)arr);
        for (int i = 0; i < 10; i++) arr[i] = i;
        printf("  Values: ");
        for (int i = 0; i < 10; i++) printf("%d ", arr[i]);
        printf("\n");
        print_heap_stats();
        my_free(arr);
        printf("  Freed 10 ints\n");
        print_heap_stats();
    }

    printf("\n2. Testing my_calloc:\n");
    double* d_arr = (double*)my_calloc(5, sizeof(double));
    if (d_arr) {
        printf("  Allocated 5 doubles with calloc at %p\n", (void*)d_arr);
        printf("  Values (should be 0.0): ");
        for (int i = 0; i < 5; i++) printf("%.1f ", d_arr[i]);
        printf("\n");
        my_free(d_arr);
        printf("  Freed 5 doubles\n");
    }

    printf("\n3. Testing my_realloc (expanding):\n");
    char* str = (char*)my_malloc(10);
    if (str) {
        snprintf(str, 10, "Hello");
        printf("  Original string: %s (at %p)\n", str, (void*)str);
        str = (char*)my_realloc(str, 20);
        snprintf(str + 5, 15, " World!");
        printf("  Expanded string: %s (at %p)\n", str, (void*)str);
        my_free(str);
    }

    printf("\n4. Testing Block Splitting:\n");
    void* p1 = my_malloc(512);
    printf("  Allocated 512 bytes at %p\n", p1);
    my_free(p1);
    printf("  Freed 512 bytes\n");
    void* p2 = my_malloc(128);
    printf("  Allocated 128 bytes (should reuse part of the 512 block) at %p\n", p2);
    void* p3 = my_malloc(128);
    printf("  Allocated another 128 bytes (should reuse another part) at %p\n", p3);
    print_heap_stats();
    my_free(p2);
    my_free(p3);

    printf("\n5. Testing Block Coalescing:\n");
    void* b1 = my_malloc(100);
    void* b2 = my_malloc(100);
    void* b3 = my_malloc(100);
    printf("  Allocated 3 blocks of 100 bytes: %p, %p, %p\n", b1, b2, b3);
    my_free(b1);
    my_free(b2); // Should coalesce b1 and b2
    printf("  Freed first two blocks. Current stats:\n");
    print_heap_stats();
    void* b4 = my_malloc(200);
    printf("  Allocated 200 bytes (should reuse coalesced b1+b2) at %p\n", b4);
    my_free(b3);
    my_free(b4);

    printf("\n6. Testing Double-Free Guard:\n");
    void* df = my_malloc(50);
    my_free(df);
    my_free(df); // Should print error

    printf("\n7. Testing Edge Cases:\n");
    printf("  my_malloc(0) returned: %p\n", my_malloc(0));
    void* p_null = NULL;
    printf("  my_free(NULL) - calling now...\n");
    my_free(p_null);
    printf("  my_realloc(NULL, 64) returned: %p\n", my_realloc(NULL, 64));

    printf("\nDemo Complete.\n");
    return 0;
}
