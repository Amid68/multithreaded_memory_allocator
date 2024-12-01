/**
 * @file allocator_internal.h
 * @brief Internal definitions and declarations for the memory allocator.
 * 
 * This file contains internal data structures and helper function prototypes
 * used by the memory allocator. These definitions are not exposed to the users
 * of the allocator but are necessary for its implementation.
 * 
 * @author Ameed Othman
 * @date 01/12/2024
 */

#ifndef __ALLOCATOR_INTERNAL_H__
#define __ALLOCATOR_INTERNAL_H__

#include <stddef.h>

// Data Structures
typedef struct memory_block {
    size_t size;               // Size of the memory block
    int free;                  // Flag indicating if the block is free (1) or in use (0)
    struct memory_block* next; // Pointer to the next memory block in the list
    struct memory_block* prev; // Pointer to the previous memory block in the list
} memory_block_t;

// Helper Function Prototypes

/**
 * @brief Aligns the given size to the nearest multiple of ALIGNMENT.
 *
 * @param size The size to be aligned.
 * @return size_t The aligned size.
 */
static size_t align_size(size_t size);

/**
 * @brief Extends the heap by allocating a new memory block.
 *
 * @param last Pointer to the last memory block in the current list.
 * @param size The size of the new memory block in bytes.
 * @return memory_block_t* Pointer to the newly allocated memory block, or NULL on failure.
 */
static memory_block_t* extend_heap(memory_block_t* last, size_t size);

/**
 * @brief Finds a free memory block of at least the specified size.
 *
 * @param size The size of the memory block to find.
 * @return memory_block_t* Pointer to the found memory block, or NULL if no suitable block is found.
 */
static memory_block_t* find_block(size_t size);

/**
 * @brief Splits a memory block into two blocks if the block is larger than the requested size.
 *
 * @param block Pointer to the memory block to be split.
 * @param size The size of the first block after splitting.
 */
static void split_block(memory_block_t* block, size_t size);

/**
 * @brief Retrieves the memory block metadata for a given memory pointer.
 *
 * @param ptr Pointer to the memory allocated by the allocator.
 * @return memory_block_t* Pointer to the memory block metadata.
 */
static memory_block_t* get_block(void* ptr);

/**
 * @brief Checks if a memory block is valid and in use.
 *
 * @param block Pointer to the memory block to check.
 * @return int Returns 1 if the block is valid and in use, 0 otherwise.
 */
static int valid_block(memory_block_t* block);

/**
 * @brief Merges adjacent free memory blocks into a single block to reduce fragmentation.
 *
 * @param block Pointer to the memory block to be merged.
 */
static void merge_blocks(memory_block_t* block);

#endif