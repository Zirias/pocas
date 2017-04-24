#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <pocas/core/list.h>
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
    void (*testMethod)(Test *) = (void (*)(Test *))
            (uintptr_t)Plugin_symbol(runningTest, testMethodSymbol);
    free(testMethodSymbol);

    if (!testMethod)
    {
        fprintf(testPipe, "0Error in test `%s': test method `%s' not found.\n",
                Plugin_id(runningTest), testMethodName);
        goto done;
    }

    void (*testInit)(Test *) = (void (*)(Test *))
            (uintptr_t)Plugin_symbol(runningTest, "pocastest__init");
    void (*testDone)(Test *) = (void (*)(Test *))
            (uintptr_t)Plugin_symbol(runningTest, "pocastest__done");

    Test *t = Test_create();
    if (!setjmp(*Test_jmp(t)))
    {
        if (testInit) testInit(t);
        testMethod(t);
    }
    if (testDone) testDone(t);
    Test_destroy(t);

done:
    Plugin_close(runningTest);
    fclose(testPipe);
    exit(EXIT_SUCCESS);
}

SOLOCAL void Runner_evaluateTest(const char *testMethodName,
                                 int exitCode, List *result)
{
    int expectCrash = 0;
    int ignore = 0;
    TestResult defaultResult = TestResult_UNKN;

    ++ntestMethods;

    ListIterator *i = List_iterator(result);
    while (ListIterator_moveNext(i))
    {
        const char *line = ListIterator_current(i);
        switch (line[0])
        {
            case '0':
                if (ignore)
                {
                    if (ignore > 0) --ignore;
                    TextColor_use(TextColor_YELLOW, ConsoleStream_ERROR);
                    fputs("   [FAIL] ", stderr);
                    TextColor_use(TextColor_BROWN, ConsoleStream_ERROR);
                    if (line[1] && line[1] != '\n')
                    {
                        fprintf(stderr, "%s: [ignored] %s", testMethodName, line+1);
                    }
                    else
                    {
                        fprintf(stderr, "%s [ignored]\n", testMethodName);
                    }
                    TextColor_use(TextColor_NORMAL, ConsoleStream_ERROR);
                    break;
                }
                else
                {
                    ++nfailed;
                    TextColor_use(TextColor_LIGHTRED, ConsoleStream_ERROR);
                    fputs("   [FAIL] ", stderr);
                    TextColor_use(TextColor_NORMAL, ConsoleStream_ERROR);
                    if (line[1] && line[1] != '\n')
                    {
                        fprintf(stderr, "%s: %s", testMethodName, line+1);
                    }
                    else
                    {
                        fprintf(stderr, "%s\n", testMethodName);
                    }
                    goto done;
                }

            case '1':
                if (expectCrash)
                {
                    ++nfailed;
                    TextColor_use(TextColor_LIGHTRED, ConsoleStream_ERROR);
                    fputs("   [FAIL] ", stderr);
                    TextColor_use(TextColor_NORMAL, ConsoleStream_ERROR);
                    fprintf(stderr, "%s: expected crash, but passed instead.\n",
                            testMethodName);
                }
                else
                {
                    ++npassed;
                    TextColor_use(TextColor_LIGHTGREEN, ConsoleStream_ERROR);
                    fputs("   [PASS] ", stderr);
                    TextColor_use(TextColor_NORMAL, ConsoleStream_ERROR);
                    if (line[1] && line[1] != '\n')
                    {
                        fprintf(stderr, "%s: %s", testMethodName, line+1);
                    }
                    else
                    {
                        fprintf(stderr, "%s\n", testMethodName);
                    }
                }
                goto done;

            case '2':
                defaultResult = atoi(line+1);
                break;

            case '3':
                ignore = atoi(line+1);
                break;

            case '4':
                expectCrash = 1;
        }
    }

    if (exitCode != EXIT_SUCCESS)
    {
        if (expectCrash)
        {
            ++npassed;
            TextColor_use(TextColor_LIGHTGREEN, ConsoleStream_ERROR);
            fputs("   [PASS] ", stderr);
            TextColor_use(TextColor_NORMAL, ConsoleStream_ERROR);
            fprintf(stderr, "%s: expectedly failed to run (exit code was %d)\n",
                    testMethodName, exitCode);
        }
        else
        {
            ++nfailed;
            TextColor_use(TextColor_LIGHTRED, ConsoleStream_ERROR);
            fprintf(stderr, "   [CRSH] %s: failed to run (exit code was %d)\n",
                    testMethodName, exitCode);
            TextColor_use(TextColor_NORMAL, ConsoleStream_ERROR);
        }
        goto done;
    }

    switch (defaultResult)
    {
        case TestResult_UNKN:
            TextColor_use(TextColor_YELLOW, ConsoleStream_ERROR);
            fputs("   [UNKN] ", stderr);
            TextColor_use(TextColor_BROWN, ConsoleStream_ERROR);
            fprintf(stderr, "%s: no test result\n", testMethodName);
            TextColor_use(TextColor_NORMAL, ConsoleStream_ERROR);
            break;
        case TestResult_FAIL:
            ++nfailed;
            TextColor_use(TextColor_LIGHTRED, ConsoleStream_ERROR);
            fputs("   [FAIL] ", stderr);
            TextColor_use(TextColor_BROWN, ConsoleStream_ERROR);
            fprintf(stderr, "%s: no test result, defaulted to FAIL\n", testMethodName);
            TextColor_use(TextColor_NORMAL, ConsoleStream_ERROR);
            break;
        case TestResult_PASS:
            ++npassed;
            TextColor_use(TextColor_LIGHTGREEN, ConsoleStream_ERROR);
            fputs("   [PASS] ", stderr);
            TextColor_use(TextColor_BROWN, ConsoleStream_ERROR);
            fprintf(stderr, "%s: no test result, defaulted to PASS\n", testMethodName);
            TextColor_use(TextColor_NORMAL, ConsoleStream_ERROR);
            break;
    }

done:
    List_destroy(result);
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
        fprintf(stderr, "failed/crashed: %d", nfailed);
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
