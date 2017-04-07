#include <stdio.h>

#include <pocas/test/test.h>

TEST("pocascore", 0, 0, \
     "dummypass",
     "dummyfail",
     "dummyunknown",
     "dummycrash",
     "dummynull",
     "dummynotequal");

TESTMETHOD(dummypass)
{
    Test_pass();
}

TESTMETHOD(dummyfail)
{
    Test_fail("failed intentionally");
}

TESTMETHOD(dummyunknown)
{
    // doing nothing is inconclusive
}

TESTMETHOD(dummycrash)
{
    // crash intentionally
    int c = *((int *)0);
    printf("%d", c); // to prevent optimizing away the crash
}

TESTMETHOD(dummynull)
{
    void *testpointer = 0;
    Test_assertNotNull(testpointer, 0);
}

TESTMETHOD(dummynotequal)
{
    int answer = 23;
    Test_assertIntEqual(42, answer, "just wrong!");
}
