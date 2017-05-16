#ifndef TESTCLASS_INTERNAL_H
#define TESTCLASS_INTERNAL_H

#include <pocas/test/decl.h>

C_CLASS_DECL(PC_Plugin);
C_CLASS_DECL(PC_List);

C_CLASS_DECL(TestClass);

typedef void (*TestClassResultHandler)(const TestClass *testClass, void *args);

TestClass *TestClass_create(PC_Plugin *plugin);
const PC_List *TestClass_testCases(const TestClass *self);

int TestClass_enabled(const TestClass *self);
void TestClass_setEnabled(TestClass *self, int enabled);

void TestClass_setResultHandler(TestClass *self,
        TestClassResultHandler handler, void *args);

void TestClass_run(TestClass *self, const char *gdbPath);

int TestClass_testsRun(const TestClass *self);
int TestClass_testsPassed(const TestClass *self);
int TestClass_testsFailed(const TestClass *self);
int TestClass_testsUnknown(const TestClass *self);

void TestClass_addPassed(TestClass *self);
void TestClass_addFailed(TestClass *self);
void TestClass_addUnknown(TestClass *self);

const PC_Plugin *TestClass_plugin(const TestClass *self);

void TestClass_destroy(TestClass *self);

#endif // TESTCLASS_INTERNAL_H
