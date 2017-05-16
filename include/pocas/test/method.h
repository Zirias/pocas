#ifndef POCAS_TEST_METHOD_H
#define POCAS_TEST_METHOD_H

#include <pocas/test/decl.h>
#include <pocas/test/resultcode.h>

C_CLASS_DECL(PT_TestMethod);

#define PT_TESTINIT() \
    SOEXPORT void pocastest__init(__attribute__((unused)) PT_TestMethod *pocastest__self)

#define PT_TESTMETHOD(name) \
    SOEXPORT void pocastest__method_##name(__attribute__((unused)) PT_TestMethod *pocastest__self)

#define PT_TESTDONE() \
    SOEXPORT void pocastest__done(__attribute__((unused)) PT_TestMethod *pocastest__self)

#define PT_Test_assertNotNull(pointer, message) \
    i_Test__assertNotNull(pocastest__self, __FILE__, __LINE__, #pointer, pointer, message)
#define PT_Test_assertRefEqual(expected, actual, message) \
    i_Test__assertRefEqual(pocastest__self, __FILE__, __LINE__, #actual, expected, actual, message)
#define PT_Test_assertStrEqual(expected, actual, message) \
    i_Test__assertStrEqual(pocastest__self, __FILE__, __LINE__, #actual, expected, actual, message)
#define PT_Test_assertIntEqual(expected, actual, message) \
    i_Test__assertIntEqual(pocastest__self, __FILE__, __LINE__, #actual, expected, actual, message)
#define PT_Test_fail(message) \
    i_Test__fail(pocastest__self, __FILE__, __LINE__, message)
#define PT_Test_pass() \
    i_Test__pass(pocastest__self)
#define PT_Test_default(result) \
    i_Test__default(pocastest__self, result)
#define PT_Test_ignore(num) \
    i_Test__ignore(pocastest__self, num)
#define PT_Test_expectCrash() \
    i_Test__expectCrash(pocastest__self)

DECLEXPORT void i_Test__assertNotNull(PT_TestMethod *self, const char *file, unsigned line, const char *pointerName,
                                   const void *pointer, const char *message);
DECLEXPORT void i_Test__assertRefEqual(PT_TestMethod *self, const char *file, unsigned line, const char *actualName,
                                    const void *expected, const void *actual, const char *message);
DECLEXPORT void i_Test__assertStrEqual(PT_TestMethod *self, const char *file, unsigned line, const char *actualName,
                                    const char *expected, const char *actual, const char *message);
DECLEXPORT void i_Test__assertIntEqual(PT_TestMethod *self, const char *file, unsigned line, const char *actualName,
                                    long expected, long actual, const char *message);
DECLEXPORT void i_Test__fail(PT_TestMethod *self, const char *file, unsigned line, const char *message);
DECLEXPORT void i_Test__pass(PT_TestMethod *self);
DECLEXPORT void i_Test__default(PT_TestMethod *self, PT_TestResultCode result);
DECLEXPORT void i_Test__ignore(PT_TestMethod *self, int num);
DECLEXPORT void i_Test__expectCrash(PT_TestMethod *self);

#endif
