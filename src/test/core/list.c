#include <stdio.h>

#include <pocas/core/list.h>
#include <pocas/test/test.h>

TESTCLASS("POCAS core::List", 0, 0, \
     "append_three_items");

TESTMETHOD(append_three_items)
{
    char *items[] = { "one", "two", "three" };

    List *list = List_create(0, 0, 0);
    List_append(list, items[0]);
    List_append(list, items[1]);
    List_append(list, items[2]);

    Test_assertIntEqual(3, List_length(list), "wrong item count");
    Test_assertStrEqual(items[0], List_get(list, 0), "unexpected item");
    Test_assertStrEqual(items[1], List_get(list, 1), "unexpected item");
    Test_assertStrEqual(items[2], List_get(list, 2), "unexpected item");

    List_destroy(list);
    Test_pass();
}
