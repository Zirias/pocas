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

DECLEXPORT void Test_assertNotNull(void *pointer, const char *message);
DECLEXPORT void Test_assertRefEqual(void *expected, void *actual, const char *message);
DECLEXPORT void Test_assertIntEqual(int expected, int actual, const char *message);
DECLEXPORT void Test_assertRefNotEqual(void *expected, void *actual, const char *message);
DECLEXPORT void Test_assertIntNotEqual(int expected, int actual, const char *message);
DECLEXPORT void Test_fail(const char *message);
DECLEXPORT void Test_pass(void);

#endif
