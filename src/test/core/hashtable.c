#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <pocas/core/hashtable.h>
#include <pocas/test/test.h>

TESTCLASS(pocascore::HashTable);

static HashTable *stringTable1;

TESTINIT()
{
    stringTable1 = HashTable_createStrKeyVal(HashTableSize_16);
}

TESTDONE()
{
    HashTable_destroy(stringTable1);
}

TESTMETHOD(set_and_retrieve_three_items)
{
    HashTable_set(stringTable1, "key1", "val1");
    HashTable_set(stringTable1, "key2", "val2");
    HashTable_set(stringTable1, "key3", "val3");

    Test_assertStrEqual("val1", HashTable_get(stringTable1, "key1"), "wrong hashtable entry");
    Test_assertStrEqual("val2", HashTable_get(stringTable1, "key2"), "wrong hashtable entry");
    Test_assertStrEqual("val3", HashTable_get(stringTable1, "key3"), "wrong hashtable entry");
    Test_assertRefEqual(0, HashTable_get(stringTable1, "valX"), "found invalid entry");

    Test_pass();
}
