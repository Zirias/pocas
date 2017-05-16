#ifndef TESTRESULT_INTERNAL_H
#define TESTRESULT_INTERNAL_H

#include <pocas/test/resultcode.h>

C_CLASS_DECL(PC_ListIterator);

C_CLASS_DECL(TestResult);

TestResult *TestResult_create(void);

PT_TestResultCode TestResult_code(const TestResult *self);
void TestResult_setCode(TestResult *self, PT_TestResultCode code);

const char *TestResult_message(const TestResult *self);
void TestResult_setMessage(TestResult *self, const char *message);

PC_ListIterator *TestResult_ignoredMessages(const TestResult *self);
void TestResult_addIgnoredMessage(TestResult *self, const char *message);

void TestResult_destroy(TestResult *self);

#endif // TESTRESULT_INTERNAL_H
