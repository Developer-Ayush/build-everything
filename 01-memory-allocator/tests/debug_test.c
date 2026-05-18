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
    printf("Test Free and Reuse:\n");
    void* p1 = my_malloc(128);
    printf("  p1 = %p\n", p1);
    print_heap();

    my_free(p1);
    printf("  After free(p1):\n");
    print_heap();

    void* p2 = my_malloc(128);
    printf("  p2 = %p\n", p2);
    print_heap();

    if (p1 != p2) {
        printf("  FAILED: p1 (%p) != p2 (%p)\n", p1, p2);
    } else {
        printf("  PASSED\n");
    }
}

int main() {
    test_basic_alloc();
    test_free_and_reuse();
    return 0;
}
