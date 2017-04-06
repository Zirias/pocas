#include <stdio.h>

#include "runner_internal.h"
#include "test_internal.h"

SOEXPORT void Test_assertNotNull(void *pointer, const char *message)
{
    if (!pointer) Test_fail(message);
}

SOEXPORT void Test_assertRefEqual(void *expected, void *actual, const char *message)
{
    if (expected != actual) Test_fail(message);
}

SOEXPORT void Test_assertRefNotEqual(void *expected, void *actual, const char *message)
{
    if (expected == actual) Test_fail(message);
}

SOEXPORT void Test_assertIntEqual(int expected, int actual, const char *message)
{
    if (expected != actual) Test_fail(message);
}

SOEXPORT void Test_assertIntNotEqual(int expected, int actual, const char *message)
{
    if (expected == actual) Test_fail(message);
}

SOEXPORT void Test_fail(const char *message)
{
    fprintf(testPipe, "0%s\n", message);
    Runner_stopTest();
}

SOEXPORT void Test_pass()
{
    fputs("1\n", testPipe);
    Runner_stopTest();
}
