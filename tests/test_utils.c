/**
 * @file test_utils.c
 * @brief Unit tests for utility functions and macros provided by utils.h and utils.c.
 *
 * This test file uses the Unity testing framework to verify the correctness of:
 *  - Alignment macros (ALIGN_UP, PTR_ALIGN_UP)
 *  - Logging functions (log_debug, log_error)
 *
 * While testing logging output is more about verifying no errors occur during 
 * usage, the alignment macros can be rigorously checked.
 *
 * Author: Ameed Othman
 * Date: 05/12/2024
 */

#include "unity.h"
#include "utils.h"
#include <stdlib.h>
#include <stdint.h>

/* -------------------------------------------------------------------------
 * Test Cases: Alignment
 * ------------------------------------------------------------------------- */

/**
 * @brief Tests the ALIGN_UP macro on a variety of sizes and alignments.
 */
void test_ALIGN_UP(void) {
    // Test aligning sizes to 16
    TEST_ASSERT_EQUAL_UINT32(16, ALIGN_UP(1, 16));
    TEST_ASSERT_EQUAL_UINT32(16, ALIGN_UP(15, 16));
    TEST_ASSERT_EQUAL_UINT32(16, ALIGN_UP(16, 16));
    TEST_ASSERT_EQUAL_UINT32(32, ALIGN_UP(17, 16));
    TEST_ASSERT_EQUAL_UINT32(32, ALIGN_UP(31, 16));
    TEST_ASSERT_EQUAL_UINT32(32, ALIGN_UP(32, 16));

    // Test aligning to 8
    TEST_ASSERT_EQUAL_UINT32(8, ALIGN_UP(1, 8));
    TEST_ASSERT_EQUAL_UINT32(8, ALIGN_UP(7, 8));
    TEST_ASSERT_EQUAL_UINT32(8, ALIGN_UP(8, 8));
    TEST_ASSERT_EQUAL_UINT32(16, ALIGN_UP(9, 8));
}

/**
 * @brief Tests the PTR_ALIGN_UP macro on a variety of pointers and alignments.
 *
 * We simulate pointers using integer addresses and then cast back to void*.
 */
void test_PTR_ALIGN_UP(void) {
    uintptr_t base = 100;
    // Align up to 16
    void* ptr = (void*)base;
    void* aligned_ptr = PTR_ALIGN_UP(ptr, 16);
    TEST_ASSERT_TRUE(((uintptr_t)aligned_ptr % 16) == 0);

    // Check a few more alignments
    for (uintptr_t addr = 1; addr < 1000; addr += 37) {
        void* test_ptr = (void*)addr;
        void* aligned16 = PTR_ALIGN_UP(test_ptr, 16);
        TEST_ASSERT_TRUE(((uintptr_t)aligned16 % 16) == 0);

        void* aligned32 = PTR_ALIGN_UP(test_ptr, 32);
        TEST_ASSERT_TRUE(((uintptr_t)aligned32 % 32) == 0);

        void* aligned64 = PTR_ALIGN_UP(test_ptr, 64);
        TEST_ASSERT_TRUE(((uintptr_t)aligned64 % 64) == 0);
    }
}

/* -------------------------------------------------------------------------
 * Test Cases: Logging
 *
 * Testing log_debug and log_error is tricky since they print to stderr.
 * Here, we merely ensure they can be called without error. One could consider
 * redirecting stderr and checking output, but that may be more complex than 
 * necessary for a simple utility test.
 * ------------------------------------------------------------------------- */

/**
 * @brief Tests calling log_debug without causing any errors or crashes.
 */
void test_log_debug(void) {
    // This test passes if no crash and no error occur.
    // We cannot easily capture stderr output here, but at least ensure no runtime issues.
    log_debug("This is a debug message for testing purposes, value=%d", 42);
    TEST_PASS_MESSAGE("log_debug called successfully.");
}

/**
 * @brief Tests calling log_error without causing any errors or crashes.
 */
void test_log_error(void) {
    // Similar to log_debug, just ensure no crash.
    log_error("This is an error message for testing purposes, value=%d", -1);
    TEST_PASS_MESSAGE("log_error called successfully.");
}

/* -------------------------------------------------------------------------
 * Test Runner
 *
 * Defines main() so this test file can be invoked standalone or integrated 
 * into a larger test suite.
 * ------------------------------------------------------------------------- */

int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_ALIGN_UP);
    RUN_TEST(test_PTR_ALIGN_UP);
    RUN_TEST(test_log_debug);
    RUN_TEST(test_log_error);

    return UNITY_END();
}
