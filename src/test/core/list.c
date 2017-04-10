#include <stdio.h>

#include <pocas/core/list.h>
#include <pocas/test/test.h>

TESTCLASS("POCAS core::List",
     "append_three_items");

static List *notOwningList;

TESTINIT()
{
    notOwningList = List_create(0, 0, 0);
}

TESTDONE()
{
    List_destroy(notOwningList);
}

TESTMETHOD(append_three_items)
{
    char *items[] = { "one", "two", "three" };

    List_append(notOwningList, items[0]);
    List_append(notOwningList, items[1]);
    List_append(notOwningList, items[2]);

    Test_assertIntEqual(3, List_length(notOwningList), "wrong item count");
    Test_assertStrEqual(items[0], List_get(notOwningList, 0), "unexpected item");
    Test_assertStrEqual(items[1], List_get(notOwningList, 1), "unexpected item");
    Test_assertStrEqual(items[2], List_get(notOwningList, 2), "unexpected item");

    Test_pass();
}
