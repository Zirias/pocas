#ifndef TESTRESULT_INTERNAL_H
#define TESTRESULT_INTERNAL_H

#include <pocas/test/resultcode.h>

typedef struct ListIterator ListIterator;
typedef struct TestResult TestResult;

TestResult *TestResult_create(void);

TestResultCode TestResult_code(const TestResult *self);
void TestResult_setCode(TestResult *self, TestResultCode code);

const char *TestResult_message(const TestResult *self);
void TestResult_setMessage(TestResult *self, const char *message);

ListIterator *TestResult_ignoredMessages(const TestResult *self);
void TestResult_addIgnoredMessage(TestResult *self, const char *message);

void TestResult_destroy(TestResult *self);

#endif // TESTRESULT_INTERNAL_H
