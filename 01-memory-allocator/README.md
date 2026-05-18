# Custom Memory Allocator

This project is a complete, from-scratch implementation of a memory allocator in C. It provides replacements for the standard library's `malloc`, `free`, `realloc`, and `calloc` functions, managing a process's heap memory directly through system calls. It demonstrates core systems programming concepts including manual memory management, pointer arithmetic, and heap fragmentation control.

## How the Free List Works

The allocator manages memory using a **doubly-linked list** of all blocks in the heap. Each block consists of a **Header** followed by the actual payload.

### Block Header Structure
```c
typedef struct Block {
    size_t size;        // Size of the payload (aligned to 8 bytes)
    int free;           // 1 if block is free, 0 if allocated
    struct Block* next; // Pointer to next block in heap
    struct Block* prev; // Pointer to previous block in heap
} Block;
```

### Allocation Strategy
- **First-Fit Search**: When `my_malloc(size)` is called, the allocator traverses the list from the `heap_head` and selects the first free block large enough to satisfy the request.
- **Splitting**: If a found free block is significantly larger than the requested size (specifically, large enough to fit another header and at least 8 bytes of data), it is split. The first part is allocated, and the remainder becomes a new free block in the list.
- **Heap Growth**: If no suitable free block is found, the allocator calls `sbrk()` to request more memory from the OS and appends a new block to the end of the list.

### Deallocation and Coalescing
- **Immediate Coalescing**: When `my_free(ptr)` is called, the block is marked as free. To prevent fragmentation, the allocator immediately checks its `next` and `prev` neighbors. If either neighbor is also free, they are merged into a single larger block.

### Heap Layout Diagram
```text
      Heap Start                                                             Heap End
      |                                                                             |
      v                                                                             v
      +-----------+---------+      +-----------+---------+      +-----------+---------+
      |  Header   | Payload | ---> |  Header   | Payload | ---> |  Header   | Payload |
      | (8b align)| (Alloc) | <--- | (8b align)| (Free)  | <--- | (8b align)| (Alloc) |
      +-----------+---------+      +-----------+---------+      +-----------+---------+
      ^                            ^                            ^
      | Block 1 (size: 64)         | Block 2 (size: 128)        | Block 3 (size: 32)
```

## How to Compile and Run

### Run the Demo
The demo program showcases basic allocation, calloc, realloc, splitting, coalescing, and edge case handling.
```bash
gcc -o allocator src/allocator.c src/main.c && ./allocator
```

### Run the Tests
The test suite includes 11 tests covering functional requirements and stress testing.
```bash
gcc -o tests tests/test_allocator.c src/allocator.c && ./tests
```

## What was learned
Building this allocator taught me that memory is not just a magical pool of bytes, but a carefully managed resource where every byte of overhead (like headers) matters. I learned how fragmentation can quickly degrade a system's performance if strategies like splitting and coalescing aren't implemented correctly. Understanding the relationship between user-space allocators and the OS kernel via system calls like `sbrk` clarified how processes actually grow their address space. Finally, implementing 8-byte alignment highlighted the necessity of hardware-aware programming for correct memory access and performance.
