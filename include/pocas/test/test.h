#ifndef POCAS_TEST_TEST_H
#define POCAS_TEST_TEST_H

#include <pocas/test/decl.h>

typedef struct Test Test;

typedef enum TestResult
{
    TestResult_UNKN = -1,
    TestResult_FAIL = 0,
    TestResult_PASS = 1
} TestResult;

DECLEXPORT void Test__assertNotNull(Test *self, const char *file, unsigned line, const char *pointerName,
                                   const void *pointer, const char *message);
DECLEXPORT void Test__assertRefEqual(Test *self, const char *file, unsigned line, const char *actualName,
                                    const void *expected, const void *actual, const char *message);
DECLEXPORT void Test__assertStrEqual(Test *self, const char *file, unsigned line, const char *actualName,
                                    const char *expected, const char *actual, const char *message);
DECLEXPORT void Test__assertIntEqual(Test *self, const char *file, unsigned line, const char *actualName,
                                    long expected, long actual, const char *message);
DECLEXPORT void Test__fail(Test *self, const char *file, unsigned line, const char *message);
DECLEXPORT void Test__pass(Test *self);
DECLEXPORT void Test__default(Test *self, TestResult result);
DECLEXPORT void Test__ignore(Test *self, int num);
DECLEXPORT void Test__expectCrash(Test *self);

#define TESTCLASS(name, init, done, ...) \
    SOEXPORT const char *pocastest__id = name; \
    SOEXPORT void (*pocastest__init)(void) = init; \
    SOEXPORT void (*pocastest__done)(void) = done; \
    SOEXPORT const char *pocastest__methods[] = { __VA_ARGS__ , 0}

#define TESTMETHOD(name) \
    SOEXPORT void pocastest__method_##name(Test *pocastest__self)

#define Test_assertNotNull(pointer, message) \
    Test__assertNotNull(pocastest__self, __FILE__, __LINE__, #pointer, pointer, message)
#define Test_assertRefEqual(expected, actual, message) \
    Test__assertRefEqual(pocastest__self, __FILE__, __LINE__, #actual, expected, actual, message)
#define Test_assertStrEqual(expected, actual, message) \
    Test__assertStrEqual(pocastest__self, __FILE__, __LINE__, #actual, expected, actual, message)
#define Test_assertIntEqual(expected, actual, message) \
    Test__assertIntEqual(pocastest__self, __FILE__, __LINE__, #actual, expected, actual, message)
#define Test_fail(message) \
    Test__fail(pocastest__self, __FILE__, __LINE__, message)
#define Test_pass() \
    Test__pass(pocastest__self)
#define Test_default(result) \
    Test__default(pocastest__self, result)
#define Test_ignore(num) \
    Test__ignore(pocastest__self, num)
#define Test_expectCrash() \
    Test__expectCrash(pocastest__self)

#endif
