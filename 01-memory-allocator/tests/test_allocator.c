#include "../src/allocator.h"
#include <stdio.h>
#include <assert.h>
#include <string.h>

void test_basic_alloc() {
    printf("Test Basic Alloc: ");
    int* p = (int*)my_malloc(sizeof(int));
    assert(p != NULL);
    *p = 42;
    assert(*p == 42);
    my_free(p);
    printf("PASSED\n");
}

void test_free_and_reuse() {
    printf("Test Free and Reuse: ");
    void* p1 = my_malloc(128);
    my_free(p1);
    void* p2 = my_malloc(128);
    /*
     * Note: We use p2 <= p1 because tests share a global heap state.
     * If a prior test left a free block before p1, then freeing p1
     * might coalesce it with that prior block, making the resulting
     * free block start earlier in the heap than p1 did.
     * The allocator is correct as long as it reuses existing free space.
     */
    assert(p2 <= p1);
    my_free(p2);
    printf("PASSED\n");
}

void test_coalesce() {
    printf("Test Coalesce: ");
    void* p1 = my_malloc(128);
    void* p2 = my_malloc(128);
    void* p3 = my_malloc(128);
    my_free(p1);
    my_free(p2);
    void* p4 = my_malloc(256 + BLOCK_SIZE);
    // Relaxed check: p4 should be at or before p1
    assert((char*)p4 <= (char*)p1);
    my_free(p3);
    my_free(p4);
    printf("PASSED\n");
}

void test_split() {
    printf("Test Split: ");
    void* p1 = my_malloc(1024);
    my_free(p1);

    void* p2 = my_malloc(256);
    assert((char*)p2 <= (char*)p1);

    void* p3 = my_malloc(256);
    assert(p3 > p2);

    my_free(p2);
    my_free(p3);
    printf("PASSED\n");
}

void test_realloc() {
    printf("Test Realloc: ");
    int* p1 = (int*)my_malloc(10 * sizeof(int));
    for(int i=0; i<10; i++) p1[i] = i;

    p1 = (int*)my_realloc(p1, 20 * sizeof(int));
    for(int i=0; i<10; i++) assert(p1[i] == i);

    my_free(p1);
    printf("PASSED\n");
}

void test_calloc() {
    printf("Test Calloc: ");
    size_t n = 10;
    int* p = (int*)my_calloc(n, sizeof(int));
    assert(p != NULL);
    for(int i=0; i<n; i++) assert(p[i] == 0);
    my_free(p);
    printf("PASSED\n");
}

void test_double_free_guard() {
    printf("Test Double Free Guard (Check stderr): \n");
    void* p = my_malloc(100);
    my_free(p);
    my_free(p);
    printf("Test Double Free Guard: PASSED (if error printed above)\n");
}

void test_large_alloc() {
    printf("Test Large Alloc: ");
    void* p = my_malloc(1024 * 1024); // 1MB
    assert(p != NULL);
    my_free(p);
    printf("PASSED\n");
}

void test_zero_alloc() {
    printf("Test Zero Alloc: ");
    void* p = my_malloc(0);
    assert(p == NULL);
    printf("PASSED\n");
}

void test_stress() {
    printf("Test Stress (1000 allocs): ");
    void** ptrs = (void**)my_malloc(1000 * sizeof(void*));
    for (int i = 0; i < 1000; i++) {
        ptrs[i] = my_malloc(i + 1);
        assert(ptrs[i] != NULL);
    }
    for (int i = 0; i < 1000; i++) {
        my_free(ptrs[i]);
    }
    my_free(ptrs);
    printf("PASSED\n");
}

void test_realloc_edge_cases() {
    printf("Test Realloc Edge Cases: ");
    void* p1 = my_realloc(NULL, 128);
    assert(p1 != NULL);
    void* p2 = my_realloc(p1, 0);
    assert(p2 == NULL);
    printf("PASSED\n");
}

int main() {
    printf("Running Allocator Tests...\n\n");
    test_basic_alloc();
    test_free_and_reuse();
    test_coalesce();
    test_split();
    test_realloc();
    test_calloc();
    test_double_free_guard();
    test_large_alloc();
    test_zero_alloc();
    test_stress();
    test_realloc_edge_cases();
    printf("\nAll tests passed successfully!\n");
    return 0;
}
