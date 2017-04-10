#ifndef POCAS_TEST_TEST_H
#define POCAS_TEST_TEST_H

#include <pocas/test/decl.h>

#define TEST(name, init, done, ...) \
    SOEXPORT const char *pocastest__id = name; \
    SOEXPORT void (*pocastest__init)(void) = init; \
    SOEXPORT void (*pocastest__done)(void) = done; \
    SOEXPORT const char *pocastest__methods[] = { __VA_ARGS__ , 0}

#define TESTMETHOD(name) \
    SOEXPORT void pocastest__method_##name(void)

typedef enum TestResult
{
    TestResult_UNKN = -1,
    TestResult_FAIL = 0,
    TestResult_PASS = 1
} TestResult;

DECLEXPORT void Test__assertNotNull(const char *file, unsigned line, const char *pointerName,
                                   void *pointer, const char *message);
DECLEXPORT void Test__assertRefEqual(const char *file, unsigned line, const char *actualName,
                                    void *expected, void *actual, const char *message);
DECLEXPORT void Test__assertIntEqual(const char *file, unsigned line, const char *actualName,
                                    long expected, long actual, const char *message);
DECLEXPORT void Test__fail(const char *file, unsigned line, const char *message);
DECLEXPORT void Test_pass(void);
DECLEXPORT void Test_default(TestResult result);
DECLEXPORT void Test_ignore(int num);
DECLEXPORT void Test_expectCrash(void);

#define Test_assertNotNull(pointer, message) \
    Test__assertNotNull(__FILE__, __LINE__, #pointer, pointer, message)
#define Test_assertRefEqual(expected, actual, message) \
    Test__assertRefEqual(__FILE__, __LINE__, #actual, expected, actual, message)
#define Test_assertIntEqual(expected, actual, message) \
    Test__assertIntEqual(__FILE__, __LINE__, #actual, expected, actual, message)
#define Test_fail(message) \
    Test__fail(__FILE__, __LINE__, message)

#endif
