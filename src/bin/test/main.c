#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <pocas/core/list.h>
#include <pocas/core/plugin.h>
#include <pocas/core/textcolor.h>

#include "test_internal.h"
#include "runner_internal.h"

int main(int argc, char **argv)
{
    int i = 0;

    if (argc < 2) goto usage;
    if (!strncmp("-g", argv[1], 2))
    {
        if (strlen(argv[1]) > 2)
        {
            if (argc < 3) goto usage;
            i = 1;
            gdb = argv[1] + 2;
        }
        else
        {
            if (argc < 4) goto usage;
            i = 2;
            gdb = argv[i];
        }
    }
    else
    {
        gdb = 0;
    }

    Runner_runMain(argc - i, argv + i);

    List *tests = Plugin_loadDir(argv[++i], TEST_PLUGIN_ID);
    while (++i < argc)
    {
        List *moreTests = Plugin_loadDir(argv[i], TEST_PLUGIN_ID);
        if (tests && moreTests)
        {
            List *newTests = List_concat(tests, moreTests);
            List_destroy(tests);
            List_destroy(moreTests);
            tests = newTests;
        }
        else if (moreTests)
        {
            tests = moreTests;
        }
    }

    if (!tests)
    {
        fputs("POCAS Testrunner error: no tests found!\n", stderr);
        return EXIT_FAILURE;
    }

    ListIterator *testIterator = List_iterator(tests);
    while (ListIterator_moveNext(testIterator))
    {
        Plugin *test = ListIterator_current(testIterator);
        TextColor_use(TextColor_LIGHTCYAN, ConsoleStream_ERROR);
        fprintf(stderr, "   [TEST] %s\n", Plugin_id(test));
        TextColor_use(TextColor_NORMAL, ConsoleStream_ERROR);
        const char **testmethods = Plugin_symbol(test, TEST_METHODS);
        do
        {
            Runner_launchTest(argv[0], test, *testmethods);
        } while (*++testmethods);
        Plugin_close(test);
    }

    ListIterator_destroy(testIterator);
    List_destroy(tests);
    return Runner_evaluateFinal();

usage:
    fprintf(stderr, "POCAS Testrunner\n"
                    "Usage: %s [-g {gdb path}] path [path ...]\n"
                    "\n"
                    "for running all tests in the given paths.\n"
                    "use -g for running each test in a GDB debugging session.",
            argv[0]);
    return EXIT_FAILURE;
}
