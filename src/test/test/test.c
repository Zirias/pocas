#include <stdio.h>

#include <pocas/test/test.h>

TESTCLASS("POCAS test::Test", 0, 0, \
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
    Test_ignore(1);

    Test_fail("failed intentionally");
    Test_pass();
}

TESTMETHOD(dummyunknown)
{
    Test_default(TestResult_PASS);

    // doing nothing is inconclusive
}

TESTMETHOD(dummycrash)
{
    Test_expectCrash();

    // crash intentionally
    int c = *((int *)0);
    printf("%d", c); // to prevent optimizing away the crash
}

TESTMETHOD(dummynull)
{
    Test_ignore(1);

    void *testpointer = 0;
    Test_assertNotNull(testpointer, 0);
    Test_pass();
}

TESTMETHOD(dummynotequal)
{
    Test_ignore(1);

    int answer = 23;
    Test_assertIntEqual(42, answer, "just wrong!");
    Test_pass();
}
