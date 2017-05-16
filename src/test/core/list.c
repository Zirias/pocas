#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <pocas/core/list.h>
#include <pocas/core/string.h>
#include <pocas/test/test.h>

PT_TESTCLASS(pocascore::List);

static PC_List *notOwningPC_List;
static PC_List *stringPC_List1;
static PC_List *stringPC_List2;

static PC_List *tmpPC_List1;
static char *tmpStr1;

PT_TESTINIT()
{
    notOwningPC_List = PC_List_create(0, 0, 0);
    stringPC_List1 = PC_List_createStr(0);
    stringPC_List2 = PC_List_createStr(0);
    tmpPC_List1 = 0;
    tmpStr1 = 0;
}

PT_TESTDONE()
{
    PC_List_destroy(notOwningPC_List);
    PC_List_destroy(stringPC_List1);
    PC_List_destroy(stringPC_List2);
    PC_List_destroy(tmpPC_List1);
    free(tmpStr1);
}

PT_TESTMETHOD(append_three_items)
{
    char *items[] = { "one", "two", "three" };

    PC_List_append(notOwningPC_List, items[0]);
    PC_List_append(notOwningPC_List, items[1]);
    PC_List_append(notOwningPC_List, items[2]);

    PT_Test_assertIntEqual(3, PC_List_length(notOwningPC_List), "wrong item count");
    PT_Test_assertStrEqual(items[0], PC_List_get(notOwningPC_List, 0), "unexpected item");
    PT_Test_assertStrEqual(items[1], PC_List_get(notOwningPC_List, 1), "unexpected item");
    PT_Test_assertStrEqual(items[2], PC_List_get(notOwningPC_List, 2), "unexpected item");

    PT_Test_pass();
}

PT_TESTMETHOD(clone_list)
{
    PC_List_append(stringPC_List1, PC_String_copy("foo"));
    PC_List_append(stringPC_List1, PC_String_copy("bar"));
    PC_List_append(stringPC_List1, PC_String_copy("baz"));

    tmpPC_List1 = PC_List_clone(stringPC_List1);

    PT_Test_assertIntEqual(3, PC_List_length(tmpPC_List1), "wrong item count");
    PT_Test_assertStrEqual("bar", PC_List_get(tmpPC_List1, 1), "unexpected item");
    if (PC_List_get(tmpPC_List1, 1) == PC_List_get(stringPC_List1, 1))
    {
        PT_Test_fail("Cloned list references same object");
    }

    PT_Test_pass();
}

PT_TESTMETHOD(join_strings)
{
    PC_List_append(notOwningPC_List, "This");
    PC_List_append(notOwningPC_List, "is");
    PC_List_append(notOwningPC_List, "a");
    PC_List_append(notOwningPC_List, "test");

    tmpStr1 = PC_List_joinStr(notOwningPC_List, " ");

    PT_Test_assertStrEqual("This is a test", tmpStr1, "wrong join result");

    PT_Test_pass();
}

PT_TESTMETHOD(insert_item)
{
    PC_List_append(notOwningPC_List, "A");
    PC_List_append(notOwningPC_List, "B");

    PC_List_insert(notOwningPC_List, 1, "or");

    PT_Test_assertStrEqual("A", PC_List_get(notOwningPC_List, 0), "unexpected item");
    PT_Test_assertStrEqual("or", PC_List_get(notOwningPC_List, 1), "unexpected item");
    PT_Test_assertStrEqual("B", PC_List_get(notOwningPC_List, 2), "unexpected item");

    PT_Test_pass();
}

PT_TESTMETHOD(set_items)
{
    PC_List_append(notOwningPC_List, "A");
    PC_List_append(notOwningPC_List, "B");
    PC_List_append(notOwningPC_List, "C");

    PC_List_set(notOwningPC_List, 63, "X");
    PC_List_set(notOwningPC_List, 1, "Y");

    PT_Test_assertIntEqual(64, PC_List_length(notOwningPC_List), "wrong item count");

    PT_Test_assertStrEqual("A", PC_List_get(notOwningPC_List, 0), "unexpected item");
    PT_Test_assertStrEqual("Y", PC_List_get(notOwningPC_List, 1), "unexpected item");
    PT_Test_assertStrEqual("C", PC_List_get(notOwningPC_List, 2), "unexpected item");
    PT_Test_assertStrEqual("X", PC_List_get(notOwningPC_List, 63), "unexpected item");

    PT_Test_assertRefEqual(0, PC_List_get(notOwningPC_List, 3), "unset item not null");
    PT_Test_assertRefEqual(0, PC_List_get(notOwningPC_List, 62), "unset item not null");

    PT_Test_pass();
}

PT_TESTMETHOD(remove_item)
{
    static char *items[] = {"A", "B", "C"};
    PC_List_append(notOwningPC_List, items[0]);
    PC_List_append(notOwningPC_List, items[1]);
    PC_List_append(notOwningPC_List, items[2]);

    PC_List_remove(notOwningPC_List, items[1]);

    PT_Test_assertIntEqual(2, PC_List_length(notOwningPC_List), "wrong item count");
    PT_Test_assertStrEqual("A", PC_List_get(notOwningPC_List, 0), "unexpected item");
    PT_Test_assertStrEqual("C", PC_List_get(notOwningPC_List, 1), "unexpected item");

    PT_Test_pass();
}

static int stringStartsWithSelector(const void *value, void *arg)
{
    const char *start = arg;
    return !strncmp(start, (const char *)value, strlen(arg));
}

PT_TESTMETHOD(remove_matching_item)
{
    static char *items[] = {"A", "B", "C"};
    PC_List_append(notOwningPC_List, items[0]);
    PC_List_append(notOwningPC_List, items[1]);
    PC_List_append(notOwningPC_List, items[2]);

    PC_List_removeMatching(notOwningPC_List, stringStartsWithSelector, "B");

    PT_Test_assertIntEqual(2, PC_List_length(notOwningPC_List), "wrong item count");
    PT_Test_assertStrEqual("A", PC_List_get(notOwningPC_List, 0), "unexpected item");
    PT_Test_assertStrEqual("C", PC_List_get(notOwningPC_List, 1), "unexpected item");

    PT_Test_pass();
}

PT_TESTMETHOD(invalid_index_gives_0)
{
    PT_Test_assertRefEqual(0, PC_List_get(notOwningPC_List, -1), "invalid index did not return 0");
    PT_Test_assertRefEqual(0, PC_List_get(notOwningPC_List, 0), "invalid index did not return 0");

    PT_Test_pass();
}

PT_TESTMETHOD(select_items)
{
    PC_List_append(stringPC_List1, PC_String_copy("y_FOO"));
    PC_List_append(stringPC_List1, PC_String_copy("n_BAR"));
    PC_List_append(stringPC_List1, PC_String_copy("y_BAZ"));

    tmpPC_List1 = PC_List_select(stringPC_List1, stringStartsWithSelector, "y_");

    PT_Test_assertIntEqual(2, PC_List_length(tmpPC_List1), "wrong item count");
    PT_Test_assertStrEqual("y_BAZ", PC_List_get(tmpPC_List1, 1), "unexpected item");

    PT_Test_pass();
}

static void *stringToLengthTransform(const void *element, void *arg)
{
    (void)arg;
    return (void *)(uintptr_t)strlen((const char *)element);
}

PT_TESTMETHOD(transform_list)
{
    PC_List_append(notOwningPC_List, "one");
    PC_List_append(notOwningPC_List, "two");
    PC_List_append(notOwningPC_List, "three");
    PC_List_append(notOwningPC_List, "fourtytwo");

    tmpPC_List1 = PC_List_transform(notOwningPC_List, 0, 0, stringToLengthTransform, 0);

    PT_Test_assertIntEqual(4, PC_List_length(tmpPC_List1), "wrong item count");
    PT_Test_assertIntEqual(3, (uintptr_t)PC_List_get(tmpPC_List1, 0), "incorrectly transformed");
    PT_Test_assertIntEqual(3, (uintptr_t)PC_List_get(tmpPC_List1, 1), "incorrectly transformed");
    PT_Test_assertIntEqual(5, (uintptr_t)PC_List_get(tmpPC_List1, 2), "incorrectly transformed");
    PT_Test_assertIntEqual(9, (uintptr_t)PC_List_get(tmpPC_List1, 3), "incorrectly transformed");

    PT_Test_pass();
}
