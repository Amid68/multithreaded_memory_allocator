/**
 * @file benchmark_allocator.c
 * @brief Benchmarks for the custom memory allocator.
 *
 * This benchmark measures the performance of the custom allocator for a series
 * of allocation patterns, comparing runtime with a baseline (e.g., standard malloc)
 * in a separate benchmark. The results are written out to a CSV file for further analysis.
 *
 * Patterns tested:
 *  1. Fixed-size allocations (repeatedly allocate and free a block of a fixed size)
 *  2. Variable-size allocations (allocate blocks of varying sizes, then free)
 *  3. Realloc patterns (allocate, then frequently realloc to larger/smaller sizes)
 *
 * Each pattern is run multiple times to collect stable averages.
 *
 * Author: Ameed Othman
 * Date: 06/12/2024
 */

#include "allocator.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define NUM_ITERATIONS 5           // How many times to repeat each test pattern
#define ALLOC_COUNT 10000          // Number of allocations per iteration
#define MAX_VAR_SIZE 1024          // Maximum size for variable-sized allocations
#define CSV_FILE "benchmarks/results/allocator_results.csv"

/**
 * @brief Returns current time in seconds (double).
 */
static double get_time_sec(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec + ts.tv_nsec / 1e9;
}

/**
 * @brief Benchmark a pattern of fixed-size allocations.
 *
 * Each iteration:
 *  - Allocate ALLOC_COUNT blocks of a fixed size (e.g. 64 bytes).
 *  - Immediately free them.
 * Measure and return the total time taken.
 */
static double benchmark_fixed_alloc(size_t alloc_size) {
    double start = get_time_sec();
    for (int i = 0; i < ALLOC_COUNT; i++) {
        void* ptr = allocator_malloc(alloc_size);
        if (!ptr) {
            fprintf(stderr, "Error: allocator_malloc returned NULL in fixed alloc test.\n");
            return -1.0;
        }
        allocator_free(ptr);
    }
    double end = get_time_sec();
    return end - start;
}

/**
 * @brief Benchmark a pattern of variable-size allocations.
 *
 * Each iteration:
 *  - Randomly choose a size between 1 and MAX_VAR_SIZE for ALLOC_COUNT blocks.
 *  - Allocate them, store pointers, then free them all at the end.
 */
static double benchmark_variable_alloc(void) {
    void* pointers[ALLOC_COUNT];
    double start = get_time_sec();
    unsigned seed = 12345;
    for (int i = 0; i < ALLOC_COUNT; i++) {
        size_t size = (rand_r(&seed) % MAX_VAR_SIZE) + 1;
        pointers[i] = allocator_malloc(size);
        if (!pointers[i]) {
            fprintf(stderr, "Error: allocator_malloc returned NULL in variable alloc test.\n");
            return -1.0;
        }
    }
    // Free them all
    for (int i = 0; i < ALLOC_COUNT; i++) {
        allocator_free(pointers[i]);
    }
    double end = get_time_sec();
    return end - start;
}

/**
 * @brief Benchmark a pattern involving reallocation.
 *
 * Each iteration:
 *  - Allocate ALLOC_COUNT blocks of an initial size.
 *  - Reallocate each block to a new size (larger or smaller).
 *  - Free them all.
 */
static double benchmark_realloc_pattern(size_t initial_size) {
    void* pointers[ALLOC_COUNT];
    unsigned seed = 6789;

    // Allocation phase
    double start = get_time_sec();
    for (int i = 0; i < ALLOC_COUNT; i++) {
        pointers[i] = allocator_malloc(initial_size);
        if (!pointers[i]) {
            fprintf(stderr, "Error: allocator_malloc returned NULL in realloc pattern test.\n");
            return -1.0;
        }
    }

    // Realloc phase
    for (int i = 0; i < ALLOC_COUNT; i++) {
        size_t new_size = (rand_r(&seed) % MAX_VAR_SIZE) + 1;
        void* new_ptr = allocator_realloc(pointers[i], new_size);
        if (!new_ptr) {
            fprintf(stderr, "Error: allocator_realloc returned NULL in realloc pattern test.\n");
            return -1.0;
        }
        pointers[i] = new_ptr;
    }

    // Free phase
    for (int i = 0; i < ALLOC_COUNT; i++) {
        allocator_free(pointers[i]);
    }

    double end = get_time_sec();
    return end - start;
}

int main(void) {
    // Initialize the allocator once per run
    if (allocator_init() != 0) {
        fprintf(stderr, "Error: Failed to initialize allocator.\n");
        return EXIT_FAILURE;
    }

    FILE* fp = fopen(CSV_FILE, "w");
    if (!fp) {
        fprintf(stderr, "Error: Could not open %s for writing.\n", CSV_FILE);
        allocator_destroy();
        return EXIT_FAILURE;
    }

    // CSV header
    fprintf(fp, "Test,Iteration,TimeSeconds\n");

    // Fixed-size allocation test
    size_t fixed_size = 64;
    for (int i = 0; i < NUM_ITERATIONS; i++) {
        double time = benchmark_fixed_alloc(fixed_size);
        if (time < 0) {
            fclose(fp);
            allocator_destroy();
            return EXIT_FAILURE;
        }
        fprintf(fp, "fixed_size_%zu,%d,%.6f\n", fixed_size, i + 1, time);
    }

    // Variable-size allocation test
    for (int i = 0; i < NUM_ITERATIONS; i++) {
        double time = benchmark_variable_alloc();
        if (time < 0) {
            fclose(fp);
            allocator_destroy();
            return EXIT_FAILURE;
        }
        fprintf(fp, "variable_size,%d,%.6f\n", i + 1, time);
    }

    // Realloc pattern test
    size_t initial_size = 128;
    for (int i = 0; i < NUM_ITERATIONS; i++) {
        double time = benchmark_realloc_pattern(initial_size);
        if (time < 0) {
            fclose(fp);
            allocator_destroy();
            return EXIT_FAILURE;
        }
        fprintf(fp, "realloc_pattern_%zu,%d,%.6f\n", initial_size, i + 1, time);
    }

    fclose(fp);
    allocator_destroy();

    printf("Benchmark completed. Results written to %s\n", CSV_FILE);
    return EXIT_SUCCESS;
}
