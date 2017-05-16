#include <stdlib.h>

#include <pocas/core/list.h>
#include <pocas/core/string.h>

#include "testresult_internal.h"

struct TestResult
{
    PT_TestResultCode code;
    char *message;
    PC_List *ignoredMessages;
};

TestResult *TestResult_create(void)
{
    TestResult *self = malloc(sizeof(TestResult));
    self->code = PT_TRC_NONE;
    self->message = 0;
    self->ignoredMessages = 0;
    return self;
}

PT_TestResultCode TestResult_code(const TestResult *self)
{
    return self->code;
}

void TestResult_setCode(TestResult *self, PT_TestResultCode code)
{
    self->code = code;
}

const char *TestResult_message(const TestResult *self)
{
    return self->message;
}

void TestResult_setMessage(TestResult *self, const char *message)
{
    free(self->message);
    self->message = PC_String_copy(message);
}


PC_ListIterator *TestResult_ignoredMessages(const TestResult *self)
{
    if (!self->ignoredMessages) return 0;
    return PC_List_iterator(self->ignoredMessages);
}

void TestResult_addIgnoredMessage(TestResult *self, const char *message)
{
    if (!self->ignoredMessages) self->ignoredMessages = PC_List_createStr(0);
    PC_List_append(self->ignoredMessages, PC_String_copy(message));
}

void TestResult_destroy(TestResult *self)
{
    if (!self) return;
    PC_List_destroy(self->ignoredMessages);
    free(self->message);
    free(self);
}
