#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "runner_internal.h"
#include "test_internal.h"

struct Test
{
    jmp_buf jmp;
    int ignore;
    int expectCrash;
};

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

SOLOCAL jmp_buf *Test_jmp(Test *self)
{
    return &(self->jmp);
}

static void Test__stopIfNotIgnored(Test *self)
{
    if (self->ignore)
    {
        if (self->ignore > 0) --self->ignore;
    }
    else
    {
        longjmp(self->jmp, 1);
    }
}

SOEXPORT void Test__assertNotNull(Test *self, const char *file, unsigned line, const char *pointerName,
                                  const void *pointer, const char *message)
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
                                   const void *expected, const void *actual, const char *message)
{
    if (expected != actual)
    {
        if (message)
        {
            fprintf(testPipe, "0%s (%s expected: 0x%"PRIxPTR", actual: 0x%"PRIxPTR") at %s:%u\n",
                    message, actualName, (uintptr_t)expected, (uintptr_t)actual, file, line);
        }
        else
        {
            fprintf(testPipe, "0%s expected: 0x%"PRIxPTR", actual: 0x%"PRIxPTR" at %s:%u\n",
                    actualName, (uintptr_t)expected, (uintptr_t)actual, file, line);
        }
        Test__stopIfNotIgnored(self);
    }
}

SOEXPORT void Test__assertStrEqual(Test *self, const char *file, unsigned line, const char *actualName,
                                   const char *expected, const char *actual, const char *message)
{
    if (!expected) expected = "(null)";
    if (!actual) actual = "(null)";
    if (strcmp(expected,actual))
    {
        if (message)
        {
            fprintf(testPipe, "0%s (%s expected: \"%s\", actual: \"%s\") at %s:%u\n",
                    message, actualName, expected, actual, file, line);
        }
        else
        {
            fprintf(testPipe, "0%s expected: \"%s\", actual: \"%s\" at %s:%u\n",
                    actualName, expected, actual, file, line);
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
    longjmp(self->jmp, 1);
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
