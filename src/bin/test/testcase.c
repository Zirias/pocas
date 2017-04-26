#include <stdio.h>
#include <stdlib.h>

#include <pocas/core/list.h>
#include <pocas/core/string.h>

#include <pocas/test/test.h>
#include "testresult_internal.h"
#include "testcase_internal.h"
#include "testclass_internal.h"

struct TestCase
{
    TestClass *testClass;
    char *testMethod;
    int enabled;
    TestResult *result;
    char *message;
    TestCaseResultHandler resultHandler;
    void *resultHandlerArgs;
};

SOLOCAL TestCase *TestCase_create(TestClass *testClass, const char *testMethod)
{
    TestCase *self = malloc(sizeof(TestCase));
    self->testClass = testClass;
    self->testMethod = String_copy(testMethod);
    self->enabled = 1;
    self->result = TestResult_create();
    self->message = 0;
    self->resultHandler = 0;
    return self;
}

SOLOCAL int TestCase_enabled(const TestCase *self)
{
    return self->enabled;
}

SOLOCAL void TestCase_setEnabled(TestCase *self, int enabled)
{
    self->enabled = !!enabled;
}

SOLOCAL const TestClass *TestCase_class(const TestCase *self)
{
    return self->testClass;
}

SOLOCAL const char *TestCase_method(const TestCase *self)
{
    return self->testMethod;
}

SOLOCAL const TestResult *TestCase_result(const TestCase *self)
{
    return self->result;
}

SOLOCAL void TestCase_setResultHandler(TestCase *self,
        TestCaseResultHandler handler, void *args)
{
    self->resultHandler = handler;
    self->resultHandlerArgs = args;
}

SOLOCAL const char *TestCase_message(const TestCase *self)
{
    return self->message;
}

SOLOCAL void TestCase_setMessage(TestCase *self, const char *message)
{
    free(self->message);
    if (message) self->message = String_copy(message);
    else self->message = 0;
}

SOLOCAL void TestCase_reset(TestCase *self)
{
    free(self->message);
    self->message = 0;
    TestResult_destroy(self->result);
    self->result = TestResult_create();
}

SOLOCAL void TestCase_evaluate(TestCase *self, int exitCode, List *output)
{
    char buf[256];
    TestCase_reset(self);

    int expectCrash = 0;
    int ignore = 0;

    TestResultCode defaultResult = TRC_UNKN;

    ListIterator *i = List_iterator(output);
    while (ListIterator_moveNext(i))
    {
        const char *line = ListIterator_current(i);
        switch (*line)
        {
            case '0':
                if (ignore)
                {
                    if (ignore > 0) --ignore;
                    TestResult_addIgnoredMessage(self->result, line+1);
                    break;
                }
                else
                {
                    TestResult_setCode(self->result, TRC_FAIL);
                    TestResult_setMessage(self->result, line+1);
                    TestClass_addFailed(self->testClass);
                    goto done;
                }

            case '1':
                if (expectCrash)
                {
                    TestResult_setCode(self->result, TRC_FAIL);
                    TestResult_setMessage(self->result, "expected crash, but passed instead.");
                    TestClass_addFailed(self->testClass);
                }
                else
                {
                    TestResult_setCode(self->result, TRC_PASS);
                    TestResult_setMessage(self->result, line+1);
                    TestClass_addPassed(self->testClass);
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
            snprintf(buf, 256, "expectedly failed to run (exit code was %d)", exitCode);
            TestResult_setCode(self->result, TRC_PASS);
            TestResult_setMessage(self->result, buf);
            TestClass_addPassed(self->testClass);
        }
        else
        {
            snprintf(buf, 256, "failed to run (exit code was %d)", exitCode);
            TestResult_setCode(self->result, TRC_CRSH);
            TestResult_setMessage(self->result, buf);
            TestClass_addFailed(self->testClass);
        }
        goto done;
    }

    switch (defaultResult)
    {
    case TRC_FAIL:
        TestResult_setCode(self->result, TRC_FAIL);
        TestResult_setMessage(self->result, "no test result, defaulted to FAIL");
        TestClass_addFailed(self->testClass);
        break;
    case TRC_PASS:
        TestResult_setCode(self->result, TRC_PASS);
        TestResult_setMessage(self->result, "no test result, defaulted to PASS");
        TestClass_addPassed(self->testClass);
        break;

    // other values not supported, default to UNKN
    case TRC_CRSH:
    case TRC_NONE:
    default:
        TestResult_setCode(self->result, TRC_UNKN);
        TestResult_setMessage(self->result, "no test result");
        TestClass_addUnknown(self->testClass);
        break;
    }

done:
    List_destroy(output);
    if (self->resultHandler)
    {
        self->resultHandler(self, self->result, self->resultHandlerArgs);
    }
}

SOLOCAL void TestCase_destroy(TestCase *self)
{
    if (!self) return;
    TestResult_destroy(self->result);
    free(self->testMethod);
    free(self->message);
    free(self);
}
