#ifndef POCAS_TEST_METHOD_H
#define POCAS_TEST_METHOD_H

#include <pocas/test/decl.h>
#include <pocas/test/resultcode.h>

typedef struct TestMethod TestMethod;

#define TESTINIT() \
    SOEXPORT void pocastest__init(__attribute__((unused)) TestMethod *pocastest__self)

#define TESTMETHOD(name) \
    SOEXPORT void pocastest__method_##name(__attribute__((unused)) TestMethod *pocastest__self)

#define TESTDONE() \
    SOEXPORT void pocastest__done(__attribute__((unused)) TestMethod *pocastest__self)

#define Test_assertNotNull(pointer, message) \
    i_Test__assertNotNull(pocastest__self, __FILE__, __LINE__, #pointer, pointer, message)
#define Test_assertRefEqual(expected, actual, message) \
    i_Test__assertRefEqual(pocastest__self, __FILE__, __LINE__, #actual, expected, actual, message)
#define Test_assertStrEqual(expected, actual, message) \
    i_Test__assertStrEqual(pocastest__self, __FILE__, __LINE__, #actual, expected, actual, message)
#define Test_assertIntEqual(expected, actual, message) \
    i_Test__assertIntEqual(pocastest__self, __FILE__, __LINE__, #actual, expected, actual, message)
#define Test_fail(message) \
    i_Test__fail(pocastest__self, __FILE__, __LINE__, message)
#define Test_pass() \
    i_Test__pass(pocastest__self)
#define Test_default(result) \
    i_Test__default(pocastest__self, result)
#define Test_ignore(num) \
    i_Test__ignore(pocastest__self, num)
#define Test_expectCrash() \
    i_Test__expectCrash(pocastest__self)

DECLEXPORT void i_Test__assertNotNull(TestMethod *self, const char *file, unsigned line, const char *pointerName,
                                   const void *pointer, const char *message);
DECLEXPORT void i_Test__assertRefEqual(TestMethod *self, const char *file, unsigned line, const char *actualName,
                                    const void *expected, const void *actual, const char *message);
DECLEXPORT void i_Test__assertStrEqual(TestMethod *self, const char *file, unsigned line, const char *actualName,
                                    const char *expected, const char *actual, const char *message);
DECLEXPORT void i_Test__assertIntEqual(TestMethod *self, const char *file, unsigned line, const char *actualName,
                                    long expected, long actual, const char *message);
DECLEXPORT void i_Test__fail(TestMethod *self, const char *file, unsigned line, const char *message);
DECLEXPORT void i_Test__pass(TestMethod *self);
DECLEXPORT void i_Test__default(TestMethod *self, TestResultCode result);
DECLEXPORT void i_Test__ignore(TestMethod *self, int num);
DECLEXPORT void i_Test__expectCrash(TestMethod *self);

#endif
