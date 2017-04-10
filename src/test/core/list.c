#include <stdio.h>
#include <stdlib.h>

#include <pocas/core/list.h>
#include <pocas/test/test.h>

TESTCLASS(pocascore::List);

static List *notOwningList;
static List *stringList1;
static List *stringList2;

static List *tmpList1;
static char *tmpStr1;

TESTINIT()
{
    notOwningList = List_create(0, 0, 0);
    stringList1 = List_createStr(0);
    stringList2 = List_createStr(0);
    tmpList1 = 0;
    tmpStr1 = 0;
}

TESTDONE()
{
    List_destroy(notOwningList);
    List_destroy(stringList1);
    List_destroy(stringList2);
    List_destroy(tmpList1);
    free(tmpStr1);
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

TESTMETHOD(clone_list)
{
    List_append(stringList1, "foo");
    List_append(stringList1, "bar");
    List_append(stringList1, "baz");

    tmpList1 = List_clone(stringList1);

    Test_assertIntEqual(3, List_length(tmpList1), "wrong item cont");
    Test_assertStrEqual("bar", List_get(tmpList1, 1), "unexpected item");
    if (List_get(tmpList1, 1) == List_get(stringList1, 1))
    {
        Test_fail("Cloned list references same object");
    }

    Test_pass();
}

TESTMETHOD(join_strings)
{
    List_append(notOwningList, "This");
    List_append(notOwningList, "is");
    List_append(notOwningList, "a");
    List_append(notOwningList, "test");

    tmpStr1 = List_joinStr(notOwningList, " ");

    Test_assertStrEqual("This is a test", tmpStr1, "wrong join result");

    Test_pass();
}

TESTMETHOD(insert_item)
{
    List_append(notOwningList, "A");
    List_append(notOwningList, "B");

    List_insert(notOwningList, 1, "or");

    Test_assertStrEqual("A", List_get(notOwningList, 0), "unexpected item");
    Test_assertStrEqual("or", List_get(notOwningList, 1), "unexpected item");
    Test_assertStrEqual("B", List_get(notOwningList, 2), "unexpected item");

    Test_pass();
}
