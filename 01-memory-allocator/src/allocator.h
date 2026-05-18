#ifndef ALLOCATOR_H
#define ALLOCATOR_H

#include <stddef.h>

/**
 * Block Header
 * size: size of the data block (excluding header)
 * free: 1 if block is free, 0 if allocated
 * next: pointer to the next block in the heap
 * prev: pointer to the previous block in the heap
 */
typedef struct Block {
    size_t size;
    int free;
    struct Block* next;
    struct Block* prev;
} Block;

#define ALIGNMENT 8
#define ALIGN(size) (((size) + (ALIGNMENT-1)) & ~(ALIGNMENT-1))
#define BLOCK_SIZE ALIGN(sizeof(Block))

void* my_malloc(size_t size);
void my_free(void* ptr);
void* my_realloc(void* ptr, size_t size);
void* my_calloc(size_t nmemb, size_t size);

void my_malloc_stats();
void print_heap(); // Kept for detailed debugging

#endif /* ALLOCATOR_H */
