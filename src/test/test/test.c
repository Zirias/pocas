#include <stdio.h>

#include <pocas/test/test.h>

TESTCLASS(pocastest::Test);

TESTMETHOD(test_pass_passes)
{
    Test_pass();
}

TESTMETHOD(test_fail_fails)
{
    Test_ignore(1);

    Test_fail("failed intentionally");
    Test_pass();
}

TESTMETHOD(test_default_is_used)
{
    Test_default(TestResult_PASS);

    // doing nothing is inconclusive
}

TESTMETHOD(test_expected_crash)
{
    Test_expectCrash();

    // crash intentionally
    int c = *((int *)0);
    printf("%d", c); // to prevent optimizing away the crash
}

TESTMETHOD(test_assert_not_null)
{
    Test_ignore(1);

    void *testpointer = 0;
    Test_assertNotNull(testpointer, 0);
    Test_pass();
}

TESTMETHOD(test_assert_not_equal)
{
    Test_ignore(1);

    int answer = 23;
    Test_assertIntEqual(42, answer, "just wrong!");
    Test_pass();
}
