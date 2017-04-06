#include <stdlib.h>
#include <string.h>

#include <pocas/core/plugin.h>

#include "test_internal.h"
#include "runner_internal.h"

SOLOCAL FILE *testPipe;
SOLOCAL Plugin *runningTest;

void Runner_runTest(const char *testMethodName)
{
    char *testMethodSymbol = malloc(19 + strlen(testMethodName));
    strcpy(testMethodSymbol, "pocastest__method_");
    strcpy(testMethodSymbol + 18, testMethodName);
    void (*testMethod)(void) = (void (*)(void))
            (uintptr_t)Plugin_symbol(runningTest, testMethodSymbol);
    free(testMethodSymbol);

    if (!testMethod)
    {
        fprintf(testPipe, "0Error in test `%s': test method `%s' not found.\n",
                Plugin_id(runningTest), testMethodName);
        Runner_stopTest();
    }

    testMethod();
    Runner_stopTest();
}

void Runner_stopTest(void)
{
    Plugin_close(runningTest);
    fclose(testPipe);
    exit(EXIT_SUCCESS);
}
