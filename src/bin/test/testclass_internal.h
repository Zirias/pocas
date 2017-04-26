#ifndef TESTCLASS_INTERNAL_H
#define TESTCLASS_INTERNAL_H

typedef struct Plugin Plugin;
typedef struct List List;
typedef struct TestClass TestClass;

typedef void (*TestClassResultHandler)(const TestClass *testClass, void *args);

TestClass *TestClass_create(Plugin *plugin);
const List *TestClass_testCases(const TestClass *self);

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

const Plugin *TestClass_plugin(const TestClass *self);

void TestClass_destroy(TestClass *self);

#endif // TESTCLASS_INTERNAL_H
