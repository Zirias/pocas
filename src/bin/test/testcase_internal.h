#ifndef TESTCASE_INTERNAL_H
#define TESTCASE_INTERNAL_H

#include "testresult_internal.h"

#include <pocas/test/decl.h>

C_CLASS_DECL(PC_List);

C_CLASS_DECL(TestClass);
C_CLASS_DECL(TestCase);

typedef void (*TestCaseResultHandler)(
        const TestCase *testCase, const TestResult *result, void *args);

TestCase *TestCase_create(TestClass *testClass, const char *testMethod);

int TestCase_enabled(const TestCase *self);
void TestCase_setEnabled(TestCase *self, int enabled);

void TestCase_run(TestCase *self, const char *gdbPath);
void TestCase_evaluate(TestCase *self, int exitCode, PC_List *output);

const TestResult *TestCase_result(const TestCase *self);
void TestCase_setResultHandler(TestCase *self,
        TestCaseResultHandler handler, void *args);

const char *TestCase_message(const TestCase *self);
void TestCase_setMessage(TestCase *self, const char *message);

void TestCase_reset(TestCase *self);

const char *TestCase_method(const TestCase *self);
const TestClass *TestCase_class(const TestCase *self);

void TestCase_destroy(TestCase *self);

#endif // TESTCASE_INTERNAL_H
