/* ============================================
    Unity Project - Unity Test Framework
    Version 2.5.2
    Copyright (c) 2007-2020
    Mike Karlesky, Mark VanderVoord, Greg Williams
    [Released under MIT License. Please refer to LICENSE file]
=============================================== */

#ifndef UNITY_H
#define UNITY_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <stddef.h>

/* Unity Configuration
 * -------------------
 * Unity expects these #defines to be set:
 *
 * #define UNITY_OUTPUT_CHAR(a) putchar(a)
 * #define UNITY_FLUSH_CALL() fflush(stdout)
 *
 * If any of the standard functions aren't working for you (or you
 * want to route Unity's output to a custom location), these can
 * be overridden.
 *
 * If you need specialized test formatting, define these:
 *
 * #define UNITY_PRINT_EOL()       <your_end_of_line_function>
 * #define UNITY_PRINT_EOL_PUBLIC()  <your_end_of_line_function_for_public_output>
 *
 * If you wish to use floating point, define UNITY_INCLUDE_FLOAT. This may cause
 * extra overhead if not using floats. Also, you can set these options to handle
 * how Unity prints floats (precision, etc.)
 *
 * If you are in an environment that supports setjmp/longjmp for recovering
 * from assert failures (most environments), define UNITY_LONGJMP. If not, the tests
 * will just stop on the first failure.
 *
 * For more options and details, see:
 * http://www.throwtheswitch.org/white-papers/unity#configuration
 */

typedef void (*UnityTestFunction)(void);

typedef enum
{
    UNITY_DISPLAY_STYLE_INT,
    UNITY_DISPLAY_STYLE_INT8,
    UNITY_DISPLAY_STYLE_INT16,
    UNITY_DISPLAY_STYLE_INT32,
    UNITY_DISPLAY_STYLE_UINT,
    UNITY_DISPLAY_STYLE_UINT8,
    UNITY_DISPLAY_STYLE_UINT16,
    UNITY_DISPLAY_STYLE_UINT32,
    UNITY_DISPLAY_STYLE_HEX8,
    UNITY_DISPLAY_STYLE_HEX16,
    UNITY_DISPLAY_STYLE_HEX32,
    UNITY_DISPLAY_STYLE_UNKNOWN
} UNITY_DISPLAY_STYLE_T;

void UnityBegin(const char* filename);
int UnityEnd(void);

void UnityDefaultTestRun(UnityTestFunction Func, const char* FuncName, const int FuncLineNum);
void UnityConcludeTest(void);

void UnityFail(const char* message, const int lineNumber);
void UnityIgnore(const char* message, const int lineNumber);

/* These macros are used by the test runner and test files to identify and run tests. */
#define TEST_FAIL_LINE(line, message)    UnityFail((message), (line))
#define TEST_IGNORE_LINE(line, message)  UnityIgnore((message), (line))

#define TEST_FAIL(message)               TEST_FAIL_LINE(__LINE__, message)
#define TEST_IGNORE(message)             TEST_IGNORE_LINE(__LINE__, message)

#define RUN_TEST(func) \
    do { \
        UnityDefaultTestRun(func, #func, __LINE__); \
    } while (0)

/* Internal Use Only */
extern int UnityVerbosity;
extern int UnityFailCount;
extern int UnityIgnoreCount;
extern const char* UnityCurrentTestName;
extern int UnityCurrentTestLineNumber;
extern int UnityCurrentTestFailed;
extern int UnityCurrentTestIgnored;

#ifndef UNITY_BOOL
#define UNITY_BOOL int
#endif

#ifndef UNITY_OUTPUT_CHAR
#include <stdio.h>
#define UNITY_OUTPUT_CHAR(a) putchar(a)
#endif

#ifndef UNITY_FLUSH_CALL
#include <stdio.h>
#define UNITY_FLUSH_CALL() fflush(stdout)
#endif

/* If we have setjmp/longjmp support */
#include <setjmp.h>
extern jmp_buf UnityAbortFrame;
#define TEST_PROTECT() (setjmp(UnityAbortFrame) == 0)
#define TEST_ABORT()   longjmp(UnityAbortFrame, 1)

/* If param is not used, ignore it to remove compiler warnings */
#ifndef UNUSED
#define UNUSED(x) (void)(x)
#endif

#define UNITY_BEGIN() UnityBegin(__FILE__)
#define UNITY_END()   UnityEnd()

#ifdef __cplusplus
}
#endif

#endif /* UNITY_H */
