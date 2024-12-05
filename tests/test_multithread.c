/**
 * @file test_multithread.c
 * @brief Unit tests for multithreaded usage of the custom memory allocator.
 *
 * This test suite verifies that the allocator behaves correctly and safely
 * when used from multiple threads concurrently. We will:
 *  - Spawn multiple threads performing random allocations, frees, and reallocations.
 *  - Check for data consistency and absence of crashes or corruption.
 *
 * Author: Ameed Othman
 * Date: 03/12/2024
 */

#include "unity.h"
#include "allocator.h"
#include <pthread.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <stdint.h>

/* -------------------------------------------------------------------------
 * Configuration Constants
 * ------------------------------------------------------------------------- */

/** @brief Number of threads to create for testing concurrent operations. */
#define NUM_THREADS 8

/** @brief Number of allocations per thread. */
#define ALLOCS_PER_THREAD 1000

/** @brief Maximum size of each allocation. */
#define MAX_ALLOC_SIZE 1024

/** @brief Pattern to initialize allocated memory. */
#define INIT_PATTERN 0xA5

/** @brief Pattern to use after reallocation. */
#define REALLOC_PATTERN 0x5A

/* -------------------------------------------------------------------------
 * Test Data Structures and Globals
 * ------------------------------------------------------------------------- */

typedef struct {
    int thread_id;
    unsigned seed;
} thread_arg_t;

/* -------------------------------------------------------------------------
 * Utility Functions for Tests
 * ------------------------------------------------------------------------- */

/**
 * @brief Allocate, fill, reallocate and free a sequence of random-sized blocks.
 * 
 * Each thread will perform a series of allocations of random sizes, write a
 * known pattern to them, then occasionally reallocate them to another size
 * and verify that the original pattern is intact in the overlapping region.
 * Finally, it will free the blocks.
 */
static void* thread_worker(void* arg) {
    thread_arg_t* t_arg = (thread_arg_t*)arg;
    unsigned seed = t_arg->seed;

    for (int i = 0; i < ALLOCS_PER_THREAD; i++) {
        // Random size between 1 and MAX_ALLOC_SIZE
        size_t size = (rand_r(&seed) % MAX_ALLOC_SIZE) + 1;

        // Allocate memory
        uint8_t* ptr = (uint8_t*)allocator_malloc(size);
        TEST_ASSERT_NOT_NULL_MESSAGE(ptr, "Failed to allocate memory in multithread test.");

        // Fill with INIT_PATTERN
        memset(ptr, INIT_PATTERN, size);

        // Possibly realloc to a new size ~50% of the time
        if ((rand_r(&seed) % 2) == 0) {
            size_t new_size = (rand_r(&seed) % MAX_ALLOC_SIZE) + 1;
            uint8_t* new_ptr = (uint8_t*)allocator_realloc(ptr, new_size);
            TEST_ASSERT_NOT_NULL_MESSAGE(new_ptr, "Failed to realloc memory in multithread test.");

            // Check old pattern up to the min of old and new sizes
            size_t min_size = (size < new_size) ? size : new_size;
            for (size_t j = 0; j < min_size; j++) {
                TEST_ASSERT_EQUAL_UINT8_MESSAGE(INIT_PATTERN, new_ptr[j], "Data corrupted after realloc in multithread test.");
            }

            // Fill the remainder (if any) with REALLOC_PATTERN
            if (new_size > min_size) {
                memset(new_ptr + min_size, REALLOC_PATTERN, new_size - min_size);
            }

            ptr = new_ptr;
            size = new_size;
        }

        // Optional: Check memory after possible realloc
        // Not strictly necessary, but we can ensure patterns are consistent
        // since we never changed the front portion except to verify.

        // Free the block
        allocator_free(ptr);
    }

    return NULL;
}

/* -------------------------------------------------------------------------
 * Test Case
 * ------------------------------------------------------------------------- */

/**
 * @brief Tests concurrent allocations, frees, and reallocations from multiple threads.
 *
 * This test launches multiple threads, each performing random allocations and
 * frees. It verifies no corruption or failures occur.
 */
void test_multithreaded_allocations(void) {
    pthread_t threads[NUM_THREADS];
    thread_arg_t args[NUM_THREADS];

    // Initialize the allocator once
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, allocator_init(), "Failed to initialize allocator for multithread test.");

    // Create threads
    for (int i = 0; i < NUM_THREADS; i++) {
        args[i].thread_id = i;
        args[i].seed = (unsigned)(time(NULL) + i);
        int rc = pthread_create(&threads[i], NULL, thread_worker, &args[i]);
        TEST_ASSERT_EQUAL_INT_MESSAGE(0, rc, "Failed to create test thread.");
    }

    // Join threads
    for (int i = 0; i < NUM_THREADS; i++) {
        int rc = pthread_join(threads[i], NULL);
        TEST_ASSERT_EQUAL_INT_MESSAGE(0, rc, "Failed to join test thread.");
    }

    // Destroy the allocator
    allocator_destroy();

    // If we reach here, no assertions failed and no crashes occurred.
    TEST_PASS_MESSAGE("Multithreaded allocations, frees, and reallocations passed without errors.");
}

/* -------------------------------------------------------------------------
 * Test Runner
 * 
 * We define main() so this test file can be run individually if desired.
 * Usually, it can be integrated into a unified test runner. 
 * ------------------------------------------------------------------------- */

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_multithreaded_allocations);
    return UNITY_END();
}
