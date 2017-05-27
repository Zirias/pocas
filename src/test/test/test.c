#include <stdio.h>

#include <pocas/test/test.h>

PT_TESTCLASS(pocastest::Test);

PT_TESTMETHOD(test_pass_passes)
{
    PT_Test_pass();
}

PT_TESTMETHOD(test_fail_fails)
{
    PT_Test_ignore(1);

    PT_Test_fail("failed intentionally");
    PT_Test_pass();
}

PT_TESTMETHOD(test_default_is_used)
{
    PT_Test_default(PT_TRC_PASS);

    // doing nothing is inconclusive
}

PT_TESTMETHOD(test_expected_crash)
{
    PT_Test_expectCrash();

    // crash intentionally
    int *volatile nptr = 0;
    int c = *nptr;
    printf("%d", c); // to prevent optimizing away the crash
}

PT_TESTMETHOD(test_assert_not_null)
{
    PT_Test_ignore(1);

    void *testpointer = 0;
    PT_Test_assertNotNull(testpointer, 0);
    PT_Test_pass();
}

PT_TESTMETHOD(test_assert_not_equal)
{
    PT_Test_ignore(1);

    int answer = 23;
    PT_Test_assertIntEqual(42, answer, "just wrong!");
    PT_Test_pass();
}
