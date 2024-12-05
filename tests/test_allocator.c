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

void setUp(void) {
    int init_result = allocator_init();
    TEST_ASSERT_EQUAL_INT(0, init_result);
    if (init_result != 0) {
        TEST_FAIL_MESSAGE("Allocator initialization failed.");
    }
}

void tearDown(void) {
    allocator_destroy();
}

/* -------------------------------------------------------------------------
 * Test Cases
 * ------------------------------------------------------------------------- */

void test_allocator_malloc_and_free(void) {
    size_t alloc_size = 64;
    void* ptr = allocator_malloc(alloc_size);
    TEST_ASSERT_NOT_NULL(ptr);
    if (ptr == NULL) {
        TEST_FAIL_MESSAGE("allocator_malloc returned NULL for a valid size.");
    }
    allocator_free(ptr);
}

void test_allocator_malloc_zero_size(void) {
    void* ptr = allocator_malloc(0);
    TEST_ASSERT_NULL(ptr);
    if (ptr != NULL) {
        TEST_FAIL_MESSAGE("allocator_malloc did not return NULL for size zero.");
    }
}

void test_allocator_realloc_grow(void) {
    size_t initial_size = 32;
    size_t larger_size = 128;
    unsigned char pattern = 0xAB;

    void* ptr = allocator_malloc(initial_size);
    TEST_ASSERT_NOT_NULL(ptr);

    memset(ptr, pattern, initial_size);

    void* new_ptr = allocator_realloc(ptr, larger_size);
    TEST_ASSERT_NOT_NULL(new_ptr);
    if (new_ptr == NULL) {
        TEST_FAIL_MESSAGE("allocator_realloc returned NULL for larger size.");
    }

    unsigned char* data = (unsigned char*)new_ptr;
    for (size_t i = 0; i < initial_size; i++) {
        TEST_ASSERT_EQUAL_HEX8(pattern, data[i]);
        if (data[i] != pattern) {
            TEST_FAIL_MESSAGE("Data corrupted after realloc.");
        }
    }

    allocator_free(new_ptr);
}

void test_allocator_realloc_shrink(void) {
    size_t initial_size = 128;
    size_t smaller_size = 32;
    unsigned char pattern = 0xCD;

    void* ptr = allocator_malloc(initial_size);
    TEST_ASSERT_NOT_NULL(ptr);

    memset(ptr, pattern, initial_size);

    void* new_ptr = allocator_realloc(ptr, smaller_size);
    TEST_ASSERT_NOT_NULL(new_ptr);
    if (new_ptr == NULL) {
        TEST_FAIL_MESSAGE("allocator_realloc returned NULL when shrinking.");
    }

    unsigned char* data = (unsigned char*)new_ptr;
    for (size_t i = 0; i < smaller_size; i++) {
        TEST_ASSERT_EQUAL_HEX8(pattern, data[i]);
        if (data[i] != pattern) {
            TEST_FAIL_MESSAGE("Data corrupted after shrinking realloc.");
        }
    }

    allocator_free(new_ptr);
}

void test_allocator_realloc_from_null(void) {
    size_t alloc_size = 64;
    void* new_ptr = allocator_realloc(NULL, alloc_size);
    TEST_ASSERT_NOT_NULL(new_ptr);
    if (new_ptr == NULL) {
        TEST_FAIL_MESSAGE("allocator_realloc(NULL) did not behave like malloc.");
    }
    allocator_free(new_ptr);
}

void test_allocator_free_null(void) {
    allocator_free(NULL);
    // If we reach here without a crash, test passes
    TEST_PASS_MESSAGE("No crash or error when freeing NULL.");
}

void test_allocator_calloc(void) {
    size_t nmemb = 10;
    size_t size = sizeof(int);
    void* ptr = allocator_calloc(nmemb, size);
    TEST_ASSERT_NOT_NULL(ptr);
    if (ptr == NULL) {
        TEST_FAIL_MESSAGE("allocator_calloc returned NULL.");
    }

    unsigned char* data = (unsigned char*)ptr;
    for (size_t i = 0; i < nmemb * size; i++) {
        TEST_ASSERT_EQUAL_HEX8(0x00, data[i]);
        if (data[i] != 0x00) {
            TEST_FAIL_MESSAGE("Memory not zero-initialized by calloc.");
        }
    }

    allocator_free(ptr);
}

void test_allocator_realloc_zero_size(void) {
    size_t alloc_size = 64;
    void* ptr = allocator_malloc(alloc_size);
    TEST_ASSERT_NOT_NULL(ptr);

    void* new_ptr = allocator_realloc(ptr, 0);
    TEST_ASSERT_NULL(new_ptr);
    if (new_ptr != NULL) {
        TEST_FAIL_MESSAGE("allocator_realloc with zero size should return NULL.");
    }
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
