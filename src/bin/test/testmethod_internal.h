#ifndef TEST_INTERNAL_H
#define TEST_INTERNAL_H

#include <setjmp.h>

#include <pocas/test/method.h>

#define TEST_PLUGIN_ID "pocastest__id"
#define TEST_METHODS "pocastest__methods"

TestMethod *TestMethod_create(void);
jmp_buf *TestMethod_jmp(TestMethod *self);
void TestMethod_destroy(TestMethod *self);

#endif
