/**
 * @file test_performance.c
 * @brief Basic performance tests for the custom memory allocator.
 *
 * These tests measure the time required for a large number of allocations,
 * reallocations, and frees to ensure that the custom memory allocator performs
 * within a reasonable time frame. While these are not strict unit tests, we use
 * Unity to structure and report results. In practice, performance tests may be
 * more free-form and not necessarily integrated with a unit testing framework.
 *
 * Author: Ameed Othman
 * Date: 04/12/2024
 */

#include "unity.h"
#include "allocator.h"
#include <time.h>
#include <stdlib.h>
#include <string.h>

/* -------------------------------------------------------------------------
 * Configuration for Performance Tests
 * ------------------------------------------------------------------------- */

/** @brief Number of allocations to perform in each performance test. */
#define NUM_ALLOCS 100000

/** @brief Size of each allocation. */
#define ALLOC_SIZE 64

/** @brief Acceptable max time (in seconds) for mass allocation test.
 *
 * This is an arbitrary threshold. Real-world thresholds should be based on
 * system capabilities or regression baselines. We pick a somewhat lenient
 * threshold that should be easily met by a well-implemented allocator on a 
 * modern system.
 */
#define MAX_ALLOC_TIME 1.0

/** @brief Acceptable max time (in seconds) for mass realloc test. */
#define MAX_REALLOC_TIME 1.5

/** @brief Acceptable max time (in seconds) for mass free test. */
#define MAX_FREE_TIME 0.8

/* -------------------------------------------------------------------------
 * Helper Functions
 * ------------------------------------------------------------------------- */

/**
 * @brief Get the current time in seconds as a double-precision float.
 *
 * Uses CLOCK_MONOTONIC for stable timing measurements not affected by system
 * clock changes.
 *
 * @return double Current time in seconds.
 */
static double get_time_sec(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec + (ts.tv_nsec / 1e9);
}

/**
 * @brief Allocates NUM_ALLOCS blocks, each of ALLOC_SIZE bytes, and measures the time.
 *
 * @param pointers Output array of pointers to allocated blocks.
 * @return double Elapsed time in seconds.
 */
static double measure_allocation_time(void** pointers) {
    double start = get_time_sec();
    for (int i = 0; i < NUM_ALLOCS; i++) {
        pointers[i] = allocator_malloc(ALLOC_SIZE);
        TEST_ASSERT_NOT_NULL_MESSAGE(pointers[i], "allocator_malloc returned NULL unexpectedly.");
        memset(pointers[i], 0xAB, ALLOC_SIZE); // Fill to simulate usage
    }
    double end = get_time_sec();
    return end - start;
}

/**
 * @brief Reallocates each block to a new size and measures the time.
 *
 * This simulates growth or shrink of memory usage.
 *
 * @param pointers The array of pointers to reallocate.
 * @return double Elapsed time in seconds.
 */
static double measure_realloc_time(void** pointers) {
    double start = get_time_sec();
    for (int i = 0; i < NUM_ALLOCS; i++) {
        size_t new_size = (i % 2 == 0) ? (ALLOC_SIZE * 2) : (ALLOC_SIZE / 2 + 1);
        void* new_ptr = allocator_realloc(pointers[i], new_size);
        TEST_ASSERT_NOT_NULL_MESSAGE(new_ptr, "allocator_realloc returned NULL unexpectedly.");
        // Check some portion of old data if shrinking
        // Not strictly necessary, this is a performance test
        pointers[i] = new_ptr;
    }
    double end = get_time_sec();
    return end - start;
}

/**
 * @brief Frees each allocated block and measures the time.
 *
 * @param pointers The array of pointers to free.
 * @return double Elapsed time in seconds.
 */
static double measure_free_time(void** pointers) {
    double start = get_time_sec();
    for (int i = 0; i < NUM_ALLOCS; i++) {
        allocator_free(pointers[i]);
        pointers[i] = NULL;
    }
    double end = get_time_sec();
    return end - start;
}

/* -------------------------------------------------------------------------
 * Test Cases
 * ------------------------------------------------------------------------- */

/**
 * @brief Tests the time to allocate, reallocate, and free a large number of blocks.
 *
 * This test ensures that the allocator can handle many allocations without
 * excessive slowdown. It's not a strict pass/fail scenario since performance
 * can vary by machine. However, we define some upper time limits and assert
 * that we stay below them.
 */
void test_allocator_performance(void) {
    // Initialize the allocator
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, allocator_init(), "allocator_init failed.");

    void** pointers = (void**)allocator_malloc(NUM_ALLOCS * sizeof(void*));
    TEST_ASSERT_NOT_NULL_MESSAGE(pointers, "Failed to allocate pointer array for test.");

    double alloc_time = measure_allocation_time(pointers);
    double realloc_time = measure_realloc_time(pointers);
    double free_time = measure_free_time(pointers);

    // Cleanup the pointer array itself
    allocator_free(pointers);

    // Destroy the allocator
    allocator_destroy();

    // Print the timing results
    printf("Performance Results:\n");
    printf("  Allocation time for %d blocks: %.6f seconds\n", NUM_ALLOCS, alloc_time);
    printf("  Reallocation time for %d blocks: %.6f seconds\n", NUM_ALLOCS, realloc_time);
    printf("  Free time for %d blocks: %.6f seconds\n", NUM_ALLOCS, free_time);

    // Check performance thresholds
    TEST_ASSERT_MESSAGE(alloc_time < MAX_ALLOC_TIME, "Allocation phase took too long!");
    TEST_ASSERT_MESSAGE(realloc_time < MAX_REALLOC_TIME, "Reallocation phase took too long!");
    TEST_ASSERT_MESSAGE(free_time < MAX_FREE_TIME, "Free phase took too long!");
}

/* -------------------------------------------------------------------------
 * Test Runner
 * 
 * Defines main() so this test file can be invoked standalone. It can also
 * integrate with a larger test runner that calls RUN_TEST on these tests.
 * ------------------------------------------------------------------------- */

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_allocator_performance);
    return UNITY_END();
}
