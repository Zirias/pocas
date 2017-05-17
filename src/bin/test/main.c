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
    return TestClass_create((PC_Plugin *)plugin);
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
    const char *classname = PC_Plugin_id(TestClass_plugin(TestCase_class(testCase)));
    const char *message = TestResult_message(result);

    PC_ListIterator *igni = TestResult_ignoredMessages(result);
    if (igni) while (PC_ListIterator_moveNext(igni))
    {
        PC_TextColor_fprintf(stderr,
                PC_TCS_YELLOW "   [IGN]  " PC_TCS_BROWN "%s::%s %s" PC_TCS_NORMAL "\n",
                classname, testname,
                (const char *)PC_ListIterator_current(igni));
    }
    PC_ListIterator_destroy(igni);

    switch (TestResult_code(result))
    {
    case PT_TRC_NONE:
        break;
    case PT_TRC_CRSH:
        PC_TextColor_fprintf(stderr, PC_TCS_LIGHTRED "   [CRSH] %s::%s %s"
                PC_TCS_NORMAL "\n", classname, testname, message);
        break;
    case PT_TRC_FAIL:
        PC_TextColor_fprintf(stderr, PC_TCS_LIGHTRED "   [FAIL] " PC_TCS_NORMAL
                "%s::%s %s\n", classname, testname, message);
        break;
    case PT_TRC_PASS:
        PC_TextColor_fprintf(stderr, PC_TCS_LIGHTGREEN "   [PASS] " PC_TCS_NORMAL
                "%s::%s %s\n", classname, testname, message);
        break;
    case PT_TRC_UNKN:
        PC_TextColor_fprintf(stderr, PC_TCS_YELLOW "   [UNKN] " PC_TCS_NORMAL
                "%s::%s %s\n", classname, testname, message);
        break;
    }
    fflush(stderr);
}

static int aggregateResult(int run, int passed, int failed, int unknown)
{
    if (passed)
    {
        PC_TextColor_fprintf(stderr, ", " PC_TCS_LIGHTGREEN "passed: %d" PC_TCS_NORMAL,
                passed);
    }
    if (failed)
    {
        PC_TextColor_fprintf(stderr, ", " PC_TCS_LIGHTRED "failed/crashed: %d"
                PC_TCS_NORMAL, failed);
    }
    if (unknown)
    {
        PC_TextColor_fprintf(stderr, ", " PC_TCS_YELLOW "unknown: %d" PC_TCS_NORMAL,
                unknown);
    }
    fputc('\n', stderr);
    fflush(stderr);
    return (passed == run) ? EXIT_SUCCESS : EXIT_FAILURE;
}

static void consoleClassResultHandler(const TestClass *testClass, void *args)
{
    struct result *result = args;
    int run = TestClass_testsRun(testClass);
    int passed = TestClass_testsPassed(testClass);
    int failed = TestClass_testsFailed(testClass);
    int unknown = TestClass_testsUnknown(testClass);

    PC_TextColor_fprintf(stderr, PC_TCS_LIGHTCYAN "   [TEST] %s run: %d" PC_TCS_NORMAL,
            PC_Plugin_id(TestClass_plugin(testClass)), run);

    aggregateResult(run, passed, failed, unknown);
    fflush(stderr);

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
    PC_List *positionalArgs = PC_List_createStr(0);

    PC_Cmdline *cmdline = PC_Cmdline_create(PC_CLS_Unix);
    PC_Cmdline_addFromArgv(cmdline, argc, argv);

    PC_CmdlineParser *parser = PC_CmdlineParser_create();
    PC_CmdlineParser_register(parser, OPTID_PREPROCESS, 'p', 0, PC_COM_Switch);
    PC_CmdlineParser_register(parser, OPTID_OUTPUT, 'o', 0, PC_COM_ArgRequired);
    PC_CmdlineParser_register(parser, OPTID_GDB, 'g', 0, PC_COM_ArgRequired);
    PC_CmdlineParser_register(parser, OPTID_INTERACTIVE, 'i', 0, PC_COM_Switch);

    const PC_CmdlineItem *item;

    while ((item = PC_CmdlineParser_next(parser, cmdline)))
    {
        switch (PC_CmdlineItem_id(item))
        {
        case OPTID_POSITIONALARG:
            PC_List_append(positionalArgs,
                    PC_String_copy(PC_CmdlineItem_arg(item)));
            break;

        case OPTID_PREPROCESS:
            preprocess = 1;
            break;

        case OPTID_OUTPUT:
            free(outFilename);
            outFilename = PC_String_copy(PC_CmdlineItem_arg(item));
            break;

        case OPTID_GDB:
            free(gdbPath);
            gdbPath = PC_String_copy(PC_CmdlineItem_arg(item));
            break;

        case OPTID_INTERACTIVE:
            interactive = 1;
            break;
        }
    }

    PC_CmdlineParser_destroy(parser);
    PC_Cmdline_destroy(cmdline);

    exeName = argv[0];

    if (interactive)
    {
        PC_List_destroy(positionalArgs);
        Gui *gui = Gui_create();
        int rc = Gui_run(gui);
        Gui_destroy(gui);
        return rc;
    }

    if (preprocess)
    {
        if (gdbPath) goto usage;
        if (PC_List_length(positionalArgs) != 1) goto usage;
        int success = preproc(PC_List_getStr(positionalArgs, 0), outFilename);
        free(outFilename);
        PC_List_destroy(positionalArgs);
        return success ? EXIT_SUCCESS : EXIT_FAILURE;
    }

    if (outFilename) goto usage;
    Runner_mainHook(positionalArgs, gdbPath);

    PC_ListIterator *pai = PC_List_iterator(positionalArgs);
    if (!PC_ListIterator_moveNext(pai))
    {
        PC_ListIterator_destroy(pai);
        goto usage;
    }

    PC_List *testClasses = 0;
    PC_List *plugins = PC_Plugin_loadDir(PC_ListIterator_current(pai), TEST_PLUGIN_ID);
    if (plugins)
    {
        testClasses = PC_List_transform(plugins, deleteTestClass, 0, pluginToTestClass, 0);
        PC_List_destroy(plugins);
    }
    while (PC_ListIterator_moveNext(pai))
    {
        plugins = PC_Plugin_loadDir(PC_ListIterator_current(pai), TEST_PLUGIN_ID);
        if (testClasses && plugins)
        {
            PC_List *moreTests = PC_List_transform(plugins, deleteTestClass, 0, pluginToTestClass, 0);
            PC_List *newTests = PC_List_concat(testClasses, moreTests);
            PC_List_destroy(testClasses);
            PC_List_destroy(moreTests);
            PC_List_destroy(plugins);
            testClasses = newTests;
        }
        else if (plugins)
        {
            testClasses = PC_List_transform(plugins, deleteTestClass, 0, pluginToTestClass, 0);
            PC_List_destroy(plugins);
        }
    }

    if (!testClasses)
    {
        fputs("POCAS Testrunner error: no tests found!\n", stderr);
        fflush(stderr);
        return EXIT_FAILURE;
    }

    struct result result;
    memset(&result, 0, sizeof(result));

    PC_ListIterator *tci = PC_List_iterator(testClasses);
    while (PC_ListIterator_moveNext(tci))
    {
        TestClass *tc = PC_ListIterator_current(tci);
        PC_ListIterator *ti = PC_List_iterator(TestClass_testCases(tc));
        while (PC_ListIterator_moveNext(ti))
        {
            TestCase *t = PC_ListIterator_current(ti);
            TestCase_setResultHandler(t, consoleResultHandler, 0);
        }
        PC_ListIterator_destroy(ti);
        TestClass_setResultHandler(tc, consoleClassResultHandler, &result);
        TestClass_run(tc, gdbPath);
    }

    PC_ListIterator_destroy(tci);
    PC_List_destroy(testClasses);

    PC_TextColor_fprintf(stderr, PC_TCS_LIGHTCYAN "   [TRES] total tests run: %d"
            PC_TCS_NORMAL, result.run);
    return aggregateResult(result.run, result.passed, result.failed, result.unknown);

usage:
    free(outFilename);
    free(gdbPath);
    PC_List_destroy(positionalArgs);
    fprintf(stderr, "\nPOCAS Testrunner\n"
                    "\n"
                    "USAGE: %s [-g {gdb path}] path [path ...]\n"
                    "for running all tests in the given paths.\n"
                    "use -g for running each test in a GDB debugging session.\n"
                    "\n"
                    "USAGE: %s -p [-o output.c] test.c\n"
                    "to preprocess test.c for compilation.\n",
            argv[0], argv[0]);
    fflush(stderr);
    return EXIT_FAILURE;
}
