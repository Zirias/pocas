#include <stdlib.h>

#include <pocas/test/test.h>

#undef HAVE_BUILTIN_TRAP
#ifdef __GNUC__
#  define GCC_VERSION (__GNUC__ * 10000 \
    + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__)
#  if GCC_VERSION > 40203
#    define HAVE_BUILTIN_TRAP
#  endif
#else
#  ifdef __has_builtin
#    if __has_builtin(__builtin_trap)
#      define HAVE_BUILTIN_TRAP
#    endif
#  endif
#endif

#ifdef HAVE_BUILTIN_TRAP
#  define crashMe() __builtin_trap()
#else
#  include <stdio.h>
#  define crashMe() do { \
    int *volatile iptr = 0; \
    int i = *iptr; \
    printf("%d", i); \
    abort(); } while (0)
#endif

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
    crashMe();
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
