#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>

#include "runner_internal.h"
#include "test_internal.h"

SOLOCAL Test *Test_create(void)
{
    Test* self = malloc(sizeof(Test));
    self->ignore = 0;
    self->expectCrash = 0;
    return self;
}

SOLOCAL void Test_destroy(Test *self)
{
    free(self);
}

static void Test__stopIfNotIgnored(Test *self)
{
    if (self->ignore)
    {
        if (self->ignore > 0) --self->ignore;
    }
    else
    {
        Test_destroy(self);
        Runner_stopTest();
    }
}

SOEXPORT void Test__assertNotNull(Test *self, const char *file, unsigned line, const char *pointerName,
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
        Test__stopIfNotIgnored(self);
    }
}

SOEXPORT void Test__assertRefEqual(Test *self, const char *file, unsigned line, const char *actualName,
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
        Test__stopIfNotIgnored(self);
    }
}

SOEXPORT void Test__assertIntEqual(Test *self, const char *file, unsigned line, const char *actualName,
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
        Test__stopIfNotIgnored(self);
    }
}

SOEXPORT void Test__fail(Test *self, const char *file, unsigned line, const char *message)
{
    if (message)
    {
        fprintf(testPipe, "0%s at %s:%u\n", message, file, line);
    }
    else
    {
        fprintf(testPipe, "0at %s:%u\n", file, line);
    }
    Test__stopIfNotIgnored(self);
}

SOEXPORT void Test__pass(Test *self)
{
    fputs("1\n", testPipe);
    Test_destroy(self);
    Runner_stopTest();
}

SOEXPORT void Test__default(Test *self, TestResult result)
{
    (void)self;
    fprintf(testPipe, "2%d\n", result);
}

SOEXPORT void Test__ignore(Test *self, int num)
{
    self->ignore = num;
    fprintf(testPipe, "3%d\n", num);
}

SOEXPORT void Test__expectCrash(Test *self)
{
    self->expectCrash = 1;
    fputs("4\n", testPipe);
}
