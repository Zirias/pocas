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
        Runner_stopTest();
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
        Runner_stopTest();
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
        Runner_stopTest();
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
    Runner_stopTest();
}

SOEXPORT void Test_pass()
{
    fputs("1\n", testPipe);
    Runner_stopTest();
}
