#ifndef TEST_INTERNAL_H
#define TEST_INTERNAL_H

#include <pocas/test/test.h>

#define TEST_PLUGIN_ID "pocastest__id"
#define TEST_METHODS "pocastest__methods"

struct Test
{
    int ignore;
    int expectCrash;
};

Test *Test_create(void);
void Test_destroy(Test *self);

#endif
