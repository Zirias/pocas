#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <pocas/core/cmdline.h>
#include <pocas/core/list.h>
#include <pocas/core/plugin.h>
#include <pocas/core/string.h>
#include <pocas/core/textcolor.h>

#include "gui_internal.h"
#include "testclass_internal.h"
#include "testcase_internal.h"
#include "preproc_internal.h"
#include "runner_internal.h"
#include "testmethod_internal.h"

#define OPTID_PREPROCESS 0x101
#define OPTID_OUTPUT 0x102
#define OPTID_GDB 0x103
#define OPTID_INTERACTIVE 0x104

struct result
{
    int run;
    int failed;
    int passed;
    int unknown;
};

static void *pluginToTestClass(const void *plugin, void *args)
{
    (void)args;
    return TestClass_create((Plugin *)plugin);
}

static void deleteTestClass(void *testclass)
{
    TestClass_destroy(testclass);
}

static void consoleResultHandler(const TestCase *testCase,
        const TestResult *result, void *args)
{
    (void)args;
    const char *testname = TestCase_method(testCase);
    const char *classname = Plugin_id(TestClass_plugin(TestCase_class(testCase)));
    const char *message = TestResult_message(result);

    ListIterator *igni = TestResult_ignoredMessages(result);
    if (igni) while (ListIterator_moveNext(igni))
    {
        TextColor_fprintf(stderr,
                TCS_YELLOW "   [IGN]  " TCS_BROWN "%s::%s %s" TCS_NORMAL "\n",
                classname, testname,
                (const char *)ListIterator_current(igni));
    }
    ListIterator_destroy(igni);

    switch (TestResult_code(result))
    {
    case TRC_NONE:
        break;
    case TRC_CRSH:
        TextColor_fprintf(stderr, TCS_LIGHTRED "   [CRSH] %s::%s %s"
                TCS_NORMAL "\n", classname, testname, message);
        break;
    case TRC_FAIL:
        TextColor_fprintf(stderr, TCS_LIGHTRED "   [FAIL] " TCS_NORMAL
                "%s::%s %s\n", classname, testname, message);
        break;
    case TRC_PASS:
        TextColor_fprintf(stderr, TCS_LIGHTGREEN "   [PASS] " TCS_NORMAL
                "%s::%s %s\n", classname, testname, message);
        break;
    case TRC_UNKN:
        TextColor_fprintf(stderr, TCS_YELLOW "   [UNKN] " TCS_NORMAL
                "%s::%s %s\n", classname, testname, message);
        break;
    }
}

static int aggregateResult(int run, int passed, int failed, int unknown)
{
    if (passed)
    {
        TextColor_fprintf(stderr, ", " TCS_LIGHTGREEN "passed: %d" TCS_NORMAL,
                passed);
    }
    if (failed)
    {
        TextColor_fprintf(stderr, ", " TCS_LIGHTRED "failed/crashed: %d"
                TCS_NORMAL, failed);
    }
    if (unknown)
    {
        TextColor_fprintf(stderr, ", " TCS_YELLOW "unknown: %d" TCS_NORMAL,
                unknown);
    }
    fputc('\n', stderr);
    return (passed == run) ? EXIT_SUCCESS : EXIT_FAILURE;
}

static void consoleClassResultHandler(const TestClass *testClass, void *args)
{
    struct result *result = args;
    int run = TestClass_testsRun(testClass);
    int passed = TestClass_testsPassed(testClass);
    int failed = TestClass_testsFailed(testClass);
    int unknown = TestClass_testsUnknown(testClass);

    TextColor_fprintf(stderr, TCS_LIGHTCYAN "   [TEST] %s run: %d" TCS_NORMAL,
            Plugin_id(TestClass_plugin(testClass)), run);

    aggregateResult(run, passed, failed, unknown);

    result->run += run;
    result->failed += failed;
    result->passed += passed;
    result->unknown += unknown;
}

int main(int argc, char **argv)
{
    int preprocess = 0;
    int interactive = 0;
    char *outFilename = 0;
    char *gdbPath = 0;
    List *positionalArgs = List_createStr(0);

    Cmdline *cmdline = Cmdline_create(CLS_Unix);
    Cmdline_addFromArgv(cmdline, argc, argv);

    CmdlineParser *parser = CmdlineParser_create();
    CmdlineParser_register(parser, OPTID_PREPROCESS, 'p', 0, COM_Switch);
    CmdlineParser_register(parser, OPTID_OUTPUT, 'o', 0, COM_ArgRequired);
    CmdlineParser_register(parser, OPTID_GDB, 'g', 0, COM_ArgRequired);
    CmdlineParser_register(parser, OPTID_INTERACTIVE, 'i', 0, COM_Switch);

    const CmdlineItem *item;

    while ((item = CmdlineParser_next(parser, cmdline)))
    {
        switch (CmdlineItem_id(item))
        {
        case OPTID_POSITIONALARG:
            List_append(positionalArgs,
                    String_copy(CmdlineItem_arg(item)));
            break;

        case OPTID_PREPROCESS:
            preprocess = 1;
            break;

        case OPTID_OUTPUT:
            free(outFilename);
            outFilename = String_copy(CmdlineItem_arg(item));
            break;

        case OPTID_GDB:
            free(gdbPath);
            gdbPath = String_copy(CmdlineItem_arg(item));
            break;

        case OPTID_INTERACTIVE:
            interactive = 1;
            break;
        }
    }

    CmdlineParser_destroy(parser);
    Cmdline_destroy(cmdline);

    exeName = argv[0];

    if (interactive)
    {
        List_destroy(positionalArgs);
        Gui *gui = Gui_create();
        int rc = Gui_run(gui);
        Gui_destroy(gui);
        return rc;
    }

    if (preprocess)
    {
        if (gdbPath) goto usage;
        if (List_length(positionalArgs) != 1) goto usage;
        int success = preproc(List_getStr(positionalArgs, 0), outFilename);
        free(outFilename);
        List_destroy(positionalArgs);
        return success ? EXIT_SUCCESS : EXIT_FAILURE;
    }

    if (outFilename) goto usage;
    Runner_mainHook(positionalArgs, gdbPath);

    ListIterator *pai = List_iterator(positionalArgs);
    if (!ListIterator_moveNext(pai))
    {
        ListIterator_destroy(pai);
        goto usage;
    }

    List *testClasses = 0;
    List *plugins = Plugin_loadDir(ListIterator_current(pai), TEST_PLUGIN_ID);
    if (plugins)
    {
        testClasses = List_transform(plugins, deleteTestClass, 0, pluginToTestClass, 0);
        List_destroy(plugins);
    }
    while (ListIterator_moveNext(pai))
    {
        plugins = Plugin_loadDir(ListIterator_current(pai), TEST_PLUGIN_ID);
        if (testClasses && plugins)
        {
            List *moreTests = List_transform(plugins, deleteTestClass, 0, pluginToTestClass, 0);
            List *newTests = List_concat(testClasses, moreTests);
            List_destroy(testClasses);
            List_destroy(moreTests);
            List_destroy(plugins);
            testClasses = newTests;
        }
        else if (plugins)
        {
            testClasses = List_transform(plugins, deleteTestClass, 0, pluginToTestClass, 0);
            List_destroy(plugins);
        }
    }

    if (!testClasses)
    {
        fputs("POCAS Testrunner error: no tests found!\n", stderr);
        return EXIT_FAILURE;
    }

    struct result result;
    memset(&result, 0, sizeof(result));

    ListIterator *tci = List_iterator(testClasses);
    while (ListIterator_moveNext(tci))
    {
        TestClass *tc = ListIterator_current(tci);
        ListIterator *ti = List_iterator(TestClass_testCases(tc));
        while (ListIterator_moveNext(ti))
        {
            TestCase *t = ListIterator_current(ti);
            TestCase_setResultHandler(t, consoleResultHandler, 0);
        }
        ListIterator_destroy(ti);
        TestClass_setResultHandler(tc, consoleClassResultHandler, &result);
        TestClass_run(tc, gdbPath);
    }

    ListIterator_destroy(tci);
    List_destroy(testClasses);

    TextColor_fprintf(stderr, TCS_LIGHTCYAN "   [TRES] total tests run: %d"
            TCS_NORMAL, result.run);
    return aggregateResult(result.run, result.passed, result.failed, result.unknown);

usage:
    free(outFilename);
    free(gdbPath);
    List_destroy(positionalArgs);
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
