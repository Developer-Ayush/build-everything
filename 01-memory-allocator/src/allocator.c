#include "allocator.h"
#include <unistd.h>
#include <stdio.h>
#include <string.h>

static Block* heap_head = NULL;
static Block* heap_tail = NULL;

/**
 * Finds a free block that can accommodate the requested size.
 * Uses First-Fit strategy.
 */
static Block* find_free_block(size_t size) {
    Block* current = heap_head;
    while (current) {
        if (current->free && current->size >= size) {
            return current;
        }
        current = current->next;
    }
    return NULL;
}

/**
 * Splits a block into two if it's large enough to hold the requested size
 * plus a new header and some minimal data.
 */
static void split_block(Block* block, size_t size) {
    if (block->size >= size + BLOCK_SIZE + ALIGNMENT) {
        Block* new_block = (Block*)((char*)block + BLOCK_SIZE + size);
        new_block->size = block->size - size - BLOCK_SIZE;
        new_block->free = 1;
        new_block->next = block->next;
        new_block->prev = block;

        if (block->next) {
            block->next->prev = new_block;
        } else {
            heap_tail = new_block;
        }

        block->next = new_block;
        block->size = size;
    }
}

/**
 * Coalesces adjacent free blocks.
 */
static void coalesce_blocks(Block* block) {
    if (block->next && block->next->free) {
        block->size += BLOCK_SIZE + block->next->size;
        block->next = block->next->next;
        if (block->next) {
            block->next->prev = block;
        } else {
            heap_tail = block;
        }
    }

    if (block->prev && block->prev->free) {
        block->prev->size += BLOCK_SIZE + block->size;
        block->prev->next = block->next;
        if (block->next) {
            block->next->prev = block->prev;
        } else {
            heap_tail = block->prev;
        }
    }
}

/**
 * Requests more memory from the OS using sbrk.
 */
static Block* request_space(size_t size) {
    void* request = sbrk(size + BLOCK_SIZE);

    if (request == (void*)-1) {
        return NULL; // sbrk failed
    }

    Block* block = (Block*)request;
    block->size = size;
    block->free = 0;
    block->next = NULL;

    if (heap_head == NULL) {
        heap_head = block;
        heap_tail = block;
        block->prev = NULL;
    } else {
        heap_tail->next = block;
        block->prev = heap_tail;
        heap_tail = block;
    }

    return block;
}

void* my_malloc(size_t size) {
    if (size == 0) { // Requirement: my_malloc(0) returns NULL
        return NULL;
    }

    size_t aligned_size = ALIGN(size);
    Block* block = find_free_block(aligned_size);

    if (block) {
        block->free = 0;
        split_block(block, aligned_size);
    } else {
        block = request_space(aligned_size);
        if (!block) {
            return NULL;
        }
    }

    return (void*)((char*)block + BLOCK_SIZE);
}

void my_free(void* ptr) {
    if (!ptr) { // Requirement: my_free(NULL) is no-op
        return;
    }

    Block* block = (Block*)((char*)ptr - BLOCK_SIZE);

    if (block->free) {
        fprintf(stderr, "Error: Double free detected at %p\n", ptr);
        return;
    }

    block->free = 1;
    coalesce_blocks(block);
}

void* my_realloc(void* ptr, size_t size) {
    if (!ptr) { // Requirement: my_realloc(NULL, size) behaves like my_malloc(size)
        return my_malloc(size);
    }

    if (size == 0) { // Requirement: my_realloc(ptr, 0) behaves like my_free(ptr)
        my_free(ptr);
        return NULL;
    }

    Block* block = (Block*)((char*)ptr - BLOCK_SIZE);
    size_t aligned_size = ALIGN(size);

    if (block->size >= aligned_size) {
        split_block(block, aligned_size);
        return ptr;
    }

    if (block->next && block->next->free && (block->size + BLOCK_SIZE + block->next->size) >= aligned_size) {
        block->size += BLOCK_SIZE + block->next->size;
        block->next = block->next->next;
        if (block->next) {
            block->next->prev = block;
        } else {
            heap_tail = block;
        }
        split_block(block, aligned_size);
        return ptr;
    }

    void* new_ptr = my_malloc(size);
    if (!new_ptr) {
        return NULL;
    }

    memcpy(new_ptr, ptr, block->size);
    my_free(ptr);

    return new_ptr;
}

void* my_calloc(size_t nmemb, size_t size) {
    if (nmemb == 0 || size == 0) return NULL;
    if (nmemb > (size_t)-1 / size) return NULL;

    size_t total_size = nmemb * size;
    void* ptr = my_malloc(total_size);
    if (ptr) {
        memset(ptr, 0, total_size);
    }
    return ptr;
}

void my_malloc_stats() {
    size_t total_heap = 0;
    size_t free_blocks = 0;
    size_t alloc_blocks = 0;
    size_t free_bytes = 0;

    Block* current = heap_head;
    while (current) {
        total_heap += BLOCK_SIZE + current->size;
        if (current->free) {
            free_blocks++;
            free_bytes += current->size;
        } else {
            alloc_blocks++;
        }
        current = current->next;
    }

    printf("\n--- Allocator Stats ---\n");
    printf("Total Heap Size:   %zu bytes\n", total_heap);
    printf("Allocated Blocks:  %zu\n", alloc_blocks);
    printf("Free Blocks:       %zu\n", free_blocks);
    printf("Total Free Space:  %zu bytes\n", free_bytes);
    printf("-----------------------\n\n");
}

void print_heap() {
    Block* current = heap_head;
    printf("Heap List: [");
    while (current) {
        printf("{ptr: %p, size: %zu, free: %d}", (void*)((char*)current + BLOCK_SIZE), current->size, current->free);
        if (current->next) printf(", ");
        current = current->next;
    }
    printf("]\n");
}
