/**
 * @file test_allocator.c
 * @brief Unit tests for the custom memory allocator.
 *
 * This file contains a suite of unit tests that validate the core functionalities
 * of the custom memory allocator implemented in allocator.c. We use the Unity
 * testing framework to ensure that functions such as allocator_init(), 
 * allocator_malloc(), allocator_realloc(), allocator_free(), and allocator_calloc() 
 * behave as expected.
 *
 * Author: Ameed Othman
 * Date: 01/12/2024
 */

#include "unity.h"
#include "allocator.h"
#include <string.h>

/* -------------------------------------------------------------------------
 * Test Fixture Setup and Teardown
 * ------------------------------------------------------------------------- */

/**
 * @brief Set up the test fixture.
 *
 * This function is called before each test. It initializes the allocator to ensure
 * we start from a known state.
 */
void setUp(void) {
    int init_result = allocator_init();
    TEST_ASSERT_EQUAL_MESSAGE(0, init_result, "Allocator initialization failed.");
}

/**
 * @brief Tear down the test fixture.
 *
 * This function is called after each test. It cleans up the allocator, releasing
 * all resources so each test is isolated.
 */
void tearDown(void) {
    allocator_destroy();
}

/* -------------------------------------------------------------------------
 * Test Cases
 * ------------------------------------------------------------------------- */

/**
 * @brief Test basic allocation and deallocation.
 *
 * This test ensures that we can allocate a small block, verify it is not NULL, 
 * and then free it without any errors.
 */
void test_allocator_malloc_and_free(void) {
    size_t alloc_size = 64;
    void* ptr = allocator_malloc(alloc_size);
    TEST_ASSERT_NOT_NULL_MESSAGE(ptr, "allocator_malloc returned NULL for a valid size.");
    allocator_free(ptr);
}

/**
 * @brief Test zero-sized allocation.
 *
 * Allocating zero bytes should return NULL (as per the current implementation).
 */
void test_allocator_malloc_zero_size(void) {
    void* ptr = allocator_malloc(0);
    TEST_ASSERT_NULL_MESSAGE(ptr, "allocator_malloc did not return NULL for size zero.");
}

/**
 * @brief Test reallocation with a larger size.
 *
 * Allocate a small block, fill it with data, then reallocate to a larger size 
 * and verify that the original data is intact.
 */
void test_allocator_realloc_grow(void) {
    size_t initial_size = 32;
    size_t larger_size = 128;
    unsigned char pattern = 0xAB;

    void* ptr = allocator_malloc(initial_size);
    TEST_ASSERT_NOT_NULL(ptr);

    // Fill memory with a known pattern
    memset(ptr, pattern, initial_size);

    // Reallocate to a larger size
    void* new_ptr = allocator_realloc(ptr, larger_size);
    TEST_ASSERT_NOT_NULL_MESSAGE(new_ptr, "allocator_realloc returned NULL for larger size.");

    // Verify old data is still correct
    unsigned char* data = (unsigned char*)new_ptr;
    for (size_t i = 0; i < initial_size; i++) {
        TEST_ASSERT_EQUAL_UINT8_MESSAGE(pattern, data[i], "Data corrupted after realloc.");
    }

    allocator_free(new_ptr);
}

/**
 * @brief Test reallocation to a smaller size.
 *
 * Allocate a block, fill it with data, then reduce its size and verify that 
 * the remaining portion of the data is intact.
 */
void test_allocator_realloc_shrink(void) {
    size_t initial_size = 128;
    size_t smaller_size = 32;
    unsigned char pattern = 0xCD;

    void* ptr = allocator_malloc(initial_size);
    TEST_ASSERT_NOT_NULL(ptr);

    // Fill memory with a known pattern
    memset(ptr, pattern, initial_size);

    // Reallocate to a smaller size
    void* new_ptr = allocator_realloc(ptr, smaller_size);
    TEST_ASSERT_NOT_NULL_MESSAGE(new_ptr, "allocator_realloc returned NULL when shrinking.");

    // Verify old data in the reduced block is still correct
    unsigned char* data = (unsigned char*)new_ptr;
    for (size_t i = 0; i < smaller_size; i++) {
        TEST_ASSERT_EQUAL_UINT8_MESSAGE(pattern, data[i], "Data corrupted after shrinking realloc.");
    }

    allocator_free(new_ptr);
}

/**
 * @brief Test reallocation with a NULL pointer.
 *
 * Passing a NULL pointer to realloc should behave like malloc.
 */
void test_allocator_realloc_from_null(void) {
    size_t alloc_size = 64;
    void* new_ptr = allocator_realloc(NULL, alloc_size);
    TEST_ASSERT_NOT_NULL_MESSAGE(new_ptr, "allocator_realloc(NULL) did not behave like malloc.");
    allocator_free(new_ptr);
}

/**
 * @brief Test freeing a NULL pointer.
 *
 * Freeing NULL should not cause any errors or changes in behavior.
 */
void test_allocator_free_null(void) {
    allocator_free(NULL);
    // No explicit test assertion needed, as we only ensure no crash or error occurs.
    TEST_PASS_MESSAGE("No crash or error when freeing NULL.");
}

/**
 * @brief Test calloc functionality.
 *
 * Allocating memory with calloc should return a zero-initialized block of memory.
 */
void test_allocator_calloc(void) {
    size_t nmemb = 10;
    size_t size = sizeof(int);
    void* ptr = allocator_calloc(nmemb, size);
    TEST_ASSERT_NOT_NULL_MESSAGE(ptr, "allocator_calloc returned NULL.");

    // Verify memory is zero-initialized
    unsigned char* data = (unsigned char*)ptr;
    for (size_t i = 0; i < nmemb * size; i++) {
        TEST_ASSERT_EQUAL_UINT8_MESSAGE(0x00, data[i], "Memory not zero-initialized by calloc.");
    }

    allocator_free(ptr);
}

/**
 * @brief Test behavior for realloc with zero size.
 *
 * Reallocating a valid pointer with a zero size should free the block and return NULL.
 */
void test_allocator_realloc_zero_size(void) {
    size_t alloc_size = 64;
    void* ptr = allocator_malloc(alloc_size);
    TEST_ASSERT_NOT_NULL(ptr);

    void* new_ptr = allocator_realloc(ptr, 0);
    TEST_ASSERT_NULL_MESSAGE(new_ptr, "allocator_realloc with zero size should return NULL.");
    // The original block should be freed at this point.
}

/* -------------------------------------------------------------------------
 * Main Runner
 * ------------------------------------------------------------------------- */

int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_allocator_malloc_and_free);
    RUN_TEST(test_allocator_malloc_zero_size);
    RUN_TEST(test_allocator_realloc_grow);
    RUN_TEST(test_allocator_realloc_shrink);
    RUN_TEST(test_allocator_realloc_from_null);
    RUN_TEST(test_allocator_free_null);
    RUN_TEST(test_allocator_calloc);
    RUN_TEST(test_allocator_realloc_zero_size);

    return UNITY_END();
}

