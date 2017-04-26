#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "runner_internal.h"
#include "testmethod_internal.h"

struct TestMethod
{
    jmp_buf jmp;
    int ignore;
    int expectCrash;
};

SOLOCAL TestMethod *TestMethod_create(void)
{
    TestMethod* self = malloc(sizeof(TestMethod));
    self->ignore = 0;
    self->expectCrash = 0;
    return self;
}

SOLOCAL void TestMethod_destroy(TestMethod *self)
{
    free(self);
}

SOLOCAL jmp_buf *TestMethod_jmp(TestMethod *self)
{
    return &(self->jmp);
}

static void i_Test__stopIfNotIgnored(TestMethod *self)
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

SOEXPORT void i_Test__assertNotNull(TestMethod *self, const char *file, unsigned line, const char *pointerName,
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
        i_Test__stopIfNotIgnored(self);
    }
}

SOEXPORT void i_Test__assertRefEqual(TestMethod *self, const char *file, unsigned line, const char *actualName,
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
        i_Test__stopIfNotIgnored(self);
    }
}

SOEXPORT void i_Test__assertStrEqual(TestMethod *self, const char *file, unsigned line, const char *actualName,
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
        i_Test__stopIfNotIgnored(self);
    }
}

SOEXPORT void i_Test__assertIntEqual(TestMethod *self, const char *file, unsigned line, const char *actualName,
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
        i_Test__stopIfNotIgnored(self);
    }
}

SOEXPORT void i_Test__fail(TestMethod *self, const char *file, unsigned line, const char *message)
{
    if (message)
    {
        fprintf(testPipe, "0%s at %s:%u\n", message, file, line);
    }
    else
    {
        fprintf(testPipe, "0at %s:%u\n", file, line);
    }
    i_Test__stopIfNotIgnored(self);
}

SOEXPORT void i_Test__pass(TestMethod *self)
{
    fputs("1\n", testPipe);
    longjmp(self->jmp, 1);
}

SOEXPORT void i_Test__default(TestMethod *self, TestResultCode result)
{
    (void)self;
    fprintf(testPipe, "2%d\n", result);
}

SOEXPORT void i_Test__ignore(TestMethod *self, int num)
{
    self->ignore = num;
    fprintf(testPipe, "3%d\n", num);
}

SOEXPORT void i_Test__expectCrash(TestMethod *self)
{
    self->expectCrash = 1;
    fputs("4\n", testPipe);
}
