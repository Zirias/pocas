#include <stdlib.h>
#include <string.h>

#include <pocas/core/plugin.h>
#include <pocas/core/textcolor.h>

#include "test_internal.h"
#include "runner_internal.h"

SOLOCAL FILE *testPipe;
SOLOCAL Plugin *runningTest;
SOLOCAL const char *gdb;

static unsigned ntestMethods = 0;
static unsigned nfailed = 0;
static unsigned npassed = 0;

SOLOCAL void Runner_runTest(const char *testMethodName)
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

SOLOCAL void Runner_stopTest(void)
{
    Plugin_close(runningTest);
    fclose(testPipe);
    exit(EXIT_SUCCESS);
}

SOLOCAL void Runner_evaluateTest(const char *testMethodName,
                                 int exitCode, const char *result)
{
    ++ntestMethods;
    if (exitCode != EXIT_SUCCESS)
    {
        if (!result)
        {
            TextColor_use(TextColor_RED, ConsoleStream_ERROR);
            fprintf(stderr, "   [CRSH] %s: failed to run (exit code was %d)\n",
                    testMethodName, exitCode);
            TextColor_use(TextColor_NORMAL, ConsoleStream_ERROR);
            ++nfailed;
            return;
        }
    }

    if (!result)
    {
        TextColor_use(TextColor_YELLOW, ConsoleStream_ERROR);
        fprintf(stderr, "   [UNKN] %s: no test result\n", testMethodName);
        TextColor_use(TextColor_NORMAL, ConsoleStream_ERROR);
    }
    else
    {
        if (result[0] == '1')
        {
            ++npassed;
            TextColor_use(TextColor_LIGHTGREEN, ConsoleStream_ERROR);
            if (result[1] && result[1] != '\n')
            {
                fprintf(stderr, "   [PASS] %s: %s", testMethodName, result+1);
            }
            else
            {
                fprintf(stderr, "   [PASS] %s\n", testMethodName);
            }
            TextColor_use(TextColor_NORMAL, ConsoleStream_ERROR);
        }
        else
        {
            ++nfailed;
            TextColor_use(TextColor_LIGHTRED, ConsoleStream_ERROR);
            if (result[1] && result[1] != '\n')
            {
                fprintf(stderr, "   [FAIL] %s: %s", testMethodName, result+1);
            }
            else
            {
                fprintf(stderr, "   [FAIL] %s\n", testMethodName);
            }
            TextColor_use(TextColor_NORMAL, ConsoleStream_ERROR);
        }
    }
}

SOLOCAL int Runner_evaluateFinal(void)
{
    TextColor_use(TextColor_LIGHTCYAN, ConsoleStream_ERROR);
    fprintf(stderr, "   [TRES] Tests run: %d", ntestMethods);
    TextColor_use(TextColor_NORMAL, ConsoleStream_ERROR);
    if (npassed)
    {
        fputs(", ", stderr);
        TextColor_use(TextColor_LIGHTGREEN, ConsoleStream_ERROR);
        fprintf(stderr, "passed: %d", npassed);
        TextColor_use(TextColor_NORMAL, ConsoleStream_ERROR);
    }
    if (nfailed)
    {
        fputs(", ", stderr);
        TextColor_use(TextColor_LIGHTRED, ConsoleStream_ERROR);
        fprintf(stderr, "failed: %d", nfailed);
        TextColor_use(TextColor_NORMAL, ConsoleStream_ERROR);
    }
    if (ntestMethods - npassed - nfailed)
    {
        fputs(", ", stderr);
        TextColor_use(TextColor_YELLOW, ConsoleStream_ERROR);
        fprintf(stderr, "unknown: %d", ntestMethods - npassed - nfailed);
        TextColor_use(TextColor_NORMAL, ConsoleStream_ERROR);
    }
    fputc('\n', stderr);

    return npassed == ntestMethods ? EXIT_SUCCESS : EXIT_FAILURE;
}
