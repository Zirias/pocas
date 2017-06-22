#ifndef POCAS_TEST_TEST_H
#define POCAS_TEST_TEST_H

#include <pocas/test/decl.h>
#include <pocas/test/resultcode.h>
#include <pocas/test/method.h>

#define PT_TESTCLASS(name) \
    TEST_SOEXPORT const char *pocastest__id = #name

#endif
