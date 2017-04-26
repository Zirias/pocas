#include <stdlib.h>

#include <pocas/core/list.h>
#include <pocas/core/plugin.h>

#include "testcase_internal.h"
#include "testmethod_internal.h"
#include "testclass_internal.h"

struct TestClass
{
    Plugin *plugin;
    List *testCases;
    int enabled;
    int run;
    int passed;
    int failed;
    int unknown;
    TestClassResultHandler resultHandler;
    void *resultHandlerArgs;
};

static void deleteTestCase(void *testCase)
{
    TestCase_destroy(testCase);
}

static void TestClass_reset(TestClass *self)
{
    self->run = 0;
    self->passed = 0;
    self->failed = 0;
    self->unknown = 0;
}

TestClass *TestClass_create(Plugin *plugin)
{
    const char **testmethods = Plugin_symbol(plugin, TEST_METHODS);
    if (!testmethods)
    {
        Plugin_close(plugin);
        return 0;
    }

    TestClass *self = malloc(sizeof(TestClass));
    self->plugin = plugin;
    self->testCases = List_create(0, deleteTestCase, 0);
    while (*testmethods)
    {
        TestCase *testCase = TestCase_create(self, *testmethods);
        List_append(self->testCases, testCase);
        ++testmethods;
    }
    self->enabled = 1;
    self->resultHandler = 0;
    TestClass_reset(self);
    return self;
}

const Plugin *TestClass_plugin(const TestClass *self)
{
    return self->plugin;
}

int TestClass_enabled(const TestClass *self)
{
    return self->enabled;
}

void TestClass_setEnabled(TestClass *self, int enabled)
{
    self->enabled = !!enabled;
}

void TestClass_setResultHandler(TestClass *self,
        TestClassResultHandler handler, void *args)
{
    self->resultHandler = handler;
    self->resultHandlerArgs = args;
}

void TestClass_run(TestClass *self, const char *gdbPath)
{
    TestClass_reset(self);
    ListIterator *ci = List_iterator(self->testCases);
    while (ListIterator_moveNext(ci))
    {
        TestCase_run(ListIterator_current(ci), gdbPath);
    }
    ListIterator_destroy(ci);
    if (self->resultHandler)
    {
        self->resultHandler(self, self->resultHandlerArgs);
    }
}

const List *TestClass_testCases(const TestClass *self)
{
    return self->testCases;
}

int TestClass_testsRun(const TestClass *self)
{
    return self->run;
}

int TestClass_testsPassed(const TestClass *self)
{
    return self->passed;
}

int TestClass_testsFailed(const TestClass *self)
{
    return self->failed;
}

int TestClass_testsUnknown(const TestClass *self)
{
    return self->unknown;
}

void TestClass_addPassed(TestClass *self)
{
    ++self->passed;
    ++self->run;
}

void TestClass_addFailed(TestClass *self)
{
    ++self->failed;
    ++self->run;
}

void TestClass_addUnknown(TestClass *self)
{
    ++self->unknown;
    ++self->run;
}

void TestClass_destroy(TestClass *self)
{
    if (!self) return;
    List_destroy(self->testCases);
    Plugin_close(self->plugin);
    free(self);
}
