#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <pocas/core/list.h>
#include <pocas/core/string.h>
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

    Test_assertIntEqual(3, List_length(tmpList1), "wrong item count");
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

TESTMETHOD(set_items)
{
    List_append(notOwningList, "A");
    List_append(notOwningList, "B");
    List_append(notOwningList, "C");

    List_set(notOwningList, 63, "X");
    List_set(notOwningList, 1, "Y");

    Test_assertIntEqual(64, List_length(notOwningList), "wrong item count");

    Test_assertStrEqual("A", List_get(notOwningList, 0), "unexpected item");
    Test_assertStrEqual("Y", List_get(notOwningList, 1), "unexpected item");
    Test_assertStrEqual("C", List_get(notOwningList, 2), "unexpected item");
    Test_assertStrEqual("X", List_get(notOwningList, 63), "unexpected item");

    Test_assertRefEqual(0, List_get(notOwningList, 3), "unset item not null");
    Test_assertRefEqual(0, List_get(notOwningList, 62), "unset item not null");

    Test_pass();
}

TESTMETHOD(remove_item)
{
    static char *items[] = {"A", "B", "C"};
    List_append(notOwningList, items[0]);
    List_append(notOwningList, items[1]);
    List_append(notOwningList, items[2]);

    List_remove(notOwningList, items[1]);

    Test_assertIntEqual(2, List_length(notOwningList), "wrong item count");
    Test_assertStrEqual("A", List_get(notOwningList, 0), "unexpected item");
    Test_assertStrEqual("C", List_get(notOwningList, 1), "unexpected item");

    Test_pass();
}

static int stringStartsWithSelector(const void *value, void *arg)
{
    const char *start = arg;
    return !strncmp(start, (const char *)value, strlen(arg));
}

TESTMETHOD(remove_matching_item)
{
    static char *items[] = {"A", "B", "C"};
    List_append(notOwningList, items[0]);
    List_append(notOwningList, items[1]);
    List_append(notOwningList, items[2]);

    List_removeMatching(notOwningList, stringStartsWithSelector, "B");

    Test_assertIntEqual(2, List_length(notOwningList), "wrong item count");
    Test_assertStrEqual("A", List_get(notOwningList, 0), "unexpected item");
    Test_assertStrEqual("C", List_get(notOwningList, 1), "unexpected item");

    Test_pass();
}

TESTMETHOD(invalid_index_gives_0)
{
    Test_assertRefEqual(0, List_get(notOwningList, -1), "invalid index did not return 0");
    Test_assertRefEqual(0, List_get(notOwningList, 0), "invalid index did not return 0");

    Test_pass();
}

TESTMETHOD(select_items)
{
    List_append(stringList1, String_copy("y_FOO"));
    List_append(stringList1, String_copy("n_BAR"));
    List_append(stringList1, String_copy("y_BAZ"));

    tmpList1 = List_select(stringList1, stringStartsWithSelector, "y_");

    Test_assertIntEqual(2, List_length(tmpList1), "wrong item count");
    Test_assertStrEqual("y_BAZ", List_get(tmpList1, 1), "unexpected item");

    Test_pass();
}

static void *stringToLengthTransform(const void *element, void *arg)
{
    (void)arg;
    return (void *)(uintptr_t)strlen((const char *)element);
}

TESTMETHOD(transform_list)
{
    List_append(notOwningList, "one");
    List_append(notOwningList, "two");
    List_append(notOwningList, "three");
    List_append(notOwningList, "fourtytwo");

    tmpList1 = List_transform(notOwningList, 0, 0, stringToLengthTransform, 0);

    Test_assertIntEqual(4, List_length(tmpList1), "wrong item count");
    Test_assertIntEqual(3, (uintptr_t)List_get(tmpList1, 0), "incorrectly transformed");
    Test_assertIntEqual(3, (uintptr_t)List_get(tmpList1, 1), "incorrectly transformed");
    Test_assertIntEqual(5, (uintptr_t)List_get(tmpList1, 2), "incorrectly transformed");
    Test_assertIntEqual(9, (uintptr_t)List_get(tmpList1, 3), "incorrectly transformed");

    Test_pass();
}
