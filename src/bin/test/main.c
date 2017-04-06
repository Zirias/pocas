#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <pocas/core/list.h>
#include <pocas/core/plugin.h>

#include "test_internal.h"
#include "runner_internal.h"

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        fprintf(stderr, "POCAS Testrunner\n"
                        "Usage: %s path [path ...]\n"
                        "for running all tests in the given paths.\n",
                argv[0]);
        return EXIT_FAILURE;
    }

    Runner_runMain(argc, argv);

    List *tests = Plugin_loadDir(argv[1], TEST_PLUGIN_ID);
    for (int i = 2; i < argc; ++i)
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
        printf("[Test] %s\n", Plugin_id(test));
        const char **testmethods = Plugin_symbol(test, TEST_METHODS);
        do
        {
            Runner_launchTest(argv[0], test, *testmethods);
        } while (*++testmethods);
    }
}
