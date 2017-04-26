#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <pocas/core/list.h>
#include <pocas/core/plugin.h>
#include <pocas/core/textcolor.h>

#include "testmethod_internal.h"
#include "runner_internal.h"

SOLOCAL FILE *testPipe;
SOLOCAL Plugin *runningTest;
SOLOCAL const char *gdb;
SOLOCAL char *exeName;

SOLOCAL void Runner_runTest(const char *testMethodName)
{
    char *testMethodSymbol = malloc(19 + strlen(testMethodName));
    strcpy(testMethodSymbol, "pocastest__method_");
    strcpy(testMethodSymbol + 18, testMethodName);
    void (*testMethod)(TestMethod *) = (void (*)(TestMethod *))
            (uintptr_t)Plugin_symbol(runningTest, testMethodSymbol);
    free(testMethodSymbol);

    if (!testMethod)
    {
        fprintf(testPipe, "0Error in test `%s': test method `%s' not found.\n",
                Plugin_id(runningTest), testMethodName);
        goto done;
    }

    void (*testInit)(TestMethod *) = (void (*)(TestMethod *))
            (uintptr_t)Plugin_symbol(runningTest, "pocastest__init");
    void (*testDone)(TestMethod *) = (void (*)(TestMethod *))
            (uintptr_t)Plugin_symbol(runningTest, "pocastest__done");

    TestMethod *t = TestMethod_create();
    if (!setjmp(*TestMethod_jmp(t)))
    {
        if (testInit) testInit(t);
        testMethod(t);
    }
    if (testDone) testDone(t);
    TestMethod_destroy(t);

done:
    Plugin_close(runningTest);
    fclose(testPipe);
    exit(EXIT_SUCCESS);
}
