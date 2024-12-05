/* ============================================
    Unity Project - Unity Test Framework
    Version 2.5.2
    Copyright (c) 2007-2020
    Mike Karlesky, Mark VanderVoord, Greg Williams
    [Released under MIT License. Please refer to LICENSE file]
=============================================== */

#include "unity.h"
#include <stdio.h>
#include <string.h>

/* If we have setjmp/longjmp, we will run test suites using this technique */
#include <setjmp.h>
#include <stdlib.h>

/* These externs are used to return information to the Unity caller. */
int UnityVerbosity = 0;
int UnityFailCount = 0;
int UnityIgnoreCount = 0;
UNITY_LINE_TYPE UnityCurrentTestLineNumber;
const char* UnityCurrentTestName;
UNITY_BOOL UnityCurrentTestIgnored;
UNITY_BOOL UnityCurrentTestFailed;

/* The following pointers are used for directing output. */
#ifndef UNITY_OUTPUT_CHAR
#define UNITY_OUTPUT_CHAR(a) putchar(a)
#endif

#ifndef UNITY_FLUSH_CALL
#define UNITY_FLUSH_CALL() fflush(stdout)
#endif

static void UnityPrintFail(void);
static void UnityPrintOk(void);

/*-----------------------------------------------
 * Assertion & Control Helpers
 *-----------------------------------------------*/

void UnityFail(const char* msg, const UNITY_LINE_TYPE lineNumber)
{
    UNITY_SKIP_EXECUTION;

    UnityTestResultsBegin(UnityCurrentTestName);
    UnityPrint(":FAIL:");
    UnityPrintNumber(lineNumber);
    if (msg != NULL)
    {
        UnityPrint(":");
        UnityPrint(msg);
    }
    UNITY_OUTPUT_CHAR('\n');
    UnityCurrentTestFailed = 1;
    UnityFailCount++;
    LONGJMP(Unity.AbortFrame, 1);
}

void UnityIgnore(const char* msg, const UNITY_LINE_TYPE lineNumber)
{
    UNITY_SKIP_EXECUTION;

    UnityTestResultsBegin(UnityCurrentTestName);
    UnityPrint(":IGNORE:");
    UnityPrintNumber(lineNumber);
    if (msg != NULL)
    {
        UnityPrint(":");
        UnityPrint(msg);
    }
    UNITY_OUTPUT_CHAR('\n');
    UnityCurrentTestIgnored = 1;
    UnityIgnoreCount++;
    LONGJMP(Unity.AbortFrame, 1);
}

/*-----------------------------------------------
 * Test Output
 *-----------------------------------------------*/

void UnityBegin(const char* filename)
{
    UnityFailCount = 0;
    UnityIgnoreCount = 0;
    Unity.CurrentTestName = NULL;
    Unity.CurrentTestLineNumber = 0;
    Unity.CurrentTestIgnored = 0;
    Unity.CurrentTestFailed = 0;
    Unity.NumberOfTests = 0;
    UNITY_OUTPUT_CHAR('\n');
    if (filename != NULL)
    {
        UnityPrint("------");
        UnityPrint(filename);
        UnityPrint("------");
        UNITY_OUTPUT_CHAR('\n');
    }
}

int UnityEnd(void)
{
    UnityPrint("=======================\n");
    UnityPrint("Tests Passed : ");
    UnityPrintNumber(Unity.NumberOfTests - UnityFailCount - UnityIgnoreCount);
    UNITY_OUTPUT_CHAR('\n');
    UnityPrint("Tests Failed : ");
    UnityPrintNumber(UnityFailCount);
    UNITY_OUTPUT_CHAR('\n');
    UnityPrint("Tests Ignored: ");
    UnityPrintNumber(UnityIgnoreCount);
    UNITY_OUTPUT_CHAR('\n');
    if (UnityFailCount == 0U)
    {
        UnityPrint("ALL TESTS PASSED\n");
    }
    else
    {
        UnityPrint("SOME TESTS FAILED\n");
    }
    return UnityFailCount;
}

void UnityConcludeTest(void)
{
    if (Unity.CurrentTestIgnored)
    {
        /* do nothing */
    }
    else if (!Unity.CurrentTestFailed)
    {
        UnityTestResultsBegin(Unity.CurrentTestName);
        UnityPrint(":PASS\n");
    }

    UNITY_FLUSH_CALL();
}

void UnityTestResultsBegin(const char* name)
{
    Unity.NumberOfTests++;
    Unity.CurrentTestFailed = 0;
    Unity.CurrentTestIgnored = 0;
    Unity.CurrentTestName = name;
    Unity.CurrentTestLineNumber = 0;
}

void UnityTestResultsFailBegin(const UNITY_LINE_TYPE line)
{
    Unity.CurrentTestLineNumber = line;
}

static void UnityPrintOk(void)
{
    UnityPrint("OK");
}

static void UnityPrintFail(void)
{
    UnityPrint("FAIL");
}

/*-----------------------------------------------
 * Printing Helper Functions
 *-----------------------------------------------*/

void UnityPrint(const char* string)
{
    if (string != NULL)
    {
        while (*string)
        {
            UNITY_OUTPUT_CHAR(*string);
            string++;
        }
    }
}

/*-----------------------------------------------
 * Numeric Print Helpers
 *-----------------------------------------------*/

void UnityPrintNumber(const UNITY_INT number)
{
    char buffer[32];
    snprintf(buffer, sizeof(buffer), "%d", (int)number);
    UnityPrint(buffer);
}

/*-----------------------------------------------
 *  Test Running
 *-----------------------------------------------*/

void UnityDefaultTestRun(UnityTestFunction Func, const char* FuncName, const int FuncLineNum)
{
    if (TEST_PROTECT())
    {
        Func();
    }
    UnityConcludeTest();
}

int TEST_PROTECT(void)
{
    return SETJMP(Unity.AbortFrame) == 0;
}

void TEST_ABORT(void)
{
    LONGJMP(Unity.AbortFrame, 1);
}
