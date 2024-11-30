/**
 * @file allocator.c
 * @brief Memory allocator implementation.
 * 
 * @author Ameed Othman
 * @date 30/11/2024
 */
#include "allocator.h"
#include "allocator_internal.h"
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/mman.h>

// Constants and Macros
#define ALIGNMENT 16
#define BLOCK_SIZE sizeof(memory_block_t)
#define PAGE_SIZE sysconf(_SC_PAGESIZE)

// Align size to the nearest multiple of ALIGNMENT
static size_t align_size(size_t size) {
    return (size + (ALIGNMENT - 1)) & ~(ALIGNMENT - 1);
}

// Data Structures
typedef struct memory_block {
    size_t size;
    int free;
    struct memory_block* next;
    struct memory_block* prev;
} memory_block_t;

static memory_block_t* free_list = NULL;
static pthread_mutex_t allocator_mutex = PTHREAD_MUTEX_INITIALIZER;

// Helper Functions
static memory_block_t* extend_heap(memory_block_t* last, size_t size);
static memory_block_t* find_block(size_t size);
static void split_block(memory_block_t* block, size_t size);
static memory_block_t* get_block(void* ptr);
static int valid_block(memory_block_t* block);
static void merge_blocks(memory_block_t* block);

int allocator_init(void) {
    // Initialization if required
    return 0;
}

void allocator_destroy(void) {
    pthread_mutex_lock(&allocator_mutex);
    memory_block_t* current = free_list;
    while (current) {
        memory_block_t* next = current->next;
        if (!current->free) {
            munmap(current, current->size + BLOCK_SIZE);
        }
        current = next;
    }
    free_list = NULL;
    pthread_mutex_unlock(&allocator_mutex);
}

void* allocator_malloc(size_t size) {
    if (size == 0) {
        return NULL;
    }
    size = align_size(size);
    pthread_mutex_lock(&allocator_mutex);
    memory_block_t* block = find_block(size);
    if (block) {
        block->free = 0;
        if (block->size > size + BLOCK_SIZE + ALIGNMENT) {
            split_block(block, size);
        }
    } else {
        block = extend_heap(free_list, size);
        if (!block) {
            pthread_mutex_unlock(&allocator_mutex);
            return NULL;
        }
    }
    pthread_mutex_unlock(&allocator_mutex);
    return (void*)(block + 1);
}

void* allocator_realloc(void* ptr, size_t size) {
    if (ptr == NULL) {
        return allocator_malloc(size);
    }
    if (size == 0) {
        allocator_free(ptr);
        return NULL;
    }
    memory_block_t* block = get_block(ptr);
    if (!valid_block(block)) {
        return NULL;
    }
    size = align_size(size);
    pthread_mutex_lock(&allocator_mutex);
    if (block->size >= size) {
        if (block->size > size + BLOCK_SIZE + ALIGNMENT) {
            split_block(block, size);
        }
        pthread_mutex_unlock(&allocator_mutex);
        return ptr;
    } else {
        pthread_mutex_unlock(&allocator_mutex);
        void* new_ptr = allocator_malloc(size);
        if (!new_ptr) {
            return NULL;
        }
        memcpy(new_ptr, ptr, block->size);
        allocator_free(ptr);
        return new_ptr;
    }
}

void allocator_free(void* ptr) {
    if (ptr == NULL) {
        return;
    }
    pthread_mutex_lock(&allocator_mutex);
    memory_block_t* block = get_block(ptr);
    if (!valid_block(block)) {
        pthread_mutex_unlock(&allocator_mutex);
        return;
    }
    block->free = 1;
    merge_blocks(block);
    pthread_mutex_unlock(&allocator_mutex);
}

void* allocator_calloc(size_t nmemb, size_t size) {
    size_t total_size = nmemb * size;
    void* ptr = allocator_malloc(total_size);
    if (ptr) {
        memset(ptr, 0, total_size);
    }
    return ptr;
}

// Internal Helper Function Implementations

static memory_block_t* find_block(size_t size) {
    memory_block_t* current = free_list;
    while (current) {
        if (current->free && current->size >= size) {
            return current;
        }
        current = current->next;
    }
    return NULL;
}

static memory_block_t* extend_heap(memory_block_t* last, size_t size) {
    size_t total_size = size + BLOCK_SIZE;
    total_size = (total_size + PAGE_SIZE - 1) & ~(PAGE_SIZE - 1); // Align to page size
    memory_block_t* block = mmap(NULL, total_size, PROT_READ | PROT_WRITE,
                                 MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (block == MAP_FAILED) {
        return NULL;
    }
    block->size = total_size - BLOCK_SIZE;
    block->free = 0;
    block->next = NULL;
    block->prev = last;
    if (last) {
        last->next = block;
    } else {
        free_list = block;
    }
    return block;
}

static void split_block(memory_block_t* block, size_t size) {
    memory_block_t* new_block = (memory_block_t*)((char*)block + BLOCK_SIZE + size);
    new_block->size = block->size - size - BLOCK_SIZE;
    new_block->free = 1;
    new_block->next = block->next;
    new_block->prev = block;
    if (new_block->next) {
        new_block->next->prev = new_block;
    }
    block->size = size;
    block->next = new_block;
}

static memory_block_t* get_block(void* ptr) {
    return (memory_block_t*)ptr - 1;
}

static int valid_block(memory_block_t* block) {
    return block && block->free == 0;
}

static void merge_blocks(memory_block_t* block) {
    // Merge with next block if possible
    if (block->next && block->next->free) {
        block->size += BLOCK_SIZE + block->next->size;
        block->next = block->next->next;
        if (block->next) {
            block->next->prev = block;
        }
    }
    // Merge with previous block if possible
    if (block->prev && block->prev->free) {
        block->prev->size += BLOCK_SIZE + block->size;
        block->prev->next = block->next;
        if (block->next) {
            block->next->prev = block->prev;
        }
        block = block->prev;
    }
}