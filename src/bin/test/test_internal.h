#ifndef TEST_INTERNAL_H
#define TEST_INTERNAL_H

#include <setjmp.h>

#include <pocas/test/test.h>

#define TEST_PLUGIN_ID "pocastest__id"
#define TEST_METHODS "pocastest__methods"

Test *Test_create(void);
jmp_buf *Test_jmp(Test *self);
void Test_destroy(Test *self);

#endif
