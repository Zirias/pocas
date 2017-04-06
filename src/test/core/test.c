#include <stdlib.h>
#include <pocas/test/test.h>

TEST("pocascore", 0, 0, \
     "dummypass",
     "dummyfail",
     "dummyunknown",
     "dummycrash");

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
}

TESTMETHOD(dummycrash)
{
    exit(EXIT_FAILURE);
}
