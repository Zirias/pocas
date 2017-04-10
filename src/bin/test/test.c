#include <inttypes.h>
#include <stdio.h>

#include "runner_internal.h"
#include "test_internal.h"

SOEXPORT void Test__assertNotNull(const char *file, unsigned line, const char *pointerName,
                                  void *pointer, const char *message)
{
    if (!pointer)
    {
        if (message)
        {
            fprintf(testPipe, "0%s (%s was NULL) at %s:%u\n", message, pointerName, file, line);
        }
        else
        {
            fprintf(testPipe, "0%s was NULL at %s:%u\n", pointerName, file, line);
        }
    }
}

SOEXPORT void Test__assertRefEqual(const char *file, unsigned line, const char *actualName,
                                   void *expected, void *actual, const char *message)
{
    if (expected != actual)
    {
        if (message)
        {
            fprintf(testPipe, "0%s (%s expected: %"PRIxPTR", actual: %"PRIxPTR") at %s:%u\n",
                    message, actualName, (uintptr_t)expected, (uintptr_t)actual, file, line);
        }
        else
        {
            fprintf(testPipe, "0%s expected: %"PRIxPTR", actual: %"PRIxPTR" at %s:%u\n",
                    actualName, (uintptr_t)expected, (uintptr_t)actual, file, line);
        }
    }
}

SOEXPORT void Test__assertIntEqual(const char *file, unsigned line, const char *actualName,
                                   long expected, long actual, const char *message)
{
    if (expected != actual)
    {
        if (message)
        {
            fprintf(testPipe, "0%s (%s expected: %ld, actual: %ld) at %s:%u\n",
                    message, actualName, expected, actual, file, line);
        }
        else
        {
            fprintf(testPipe, "0%s expected: %ld, actual: %ld at %s:%u\n",
                    actualName, expected, actual, file, line);
        }
    }
}

SOEXPORT void Test__fail(const char *file, unsigned line, const char *message)
{
    if (message)
    {
        fprintf(testPipe, "0%s at %s:%u\n", message, file, line);
    }
    else
    {
        fprintf(testPipe, "0at %s:%u\n", file, line);
    }
}

SOEXPORT void Test_pass()
{
    fputs("1\n", testPipe);
}

SOEXPORT void Test_default(TestResult result)
{
    fprintf(testPipe, "2%d\n", result);
}

SOEXPORT void Test_ignore(int num)
{
    fprintf(testPipe, "3%d\n", num);
}

SOEXPORT void Test_expectCrash(void)
{
    fputs("4\n", testPipe);
}
