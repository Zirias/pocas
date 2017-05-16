#ifndef TEST_INTERNAL_H
#define TEST_INTERNAL_H

#include <setjmp.h>
#include <stdio.h>

#include <pocas/test/method.h>

#define TEST_PLUGIN_ID "pocastest__id"
#define TEST_METHODS "pocastest__methods"

PT_TestMethod *PT_TestMethod_create(FILE *output);
jmp_buf *PT_TestMethod_jmp(PT_TestMethod *self);
void PT_TestMethod_destroy(PT_TestMethod *self);

#endif
