/**
 * @file allocator.h
 * @brief Memory allocator interface.
 * 
 * @author Ameed Othman 
 * @date 30/11/2024
 */

#ifndef __ALLOCATOR_H__
#define __ALLOCATOR_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>

/**
 * @brief Initializes the memory allocator.
 *
 * @return int Returns 0 on success, non-zero on failure.
 */
int allocator_init(void);

/**
 * @brief Shuts down the memory allocator and releases all resources.
 */
void allocator_destroy(void);

/**
 * @brief Allocates a block of memory.
 *
 * @param size The size of the memory block in bytes.
 * @return void* Pointer to the allocated memory, or NULL on failure.
 */
void* allocator_malloc(size_t size);

/**
 * @brief Reallocates a previously allocated memory block.
 *
 * @param ptr Pointer to the existing memory block.
 * @param size The new size of the memory block in bytes.
 * @return void* Pointer to the reallocated memory, or NULL on failure.
 */
void* allocator_realloc(void* ptr, size_t size);

/**
 * @brief Frees a previously allocated memory block.
 *
 * @param ptr Pointer to the memory block to be freed.
 */
void allocator_free(void* ptr);

/**
 * @brief Allocates memory for an array of elements and initializes them to zero.
 *
 * @param nmemb Number of elements.
 * @param size Size of each element in bytes.
 * @return void* Pointer to the allocated memory, or NULL on failure.
 */
void* allocator_calloc(size_t nmemb, size_t size);

#ifdef __cplusplus
}
#endif

#endif