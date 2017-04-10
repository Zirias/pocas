#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <pocas/core/list.h>
#include <pocas/core/plugin.h>
#include <pocas/core/textcolor.h>

#include "preproc_internal.h"
#include "runner_internal.h"
#include "test_internal.h"

int main(int argc, char **argv)
{
    int i = 0;

    if (argc < 2) goto usage;

    if (!strncmp("-p", argv[1], 2))
    {
        const char *inFilename = 0;
        const char *outFilename = 0;
        if (strlen(argv[1]) > 2)
        {
            if (argv[1][2] != 'o') goto usage;
            if (strlen(argv[1]) > 3) outFilename = argv[1] + 3;
        }
        else
        {
            if (argc < 3) goto usage;
            i = 2;
            if (!strncmp("-o", argv[2], 2))
            {
                if (strlen(argv[2]) > 2)
                {
                    if (argc < 4) goto usage;
                    i = 3;
                    outFilename = argv[2] + 2;
                }
                else
                {
                    if (argc < 5) goto usage;
                    i = 4;
                    outFilename = argv[3];
                }
            }
        }
        inFilename = argv[i];
        return preprocess(inFilename, outFilename) ? EXIT_SUCCESS : EXIT_FAILURE;
    }

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
        if (testmethods) do
        {
            Runner_launchTest(argv[0], test, *testmethods);
        } while (*++testmethods);
        Plugin_close(test);
    }

    ListIterator_destroy(testIterator);
    List_destroy(tests);
    return Runner_evaluateFinal();

usage:
    fprintf(stderr, "\nPOCAS Testrunner\n"
                    "\n"
                    "USAGE: %s [-g {gdb path}] path [path ...]\n"
                    "for running all tests in the given paths.\n"
                    "use -g for running each test in a GDB debugging session.\n"
                    "\n"
                    "USAGE: %s -p [-o output.c] test.c\n"
                    "to preprocess test.c for compilation.\n",
            argv[0], argv[0]);
    return EXIT_FAILURE;
}
